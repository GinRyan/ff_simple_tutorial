export PREFIX=`pwd`/../SDL-dev

./configure --prefix=$PREFIX  

make -j`cat /proc/cpuinfo|grep processor|wc -l` & make install
