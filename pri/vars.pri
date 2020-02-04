APP_NAME                      = "Textosaurus"
APP_LOW_NAME                  = "textosaurus"
APP_REVERSE_NAME              = "io.github.martinrotter.textosaurus"
APP_AUTHOR                    = "Martin Rotter"
APP_COPYRIGHT                 = "(C) 2017-2019 $$APP_AUTHOR"
APP_VERSION                   = "0.9.13"
APP_LONG_NAME                 = "$$APP_NAME $$APP_VERSION"
APP_EMAIL                     = "rotter.martinos@gmail.com"
APP_URL                       = "https://github.com/martinrotter/textosaurus"
APP_URL_ISSUES                = "https://github.com/martinrotter/textosaurus/issues"
APP_URL_ISSUES_NEW            = "https://github.com/martinrotter/textosaurus/issues/new"
APP_URL_WIKI                  = "https://github.com/martinrotter/textosaurus/wiki"
APP_USERAGENT                 = "Textosaurus/$$APP_VERSION (io.github.martinrotter.textosaurus)"
APP_DONATE_URL                = "https://martinrotter.github.io/donate"
APP_WIN_ARCH                  = "win64"

message($$MSG_PREFIX: Welcome to $$MSG_PREFIX qmake script.)

lessThan(QT_MAJOR_VERSION, 5)|lessThan(QT_MINOR_VERSION, 7) {
  error($$MSG_PREFIX: At least Qt \"5.7.0\" is required!!!)
}
