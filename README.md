# Memory Image Dump (MIDump)
Memory image dump Linux kernel module for B!nalyze Coding Challenge. 

### Test Results
```
Distro                  Kernel      Arch        Test Result
---------------------------------------------------------------------------------------
Ubuntu 20.04.4 LTS      5.13.0      x86_64      FAILED : unsigned module error
Debian 11.2             5.10.0      x86_64      SUCCESS
Debian 10.12            4.19.0      x86_64      SUCCESS
Debian 9.3              4.9.0       x86_64      SUCCESS
Debian 8.11             3.16.0      x86_64      FAILED : (BUG) scheduling while atomic
Debian 7.11             3.2.0       x86_64      FAILED : (BUG) scheduling while atomic
Debian 6.0.10           2.6.32      x86_64      SUCCESS
Debian 5.0.10           2.6.26      x86_64      SUCCESS
Debian 4.0              2.6.18      i686        FAILED : Compile error
Debian 3.1              2.4.27      i686        FAILED : Compile error
```
### Required Packages for Debian
```
# apt-get install make linux-headers-$(uname -r)
```
### Build Kernel Module
```
# make debug
KCFLAGS="-DMIDUMP_DEBUG" make CONFIG_DEBUG_SG=y -C /lib/modules/5.10.0-10-amd64/build M="/root/memory-image-dump" modules
make[1]: Entering directory '/usr/src/linux-headers-5.10.0-10-amd64'
  CC [M]  /root/memory-image-dump/disk.o
  CC [M]  /root/memory-image-dump/main.o
  LD [M]  /root/memory-image-dump/midump.o
  MODPOST /root/memory-image-dump/Module.symvers
  CC [M]  /root/memory-image-dump/midump.mod.o
  LD [M]  /root/memory-image-dump/midump.ko
make[1]: Leaving directory '/usr/src/linux-headers-5.10.0-10-amd64'
strip --strip-unneeded midump.ko
mv midump.ko midump-5.10.0-10-amd64.ko
```
### Load Kernel Module
```
# insmod midump-5.10.0-10-amd64.ko path=/memory_dump.img
[   55.758005] midump: loading out-of-tree module taints kernel.
[   55.758034] midump: module verification failed: signature and/or required key missing - tainting kernel
[   55.758148] [MIDump] Parameter : PATH : /memory_dump.img
[   55.758148] [MIDump] Initializing Dump...
[   55.758205] [MIDump] Direct IO Disabled
[   55.758215] [MIDump] Writing range 1000 - 9fbff.
[   55.759104] [MIDump] Padding partial page : vaddr 000000000de051c7 size 3072
[   55.759109] [MIDump] Writing range 100000 - 1ffeffff.
[   56.478367] [MIDump] Memory Dump Completed
```
