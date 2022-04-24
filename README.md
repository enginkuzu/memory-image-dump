# Memory Image Dump (MIDump)
Memory image dump Linux kernel module for B!nalyze Coding Challenge. 

### Test Results
```
Distro                  Kernel      Arch        Test Result
---------------------------------------------------------------------------------------
Ubuntu 20.04.4 LTS      5.13.0      x86_64      SUCCESS
Debian 11.2             5.10.0      x86_64      SUCCESS
Debian 10.12            4.19.0      x86_64      SUCCESS
Debian 9.3              4.9.0       x86_64      SUCCESS
Debian 8.11             3.16.0      x86_64      FAILED : (BUG) scheduling while atomic
Debian 7.11             3.2.0       x86_64      FAILED : (BUG) scheduling while atomic
Debian 6.0.10           2.6.32      x86_64      SUCCESS
Debian 5.0.10           2.6.26      x86_64      SUCCESS
Debian 4.0              2.6.18      i686        SUCCESS
Debian 3.1              2.6.8       i386        SUCCESS
Debian 3.1              2.4.27      i386        FAILED : Makefile not compatible for 2.4 linux kernels
```
### Required Packages for Debian
```
$ sudo apt-get install make linux-headers-$(uname -r)
```
### Build Kernel Module
```
$ make debug
KCFLAGS="-DMIDUMP_DEBUG" make CONFIG_DEBUG_SG=y -C /lib/modules/5.13.0-39-generic/build M="/home/engin/workspace/memory-image-dump" modules
make[1]: Entering directory '/usr/src/linux-headers-5.13.0-39-generic'
  CC [M]  /home/engin/workspace/memory-image-dump/disk.o
  CC [M]  /home/engin/workspace/memory-image-dump/main.o
  LD [M]  /home/engin/workspace/memory-image-dump/midump.o
  MODPOST /home/engin/workspace/memory-image-dump/Module.symvers
  CC [M]  /home/engin/workspace/memory-image-dump/midump.mod.o
  LD [M]  /home/engin/workspace/memory-image-dump/midump.ko
  BTF [M] /home/engin/workspace/memory-image-dump/midump.ko
Skipping BTF generation for /home/engin/workspace/memory-image-dump/midump.ko due to unavailability of vmlinux
make[1]: Leaving directory '/usr/src/linux-headers-5.13.0-39-generic'
strip --strip-unneeded midump.ko
sudo /usr/src/linux-headers-5.13.0-39-generic/scripts/sign-file sha256 /var/lib/shim-signed/mok/MOK.priv /var/lib/shim-signed/mok/MOK.der midump.ko
mv -f midump.ko midump-5.13.0-39-generic.ko
```
### Load Kernel Module
```
$ sudo insmod midump-5.13.0-39-generic.ko path=/memory_dump.img
[137699.659933] [MIDump] Parameter : PATH : /memory_dump.img
[137699.659936] [MIDump] Initializing Dump...
[137700.438603] [MIDump] Direct IO Disabled
[137700.438621] [MIDump] Writing range 1000 - 9dfff.
[137700.439006] [MIDump] Writing range 9f000 - 9ffff.
[137700.439010] [MIDump] Writing range 100000 - 2ed64017.
[137700.931059] [MIDump] Padding partial page : vaddr 00000000e5183119 size 24
[137700.931067] [MIDump] Writing range 2ed64018 - 2ed84057.
[137700.931174] [MIDump] Padding partial page : vaddr 0000000026fed75a size 64
[137700.931176] [MIDump] Writing range 2ed84058 - 2edd4fff.
[137700.931451] [MIDump] Padding partial page : vaddr 0000000066094351 size 4008
[137700.931454] [MIDump] Writing range 2edd6000 - 2f26dfff.
[137700.934811] [MIDump] Writing range 2f362000 - 31e33fff.
[137700.963680] [MIDump] Writing range 31e35000 - 35ddcfff.
[137701.009687] [MIDump] Writing range 39c4e000 - 39c4efff.
[137701.009695] [MIDump] Writing range 100000000 - 4be7fffff.
[137728.822704] [MIDump] Memory Dump Completed
```
