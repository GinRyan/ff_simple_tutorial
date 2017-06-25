#!/bin/sh
export FFMPEG_VERSION="3.3.2"
export FFMPEG_SRC_DIR="ffmpeg-$FFMPEG_VERSION"
export FFMPEG_SRC_ARC_FILENAME="$FFMPEG_SRC_DIR.tar.bz2"
export DOWNLOAD_URL="http://ffmpeg.org/releases/$FFMPEG_SRC_ARC_FILENAME"

if [ ! -f "$FFMPEG_SRC_ARC_FILENAME" ]; then
  wget $DOWNLOAD_URL
fi

if [ ! -d "$FFMPEG_SRC_DIR" ]; then
  tar -jxvf $FFMPEG_SRC_ARC_FILENAME
fi

cd $FFMPEG_SRC_DIR
export OUT_PREFIX=`pwd`/../ffmpeg-dev

./configure --prefix=$OUT_PREFIX \
 	--disable-gpl \
	--enable-static \
	--enable-shared \

make -j`cat /proc/cpuinfo|grep processor|wc -l` & make install
cd ..
rm -rf $FFMPEG_SRC_DIR
