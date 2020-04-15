// This file is distributed under GNU GPLv3 license. For full license text, see <project-git-repository-root-folder>/LICENSE.md.

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QtGlobal>

// *INDENT-OFF*
#define IDENT(x) x
#define XSTR(x) #x
#define STR(x) XSTR(x)
#define PATH(x, y) STR(IDENT(x)IDENT(y))

// *INDENT-ON*

constexpr auto DATA_CLIPBOARD_TOO_BIG_WARNING = 30 * 1024;
constexpr auto CRYPTO_RECOMMENDED_PASS_LENGTH = 8;
constexpr char CRYPTO_FORMAT_BOUNDARY = 30;
constexpr auto CRYPTED_FILE_ENCODING = "UTF-8";
constexpr auto FIND_RESULT_ENCLOSING_CHAR_COUNT = 8;
constexpr auto INDICATOR_URL = 0;
constexpr auto INDICATOR_FIND = 1;
constexpr auto MARGIN_LINE_NUMBERS_MIN_WIDTH = 10;
constexpr auto MARGIN_LINE_NUMBERS = 0;
constexpr auto MARGIN_SYMBOLS = 1;
constexpr auto MARGIN_FOLDING = 2;
constexpr auto MARGIN_LINE_NUMBERS_RIGHT_SPACE = 4 + 1; // SC_MAX_MARGIN = 4
constexpr auto MARGIN_PADDING_LINE_NUMBERS = 7;
constexpr auto MARGIN_WIDTH_FOLDING = 18;
constexpr auto LINE_SPACING_MIN = -10;
constexpr auto LINE_SPACING_MAX = 50;
constexpr auto CHANGE_EVENT_DELAY = 250;
constexpr auto FS_SIDEBAR_PATH_LINES = 4;
constexpr auto TRAY_ICON_BUBBLE_TIMEOUT = 15000;
constexpr auto FORMAT_DATETIME_OUTPUT = "yyyy-MM-dd HH:mm:ss";
constexpr auto FILE_CHUNK_LENGTH_FOR_EOL_DETECTION = 1024;
constexpr auto FILE_CHUNK_LENGTH_FOR_ENCODING_DETECTION = 2048;
constexpr auto DEFAULT_TEXT_FILE_ENCODING = "UTF-8";
constexpr auto BIG_TEXT_FILE_SIZE = qint64(15000000);
constexpr auto MAX_TEXT_FILE_SIZE = qint64(std::numeric_limits<int>::max());
constexpr auto ARGUMENTS_LIST_SEPARATOR = "\n";
constexpr auto RELEASES_LIST = "https://api.github.com/repos/martinrotter/textosaurus/releases";
constexpr auto DEFAULT_LOCALE = "en";
constexpr auto ICON_SIZE_SETTINGS = 16;
constexpr auto DOWNLOAD_TIMEOUT = 20000;
constexpr auto ELLIPSIS_LENGTH = 3;
constexpr auto FLAG_ICON_SUBFOLDER = "flags";
constexpr auto SPACER_ACTION_NAME = "spacer";
constexpr auto SEPARATOR_ACTION_NAME = "separator";
constexpr auto MAX_RECENT_FILES = 25;
constexpr auto ENCRYPTION_FILE_NAME = "key.private";
constexpr auto EXT_TOOLS_CONFIG = "external_tools.ini";
constexpr auto EXT_TOOL_INTERPRETER = "bash";
constexpr auto PASTEBIN_HASTE_POST = "https://hastebin.com/documents";
constexpr auto PASTEBIN_HASTE = "https://hastebin.com/";
constexpr auto PASTEBIN_CLBIN = "https://clbin.com";
constexpr auto PASTEBIN_IXIO = "http://ix.io";
constexpr auto HTTP_HEADERS_CONTENT_TYPE = "Content-Type";
constexpr auto HTTP_HEADERS_USER_AGENT = "User-Agent";
constexpr auto APP_CFG_FILE = "config.ini";
constexpr auto APP_STYLE_DEFAULT = "Fusion";
constexpr auto APP_THEME_DEFAULT = "Papirus";
constexpr auto APP_NO_THEME = "";
constexpr auto APP_THEME_SUFFIX = ".png";
constexpr auto APP_QUIT_INSTANCE_SHORT = "q";
constexpr auto APP_QUIT_INSTANCE = "quitapp";
constexpr auto APP_NOSINGLE_INSTANCE_SHORT = "n";
constexpr auto APP_NOSINGLE_INSTANCE = "no-single-instance";
constexpr auto APP_OPT_CONFIG_SHORT = "c";
constexpr auto APP_OPT_CONFIG = "config";
constexpr auto APP_NAT_DLGS_CONFIG = "nativedialogs";
constexpr auto APP_IS_RUNNING_SHORT = "r";
constexpr auto APP_IS_RUNNING = "appisrunning";

#ifndef QSL

// Thin macro wrapper for literal strings.
// They are much more memory efficient and faster.
// Use it for all literals except for two cases:
//  a) Methods which take QLatin1String (use QLatin1String for literal argument too),
//  b) Construction of empty literals "", use QString() instead of QStringLiteral("").
#define QSL(x) QStringLiteral(x)
#endif

#ifndef qDebugNN
#define qDebugNN qDebug().noquote().nospace()
#endif

#ifndef qWarningNN
#define qWarningNN qWarning().noquote().nospace()
#endif

#ifndef qCriticalNN
#define qCriticalNN qCritical().noquote().nospace()
#endif

#ifndef qFatalNN
#define qFatalNN qFatal().noquote().nospace()
#endif

#ifndef qInfoNN
#define qInfoNN qInfo().noquote().nospace()
#endif

#ifndef QL1S

// Macro for latin strings. Latin strings are
// faster than QStrings created from literals.
#define QL1S(x) QLatin1String(x)
#endif

#ifndef QL1C

// Macro for latin chars.
#define QL1C(x) QLatin1Char(x)
#endif

#define IS_IN_ARRAY(idx, count_array) ((idx) >= 0 && (idx) < (count_array))

#if defined(Q_OS_LINUX)
constexpr auto OS_ID = "Linux";
constexpr auto OS_ID_LOW = "linux";
constexpr auto APP_DESKTOP_ENTRY_FILE = "io.github.martinrotter.textosaurus.desktop";
constexpr auto APP_DESKTOP_ENTRY_PATH = ":/desktop";

#elif defined(Q_OS_OSX)
constexpr auto OS_ID = "Mac OS X";
constexpr auto OS_ID_LOW = "macosx";

#elif defined(Q_OS_WIN)
constexpr auto OS_ID = "Windows";
constexpr auto OS_ID_LOW = "windows";

#elif defined(Q_OS_ANDROID)
constexpr auto OS_ID = "Android";
constexpr auto OS_ID_LOW = "android";

#else
constexpr auto OS_ID = "";
constexpr auto OS_ID_LOW = "";

#endif

// Paths.
constexpr auto APP_THEME_PATH = ":/graphics";
constexpr auto APP_INFO_PATH = ":/text";
constexpr auto APP_ICON_PATH = ":/graphics/textosaurus.png";
constexpr auto APP_LANG_PATH = ":/localization";

#endif // DEFINITIONS_H
