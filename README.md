# Tagit

Tagit is a simple CLI utility that sets the artist and title
tags of music files in a certain directory based on the filename.
Specifically the file name should be in the form: "Artist - Title.Extension"

## Example

If your music directory is lets say ~/Music and it contains ogg vorbis and
ogg opus files in the above form, then all you have to do is run
**tagit ~/Music opus ogg**

## Build Requirements:
- Autoconf
- Automake
- TagLib C bindings develepoment headers

## Build Instructions:
- autoreconf -ivf
- configure
- make

To enable debugging run **./configure --enable-debug**.

## Install/Uninstall Instructions:
- sudo make install
- sudo make uninstall