// Copyright (c) 2011-2014 Joe Sylve <joe.sylve@gmail.com> (504ENSICS Labs) LiME Linux Memory Extractor
// Copyright (c) 2022 Engin Kuzu <enginkuzu@gmail.com>
#include <linux/ioport.h>
#include <linux/highmem.h>
#include <linux/module.h>
#include <linux/version.h>
#include <asm/uaccess.h>

#ifdef MIDUMP_DEBUG
#define DBG(fmt, args...) do { printk("[MIDump] "fmt"\n", ## args); } while (0)
#else
#define DBG(fmt, args...) do {} while(0)
#endif

#define RETRY_IF_INTERRUPTED(f) ({ \
    ssize_t err; \
    do { err = f; } while(err == -EAGAIN || err == -EINTR); \
    err; \
})

