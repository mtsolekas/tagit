# Tagit

Tagit is a simple CLI utility that sets the artist and title
tags of music files in a certain directory based on the filename.
Specifically the file name should be in the form: "Artist - Title.Extension"

## Example

If the directory you want to edit is ~/Music and it contains ogg
vorbis and ogg opus files in the above form, then all you have to
do is run **tagit ~/Music -f opus,ogg** or simply **tagit ~/Music**

The previous invokation method of **tagit ~/Music opus ogg** is still
valid, but if mixed with the --format option only the option will be
taken into account.

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
