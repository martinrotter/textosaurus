-- txt2tags LPeg lexer.
-- (developed and tested with Txt2tags Markup Rules
-- [https://txt2tags.org/doc/english/rules.t2t])
-- Contributed by Julien L.

local lexer = require('lexer')
local token, word_match = lexer.token, lexer.word_match
local P, R, S = lpeg.P, lpeg.R, lpeg.S
local nonspace = lexer.any - lexer.space

local lex = lexer.new('txt2tags')

-- Whitespace.
local ws = token(lexer.WHITESPACE, (lexer.space - lexer.newline)^1)

-- Titles
local alphanumeric = R('AZ') + R('az') + R('09') + P('_') + P('-')
local header_label = token('header_label_start', '[') *
                     token('header_label', alphanumeric^1) *
                     token('header_label_end', ']')
local header = (token('h5', ('=====' * (lexer.nonnewline - '=')^1 * '=====') +
                            ('+++++' * (lexer.nonnewline - '+')^1 * '+++++')) *
                header_label^-1) +
               (token('h4', ('====' * (lexer.nonnewline - '=')^1 * '====') +
                            ('++++' * (lexer.nonnewline - '+')^1 * '++++')) *
                header_label^-1) +
               (token('h3', ('===' * (lexer.nonnewline - '=')^1 * '===') +
                            ('+++' * (lexer.nonnewline - '+')^1 * '+++')) *
                header_label^-1) +
               (token('h2', ('==' * (lexer.nonnewline - '=')^1 * '==') +
                            ('++' * (lexer.nonnewline - '+')^1 * '++')) *
                header_label^-1) +
               (token('h1', ('=' * (lexer.nonnewline - '=')^1 * '=') +
                            ('+' * (lexer.nonnewline - '+')^1 * '+')) *
                header_label^-1)

-- Comments.
local line_comment = lexer.starts_line('%') * lexer.nonnewline^0
local block_comment = lexer.starts_line('%%%') *
                      (lexer.space - lexer.newline)^0 * lexer.newline *
                      (lexer.any - '%%%')^0 * lexer.starts_line('%%%')^-1
local comment = token(lexer.COMMENT, block_comment + line_comment)

-- Inline.
local bold = token('bold', ('**' * nonspace * '**' * S('*')^0) +
                           ('**' * nonspace *
                            (lexer.nonnewline - (nonspace * '**'))^0 *
                            nonspace * '**' * S('*')^0))
local italic = token('italic', ('//' * nonspace * '//' * S('/')^0) +
                               ('//' * nonspace *
                                (lexer.nonnewline - (nonspace * '//'))^0 *
                                nonspace * '//' * S('/')^0))
local underline = token('underline', ('__' * nonspace * '__' * S('_')^0) +
                                     ('__' * nonspace *
                                      (lexer.nonnewline - (nonspace * '__'))^0 *
                                      nonspace * '__' * S('_')^0))
local strike = token('strike', ('--' * nonspace * '--' * S('-')^0) +
                               ('--' * nonspace *
                                (lexer.nonnewline - (nonspace * '--'))^0 *
                                nonspace * '--' * S('-')^0))
local mono = token('mono', ('``' * nonspace * '``' * S('`')^0) +
                           ('``' * nonspace *
                            (lexer.nonnewline - (nonspace * '``'))^0 *
                            nonspace * '``' * S('`')^0))
local raw = token('raw', ('""' * nonspace * '""' * S('"')^0) +
                         ('""' * nonspace *
                          (lexer.nonnewline - (nonspace * '""'))^0 * nonspace *
                          '""' * S('"')^0))
local tagged = token('tagged', ('\'\'' * nonspace * '\'\'' * S('\'')^0) +
                               ('\'\'' * nonspace *
                                (lexer.nonnewline - (nonspace * '\'\''))^0 *
                                nonspace * '\'\'' * S('\'')^0))
local inline = bold + italic + underline + strike + mono + raw + tagged

-- Link.
local email = token('email', (nonspace - '@')^1 * '@' * (nonspace - '.')^1 *
                             ('.' * (nonspace - '.' - '?')^1)^1 *
                             ('?' * nonspace^1)^-1)
local host = token('host', (P('www') + P('WWW') + P('ftp') + P('FTP')) *
                           (nonspace - '.')^0 * '.' * (nonspace - '.')^1 * '.' *
                           (nonspace - ',' - '.')^1)
local url = token('url', (nonspace - '://')^1 * '://' *
                         (nonspace - ',' - '.')^1 *
                         ('.' * (nonspace - ',' - '.' - '/' - '?' - '#')^1)^1 *
                         ('/' * (nonspace - '.' - '/' - '?' - '#')^0 *
                          ('.' * (nonspace - ',' - '.' - '?' - '#')^1)^0)^0 *
                         ('?' * (nonspace - '#')^1)^-1 * ('#' * nonspace^0)^-1)
local label_with_address = token('label_start', '[') *
                           lexer.space^0 *
                           token('address_label', ((nonspace - ']')^1 *
                                                   lexer.space^1)^1) *
                           token('address', (nonspace - ']')^1) *
                           token('label_end', ']')
local link = label_with_address + url + host + email

-- Line.
local line = token('line', (P('-') + P('=') + P('_'))^20)

-- Image.
local image_only = token('image_start', '[') *
                   token('image', (nonspace - ']')^1) * token('image_end', ']')
local image_link = token('image_link_start', '[') * image_only *
                   token('image_link_sep', lexer.space^1) *
                   token('image_link', (nonspace - ']')^1) *
                   token('image_link_end', ']')
local image = image_link + image_only

-- Macro.
local macro = token('macro', '%%' * (nonspace - '(')^1 *
                             ('(' * (lexer.nonnewline - ')')^0 * ')')^-1)

-- Verbatim.
local verbatim_line = lexer.starts_line('```') * (lexer.space - lexer.newline) *
                      lexer.nonnewline^0
local verbatim_block = lexer.starts_line('```') *
                       (lexer.space - lexer.newline)^0 * lexer.newline *
                       (lexer.any - '```')^0 * lexer.starts_line('```')^-1
local verbatim_area = token('verbatim_area', verbatim_block + verbatim_line)

-- Raw.
local raw_line = lexer.starts_line('"""') * (lexer.space - lexer.newline) *
                 lexer.nonnewline^0
local raw_block = lexer.starts_line('"""') * (lexer.space - lexer.newline)^0 *
                  lexer.newline * (lexer.any - '"""')^0 *
                  lexer.starts_line('"""')^-1
local raw_area = token('raw_area', raw_block + raw_line)

-- Tagged.
local tagged_line = lexer.starts_line('\'\'\'') *
                    (lexer.space - lexer.newline) * lexer.nonnewline^0
local tagged_block = lexer.starts_line('\'\'\'') *
                     (lexer.space - lexer.newline)^0 * lexer.newline *
                     (lexer.any - '\'\'\'')^0 * lexer.starts_line('\'\'\'')^-1
local tagged_area = token('tagged_area', tagged_block + tagged_line)

-- Table.
local table_sep = token('table_sep', '|')
local cell_content = inline + link + image + macro +
                     token('cell_content', lexer.nonnewline - ' |')
local header_cell_content = token('header_cell_content',
                                  lexer.nonnewline - ' |')
local field_sep = ' ' * table_sep^1 * ' '
local table_row_end = P(' ')^0 * table_sep^0
local table_row = lexer.starts_line(P(' ')^0 * table_sep) * cell_content^0 *
                  (field_sep * cell_content^0)^0 * table_row_end
local table_row_header = lexer.starts_line(P(' ')^0 * table_sep * table_sep) *
                         header_cell_content^0 *
                         (field_sep * header_cell_content^0)^0 * table_row_end
local table = table_row_header + table_row

lex:add_rule('table', table)
lex:add_rule('link', link)
lex:add_rule('line', line)
lex:add_rule('header', header)
lex:add_rule('comment', comment)
lex:add_rule('whitespace', ws)
lex:add_rule('image', image)
lex:add_rule('macro', macro)
lex:add_rule('inline', inline)
lex:add_rule('verbatim_area', verbatim_area)
lex:add_rule('raw_area', raw_area)
lex:add_rule('tagged_area', tagged_area)

local font_size = lexer.property_int['fontsize'] > 0 and
                  lexer.property_int['fontsize'] or 10
local hstyle = 'fore:$(color.red)'

lex:add_style('line', 'bold')
lex:add_style('h5', hstyle..',size:'..(font_size + 1))
lex:add_style('h4', hstyle..',size:'..(font_size + 2))
lex:add_style('h3', hstyle..',size:'..(font_size + 3))
lex:add_style('h2', hstyle..',size:'..(font_size + 4))
lex:add_style('h1', hstyle..',size:'..(font_size + 5))
lex:add_style('header_label', lexer.STYLE_LABEL)
lex:add_style('email', 'underlined')
lex:add_style('host', 'underlined')
lex:add_style('url', 'underlined')
lex:add_style('address_label', lexer.STYLE_LABEL)
lex:add_style('address', 'underlined')
lex:add_style('image', 'fore:$(color.green)')
lex:add_style('image_link', 'underlined')
lex:add_style('macro', lexer.STYLE_PREPROCESSOR)
lex:add_style('bold', 'bold')
lex:add_style('italic', 'italics')
lex:add_style('underline', 'underlined')
lex:add_style('strike', 'italics') -- a strike style is not available
lex:add_style('mono', 'font:mono')
lex:add_style('raw', 'back:$(color.grey)')
lex:add_style('tagged', lexer.STYLE_EMBEDDED)
lex:add_style('verbatim_area', 'font:mono') -- in consistency with mono
lex:add_style('raw_area', 'back:$(color.grey)') -- in consistency with raw
lex:add_style('tagged_area', lexer.STYLE_EMBEDDED) -- in consistency with tagged
lex:add_style('table_sep', 'fore:$(color.green)')
lex:add_style('header_cell_content', 'fore:$(color.green)')

return lex
