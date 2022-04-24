// Copyright (c) 2011-2014 Joe Sylve <joe.sylve@gmail.com> (504ENSICS Labs) LiME Linux Memory Extractor
// Copyright (c) 2022 Engin Kuzu <enginkuzu@gmail.com>
#include "common.h"
#include "disk.h"

static void write_range(struct resource *);
static int init(void);
static ssize_t write_vaddr(void *, size_t);
static ssize_t try_write(void *, ssize_t);
static int setup(void);
static void cleanup(void);

static void *vpage;

char *path = 0;

extern struct resource iomem_resource;

module_param(path, charp, S_IRUGO);

int init_module(void){

    if( !path ){
        DBG("No path parameter specified!");
        return -EINVAL;
    }

    DBG("Parameter : PATH : %s", path);

    return init();
}

static int init(){

    struct resource *p;
    int err = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
    resource_size_t p_last = -1;
#else
    __PTRDIFF_TYPE__ p_last = -1;
#endif

    DBG("Initializing Dump...");
    if( (err = setup()) ){
        DBG("Setup Error!");
        cleanup();
        return err;
    }

    vpage = (void *)__get_free_page(GFP_NOIO);

    for(p=iomem_resource.child; p; p=p->sibling){
        if( !p->name || strcmp(p->name, MIDUMP_RAMSTR) ) continue;
        write_range(p);
        p_last = p->end;
    }

    DBG("Memory Dump Completed");
    cleanup();
    free_page((unsigned long) vpage);

    return 0;
}

static ssize_t write_padding(size_t s){

    size_t i = 0;
    ssize_t r;

    memset(vpage, 0, PAGE_SIZE);

    while( s-=i ){

        i = min((size_t) PAGE_SIZE, s);
        r = write_vaddr(vpage, i);

        if( r != i ){
            DBG("Error sending zero page : %zd", r);
            return r;
        }
    }

    return 0;
}

static void write_range(struct resource *res){

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
    resource_size_t i, is;
#else
    __PTRDIFF_TYPE__ i, is;
#endif

    struct page *p;
    void *v;
    ssize_t s;

    DBG("Writing range %llx - %llx.", res->start, res->end);

    for(i=res->start; i<=res->end; i+=is){

        p = pfn_to_page((i)>>PAGE_SHIFT);
        is = min((size_t)PAGE_SIZE, (size_t)(res->end - i + 1));

        if( is < PAGE_SIZE ){
            // We can't map partial pages and the linux kernel doesn't use them anyway
            DBG("Padding partial page : vaddr %p size %lu", (void *)i, (unsigned long)is);
            write_padding(is);
        }else{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
            v = kmap_atomic(p);
#else
            v = kmap(p);
#endif
            s = write_vaddr(v, is);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
            kunmap_atomic(v);
#else
            kunmap(p);
#endif
            if( s < 0 ){
                DBG("Failed to write page : vaddr %p. Skipping range", v);
                break;
            }
        }
    }
}

static ssize_t write_vaddr(void *v, size_t is){
    ssize_t ret;
    ret = try_write(v, is);
    return ret;
}

static ssize_t try_write(void *v, ssize_t is){

    ssize_t ret;

    if( is <= 0 ) return is;

    ret = RETRY_IF_INTERRUPTED(
        write_vaddr_disk(v, is)
    );

    if( ret < 0 ){
        DBG("Write error: %zd", ret);
    }else if( ret != is ){
        DBG("Short write %zu instead of %zu.", ret, is);
        ret = -1;
    }

    return ret;
}

static int setup(void){
    return setup_disk(path);
}

static void cleanup(void){
    return cleanup_disk();
}

void cleanup_module(void){
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Engin Kuzu <enginkuzu@gmail.com>");
MODULE_DESCRIPTION("Memory image dump Linux kernel module for B!nalyze Coding Challenge");

