#!/bin/sh
export SDL_VER="2.0.5"
export SDL_SRC_DIR="SDL2-$SDL_VER"
export SDL_SRC_ARC_FILE="$SDL_SRC_DIR.tar.gz"
export DL_URL="http://www.libsdl.org/release/$SDL_SRC_ARC_FILE"

if [ ! -f "$SDL_SRC_ARC_FILE" ]; then 
   wget $DL_URL
fi

if [ ! -d "$SDL_SRC_DIR" ]; then 
   tar -xzvf $SDL_SRC_ARC_FILE
fi

cd $SDL_SRC_DIR

export OUT_PREFIX=`pwd`/../SDL-dev

./configure --prefix=$OUT_PREFIX  

make -j`cat /proc/cpuinfo|grep processor|wc -l` & make install

cd ..
rm -rf $SDL_SRC_DIR
