package_name="bomberman_0.1-1_amd64"

# copy executable and create directory structure
mkdir $package_name
mkdir -p ${package_name}/usr/local/bin
cp build/Bomberman ${package_name}/usr/local/bin/bomberman_helper
echo "/usr/local/bin/bomberman_helper /usr/local/games/bomberman/media/" > \
      ${package_name}/usr/local/bin/Bomberman
chmod +x ${package_name}/usr/local/bin/Bomberman
mkdir -p ${package_name}/usr/local/games/bomberman
cp -r build/media ${package_name}/usr/local/games/bomberman/

# create control file
mkdir ${package_name}/DEBIAN
echo """Package: Bomberman
Version: 0.1
Architecture: amd64
Maintainer: Frantisek Trebuna <ferotre@gmail.com>
Description: Implementation of bomberman, which is playable over the local network.
Depends: libc6 (>= 2.30), libfreetype6 (>= 2.2.1), libgcc-s1 (>= 3.0), libstdc++6 (>= 9), libudev1 (>= 183), libx11-6, libxcursor1 (>> 1.1.2), libxrandr2 (>= 2:1.2.99.3)
""" > ${package_name}/DEBIAN/control

desktop_launcher="/usr/share/applications/Bomberman.desktop"
echo """#!/bin/sh
sudo echo \"\"\"[Desktop Entry]
Type=Application
Exec=/usr/local/bin/Bomberman
Name=Bomberman 0.1
GenericName=Bomberman
Categories=Game;
\"\"\" > ${desktop_launcher}
sudo update-desktop-database
""" > ${package_name}/DEBIAN/postinst

echo """#!/bin/sh
sudo rm ${desktop_launcher}
""" > ${package_name}/DEBIAN/prerm

chmod +x ${package_name}/DEBIAN/postinst
chmod +x ${package_name}/DEBIAN/prerm

# build the deb package
dpkg-deb --build --root-owner-group ${package_name}
rm -r $package_name
