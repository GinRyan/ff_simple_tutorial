export PREFIX=`pwd`/../ffmpeg-dev

./configure --prefix=$PREFIX --disable-gpl --enable-static --enable-shared --enable-pic 

make -j`cat /proc/cpuinfo|grep processor|wc -l` & make install

