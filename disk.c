// Copyright (c) 2011-2014 Joe Sylve <joe.sylve@gmail.com> (504ENSICS Labs) LiME Linux Memory Extractor
// Copyright (c) 2022 Engin Kuzu <enginkuzu@gmail.com>
#include "common.h"

static struct file *fp = NULL;

ssize_t write_vaddr_disk(void *v, size_t is);

// Test function for Direct I/O access
static char dio_write_test(char *path, int oflags){

    char dio_success = 0;

    // Open file and return file pointer
    fp = filp_open(path, oflags|O_DIRECT|O_SYNC, 0444);
    if( fp && !IS_ERR(fp) ){
        dio_success = write_vaddr_disk("DIO", 3) == 3;
        filp_close(fp, NULL);
    }

    return dio_success;
}

int setup_disk(char *path){

    int err = 0;
    int oflags = O_WRONLY|O_CREAT|O_LARGEFILE|O_TRUNC;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    // Modify the current process address limits
    mm_segment_t fs = get_fs();
    set_fs(KERNEL_DS);
#endif

    // Test for direct I/O access
    if( dio_write_test(path, oflags) ){
        oflags |= O_DIRECT|O_SYNC;
        DBG("Direct IO Enabled");
    }else{
        DBG("Direct IO Disabled");
    }

    // Open file and return file pointer
    fp = filp_open(path, oflags, 0444);
    if( !fp || IS_ERR(fp) ){
        DBG("Error opening file %ld", PTR_ERR(fp));
        err = (fp) ? PTR_ERR(fp) : -EIO;
        fp = NULL;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    set_fs(fs);
#endif

    return err;
}

ssize_t write_vaddr_disk(void *v, size_t is){

    loff_t pos = fp->f_pos;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    mm_segment_t fs = get_fs();
    set_fs(KERNEL_DS);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,17)
    ssize_t s = fp->f_op->write(fp, v, is, &pos);
#else
    ssize_t s = vfs_write(fp, v, is, &pos);
#endif
    set_fs(fs);
#else
    ssize_t s = kernel_write(fp, v, is, &pos);
#endif

    if( s == is ){
        fp->f_pos = pos;
    }

    return s;
}

void cleanup_disk(void){

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    mm_segment_t fs = get_fs();
    set_fs(KERNEL_DS);
#endif

    if(fp) filp_close(fp, NULL);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,0)
    set_fs(fs);
#endif
}

