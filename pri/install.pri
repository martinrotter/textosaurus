# Install all files on Windows.
win32 {
  target.path = $$PREFIX

  lib.files = $$OUT_PWD/../libtextosaurus/libtextosaurus.dll $$OUT_PWD/../libtextosaurus/libtextosaurus.lib
  lib.path = $$PREFIX
  lib.CONFIG = no_check_exist

  INSTALLS += target lib
}

# Install all files on Linux.
unix:!mac:!android {
  target.path = $$PREFIX/bin

  desktop_file.files = ../../resources/desktop/$${APP_REVERSE_NAME}.desktop
  desktop_file.path = $$quote($$PREFIX/share/applications/)

  appdata.files = ../../resources/desktop/$${APP_REVERSE_NAME}.appdata.xml
  appdata.path = $$quote($$PREFIX/share/metainfo/)

  lib.files = $$OUT_PWD/../libtextosaurus/libtextosaurus.so
  lib.path = $$quote($$PREFIX/lib/)
  lib.CONFIG = no_check_exist

  desktop_icon.files = ../../resources/graphics/$${TARGET}.png
  desktop_icon.path = $$quote($$PREFIX/share/icons/hicolor/512x512/apps/)

  headerss.files = $$HEADERS
  headerss.path = $$quote($$PREFIX/include/)

  INSTALLS += target desktop_file desktop_icon appdata lib headerss
}

mac {
  IDENTIFIER = $$APP_REVERSE_NAME
  CONFIG -= app_bundle
  ICON = ../../resources/graphics/$${TARGET}.icns
  QMAKE_MAC_SDK = macosx10.12
  QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
  LIBS += -framework AppKit

  QMAKE_POST_LINK += $$system(install_name_tool -change "libtextosaurus.dylib" "@executable_path/libtextosaurus.dylib" $$OUT_PWD/textosaurus)

  target.path = $$quote($$PREFIX/Contents/MacOS/)

  lib.files = $$OUT_PWD/../libtextosaurus/libtextosaurus.dylib
  lib.path = $$quote($$PREFIX/Contents/MacOS/)
  lib.CONFIG = no_check_exist

  readme.files = ../../resources/macosx/README_MACOSX
  readme.path = $$quote($$PREFIX/)

  # Install app icon.
  icns_icon.files = ../../resources/graphics/$${TARGET}.icns
  icns_icon.path = $$quote($$PREFIX/Contents/Resources/)

  # Install Info.plist.
  info_plist.files = ../../resources/macosx/Info.plist.in
  info_plist.path  = $$quote($$PREFIX/Contents/)

  # Process the just installed Info.plist.
  info_plist2.extra = @sed -e "s,@EXECUTABLE@,$$TARGET,g" -e "s,@SHORT_VERSION@,$$APP_VERSION,g" -e "s,@APP_NAME@,\"$$APP_NAME\",g" -e "s,@ICON@,$$basename(ICON),g"  -e "s,@TYPEINFO@,"????",g" $$shell_quote($$PREFIX/Contents/Info.plist.in) > $$shell_quote($$PREFIX/Contents/Info.plist) && \
                      rm -f $$shell_quote($$PREFIX/Contents/Info.plist.in)
  info_plist2.path = $$quote($$PREFIX/Contents/)

  # Install PkgInfo
  pkginfo.extra = @printf "APPL????" > $$shell_quote($$PREFIX/Contents/PkgInfo)
  pkginfo.path = $$quote($$PREFIX/Contents/)

  INSTALLS += target lib icns_icon info_plist info_plist2 pkginfo readme
}
