
echo "Cleaning old files"
rm -f *.o *.img

KVER=$(uname -r)
HEADERS_INCLUDE=/usr/src/kernel-headers-$KVER/include

echo "Building disk.c ..."
gcc -D__KERNEL__ -I $HEADERS_INCLUDE -Wall -Wstrict-prototypes -Wno-trigraphs -Os -fno-strict-aliasing -fno-common -fomit-frame-pointer -pipe -mpreferred-stack-boundary=2 -march=i386 -DMODULE -DMODVERSIONS -include $HEADERS_INCLUDE/linux/modversions.h -nostdinc -iwithprefix include -DKBUILD_BASENAME=disk -c -o disk.o disk.c

echo "Building main.c ..."
gcc -D__KERNEL__ -I $HEADERS_INCLUDE -Wall -Wstrict-prototypes -Wno-trigraphs -Os -fno-strict-aliasing -fno-common -fomit-frame-pointer -pipe -mpreferred-stack-boundary=2 -march=i386 -DMODULE -DMODVERSIONS -include $HEADERS_INCLUDE/linux/modversions.h -nostdinc -iwithprefix include -DKBUILD_BASENAME=main -c -o main.o main.c

echo "Building kernel module ..."
ld -m elf_i386 -e stext -r -o midump-$KVER.o disk.o main.o

echo "List kernel module"
ls -lh midump-$KVER.o

echo "Get module info"
modinfo midump-$KVER.o

echo "Loading module .."
sudo insmod midump-$KVER.o

echo "Dump finished"
ls -lh *.img

echo "Removing module .."
sudo rmmod midump-$KVER

echo "All operations finished"

