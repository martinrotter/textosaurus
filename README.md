Textilosaurus
=============
[![AppVeyor](https://img.shields.io/appveyor/ci/martinrotter/textilosaurus.svg?maxAge=360)](https://ci.appveyor.com/project/martinrotter/textilosaurus)
[![Travis CI](https://img.shields.io/travis/martinrotter/textilosaurus.svg?maxAge=360)](https://travis-ci.org/martinrotter/textilosaurus)
[![Total downloads](https://img.shields.io/github/downloads/martinrotter/textilosaurus/total.svg?maxAge=360)](http://www.somsubhra.com/github-release-stats/?username=martinrotter&repository=textilosaurus)
[![Version](https://img.shields.io/github/release/martinrotter/textilosaurus.svg?maxAge=360)](https://raw.githubusercontent.com/martinrotter/textilosaurus/master/resources/text/CHANGELOG)
[![GitHub issues](https://img.shields.io/github/issues/martinrotter/textilosaurus.svg?maxAge=360)](https://github.com/martinrotter/textilosaurus/issues)
[![License](https://img.shields.io/github/license/martinrotter/textilosaurus.svg?maxAge=360000)](https://github.com/martinrotter/textilosaurus/blob/master/LICENSE.md)

<img align="left" src="https://raw.githubusercontent.com/martinrotter/textilosaurus/master/resources/graphics/textilosaurus.png" alt="Textilosarus" height="100px" />

Textilosaurus is simple cross-platform UTF-8 text editor based on Qt and Scintilla. Textilosaurus aims to provide similar workflow as [Notepad++](https://notepad-plus-plus.org) does.

You can support author of Textilosaurus on [PATREON](https://www.patreon.com/martinrotter). Textilosaurus icon was created by [http://www.fasticon.com](http://www.fasticon.com).

Features
--------

* UI:
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
    - switchable encoding for each separate document,
    - **uses full UTF-8 internally**,
    - can load relatively big files (100 MB file should not be a problem),
    - can **print files including syntax highlighting** and printing to PDF,
    - hideable line numbers,
    - adjustable vertical line height (responds to `SHIFT+WHEEL UP/DOWN` shortcut too),
    - adjustable editor font (size can be changed also via `CTRL+WHEEL UP/DOWN` shortcut),
    - can display **ligatures** (tested with Fira Code on GNU/Linux and Windows),
    - simple find/replace functionality, which allows you to:
        * search forward/backward for plain string or regular expression (ECMAScript),
        * display number of occurrences of search phrase in a document.
    - switchable EOL mode for new documents,
    - EOL mode auto-detection,
    - **syntax highlighting** for many text formats,
	- inituitive filtering for menu of available syntax highlighters,
    - conversion of existing EOLs,
	- switchable visibility of EOL/whitespace characters,
    - support for **many text operations**, which allow you to:
        * upload selection/document to pastebin sites,
        * convert to/from Base64 and Base64Url,
        * convert to/from URL encoded text,
        * convert to HTML escaped text, 
        * insert current date/time,
	- support for **external scripts**, which allow you to:
	    * pass current selection/line/document/file to your script,
		* run your script with arbitrary interpreter (Python, Bash or some other executable),
		* get output of your script (as new file, replace selection/document, copy to clipboard, ...),
		* scripts run in worker threads (multithreaded environment),
		* **settings of external tools is completely portable** (*),
* Misc:
    - **all user data (settings, external tools definitions, ...) are fully portable** (You can use the same user data shared via Dropbox on all supported platforms.),
    - depends only on Qt (Scintilla is bundled),
    - relatively lightweight,
    - no JavaScript components, no bundled Chromium, no Electron, no web technologies inside,
    - always OSS.
	
\* Usually, custom external tools are executed via Bash or Python, so you must have used interpreter available in all environments where Textilosaurus is used. On Windows, Cygwin is probably the best way to use these.
