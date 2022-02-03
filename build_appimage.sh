#!/bin/bash
set -xeu

HERE="$(dirname "$(readlink -f "${0}")")"
APPDIR="${HERE}/AppDir"

create_appimage(){
    folder=$(mktemp -d)
    appimagetool="$folder/appimage"
    wget -O "$appimagetool" https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage
    chmod +x "$appimagetool"
    $appimagetool "${APPDIR}" ytwatch.AppImage
    echo "readlink -f $(pwd)/ytwatch"
    rm -rf "$folder"
}

compile_ytscraper(){
    make && mv ${HERE}/ytscraper "${APPDIR}/usr/bin"
}

mkdir -p "${APPDIR:?}/usr/bin"

cat > "${APPDIR}/AppRun" <<EOF
#!/bin/sh
HERE="\$(dirname "\$(readlink -f "\${0}")")"
YTWATCH_SCRAPER=\$HERE/usr/bin/ytscraper PATH="\$HERE/usr/bin:\$PATH" \${HERE}/usr/bin/ytwatch
EOF

chmod +x ${APPDIR}/AppRun

cat > ${APPDIR}/ytwatch.desktop <<EOF
[Desktop Entry]
Name=ytwatch
Exec=ytwatch
Icon=icon
Type=Application
Categories=Utility;
EOF

cp ./ytwatch "${APPDIR}/usr/bin"
cp ./ytwatch.png "${APPDIR}/icon.png"

compile_ytscraper
create_appimage
rm -rf ${APPDIR}
