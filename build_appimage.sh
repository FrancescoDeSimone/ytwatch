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

compile_devour(){
  folder=$(mktemp -d)
  pushd ${folder}
  git clone --depth 1 https://github.com/salman-abedin/devour.git
  pushd devour
  make install BIN_DIR="${APPDIR}/usr/bin"
  popd
  popd
  rm -rf "$folder"

}

compile_ueberzug(){
  folder=$(mktemp -d)
  pushd ${folder}
  git clone --depth 1 https://github.com/jstkdng/ueberzugpp.git
  pushd ueberzugpp
  mkdir build && pushd build
  cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_OPENCV=OFF  ..
  cmake --build .
  mv ueberzug "${APPDIR}/usr/bin"
  popd
  popd
  popd
  rm -rf "$folder"
}

compile_ytscraper(){
    pushd "${HERE}"
    make
    mv "${HERE}/ytscraper" "${APPDIR}/usr/bin"
    popd
}

download_fzf(){
  folder=$(mktemp -d)
  pushd ${folder}
  wget https://github.com/junegunn/fzf/releases/download/0.44.1/fzf-0.44.1-linux_amd64.tar.gz
  tar xf fzf-0.44.1-linux_amd64.tar.gz
  mv fzf "${APPDIR}/usr/bin"
  popd
  rm -rf "$folder"
}

mkdir -p "${APPDIR:?}/usr/bin"

cat > "${APPDIR}/AppRun" <<EOF
#!/bin/sh
HERE="\$(dirname "\$(readlink -f "\${0}")")"
YTWATCH_FZF=\$HERE/usr/bin/fzf YTWATCH_UEBERZUGPP=\$HERE/usr/bin/ueberzug YTWATCH_SCRAPER=\$HERE/usr/bin/devour YTWATCH_SCRAPER=\$HERE/usr/bin/ytscraper PATH="\$HERE/usr/bin:\$PATH" \${HERE}/usr/bin/ytwatch
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
compile_devour
download_fzf
compile_ueberzug
create_appimage
rm -rf ${APPDIR}
