Textilosaurus
=============
[![AppVeyor](https://img.shields.io/appveyor/ci/martinrotter/textilosaurus.svg?maxAge=360)](https://ci.appveyor.com/project/martinrotter/textilosaurus)
[![Travis CI](https://img.shields.io/travis/martinrotter/textilosaurus.svg?maxAge=360)](https://travis-ci.org/martinrotter/textilosaurus)
[![Total downloads](https://img.shields.io/github/downloads/martinrotter/textilosaurus/total.svg?maxAge=360)](http://www.somsubhra.com/github-release-stats/?username=martinrotter&repository=textilosaurus)
[![Version](https://img.shields.io/github/release/martinrotter/textilosaurus.svg?maxAge=360)](https://raw.githubusercontent.com/martinrotter/textilosaurus/master/resources/text/CHANGELOG)
[![GitHub issues](https://img.shields.io/github/issues/martinrotter/textilosaurus.svg?maxAge=360)](https://github.com/martinrotter/textilosaurus/issues)
[![License](https://img.shields.io/github/license/martinrotter/textilosaurus.svg?maxAge=360000)](https://github.com/martinrotter/textilosaurus/blob/master/LICENSE.md)

<img align="left" src="https://raw.githubusercontent.com/martinrotter/textilosaurus/master/resources/graphics/textilosaurus.png" alt="Textilosarus" height="100px" />

Textilosaurus is simple cross-platform UTF-8 text editor based on Qt and QScintilla. Textilosaurus aims to provide similar workflow as [Notepad++](https://notepad-plus-plus.org) does.

You can support author of Textilosaurus on [PATREON](https://www.patreon.com/martinrotter). Used Textilosaurus icon was created by [http://www.fasticon.com](http://www.fasticon.com).

Features
--------

* User interface:
    - cross-platform look & feel,
    - **configurable toolbar**
    - changeable icon themes and styles,
    - main window remembers its location and size,
    - hideable toolbar and statusbar,
    - simple interface,
    - tabbed interface.
* Text editor component:
    - **supports a lot of input encodings**,
    - input file encoding detection (via bundled [uchardet](https://www.freedesktop.org/wiki/Software/uchardet/)),
    - **uses full UTF-8 internally**,
    - can load relatively big files (100 MB file should not be a problem),
    - switchable EOL mode for new documents,
    - conversion of existing EOLs,
	- switchable visibility of EOL/whitespace characters,
    - support for **many text operations**, which allow you to:
        * upload selection/document to pastebin sites,
        * convert to/from Base64 and Base64Url,
        * insert current date/time.
* Misc:
    - all user data (settings, external tools definitions, ...) are fully portable (You can use the same user data shared via Dropbox on all supported platforms.),
    - depends only on Qt and QScintilla,
    - relatively lightweight,
    - no JavaScript components, no bundled Chromium, no Electron, no web technologies inside,
    - always OSS.