// Copyright (c) 2011-2014 Joe Sylve <joe.sylve@gmail.com> (504ENSICS Labs) LiME Linux Memory Extractor
// Copyright (c) 2022 Engin Kuzu <enginkuzu@gmail.com>
#include "common.h"
#include "disk.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,14)
#define pfn_to_page(pfn) (mem_map+(pfn))
#endif

extern struct resource iomem_resource;

char *path = "mem_dump.img";
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,52)
module_param(path, charp, S_IRUGO);
#endif

static void *vpage4padding;

static int init(void);
static void write_range(struct resource *);
static ssize_t write_padding(size_t s);
static ssize_t write_vaddr(void *, size_t);

// Called when 'insmod'
int init_module(void){

    DBG("Initializing Dump...");

    if( !path ){
        DBG("No path parameter specified!");
        return -EINVAL;
    }
    DBG("Parameter : PATH : %s", path);

    return init();
}

static int init(){

    int err;
    struct resource *res;

    if( (err = setup_disk(path)) ){
        DBG("Setup Error!");
        cleanup_disk();
        return err;
    }

    vpage4padding = (void *)__get_free_page(GFP_NOIO);

    // Dump only "System RAM" blocks
    // All blocks can be listed with "sudo cat /proc/iomem" command
    for(res=iomem_resource.child;res;res=res->sibling){
        if( !res->name || strcmp(res->name, "System RAM") ) continue;
        write_range(res);
    }

    DBG("Memory Dump Completed");
    cleanup_disk();
    free_page((unsigned long)vpage4padding);

    return 0;
}

static void write_range(struct resource *res){

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
    resource_size_t i, is;
#else
    __PTRDIFF_TYPE__ i, is;
#endif

    ssize_t s;
    void *vaddr;
    struct page *page;

    DBG("Writing range %llx - %llx", res->start, res->end);

    for(i=res->start;i<=res->end;i+=is){

        // Get page pointer from virtual address
        page = pfn_to_page((i)>>PAGE_SHIFT);
        is = min((size_t)PAGE_SIZE, (size_t)(res->end - i + 1));

        if( is < PAGE_SIZE ){
            // We can't map partial pages and the linux kernel doesn't use them anyway
            DBG("Padding partial page : vaddr %p size %lu", (void *)i, (unsigned long)is);
            write_padding(is);
        }else{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
            // Map a page to virtual address (for temporary usage)
            vaddr = kmap_atomic(page);
#else
            // Map a page to virtual address (for long term usage)
            vaddr = kmap(page);
#endif
            s = write_vaddr(vaddr, is);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,9,0)
            kunmap_atomic(vaddr);
#else
            kunmap(page);
#endif
            if( s < 0 ){
                DBG("Failed to write page : vaddr %p. Skipping range", vaddr);
                break;
            }
        }
    }
}

static ssize_t write_padding(size_t s){

    ssize_t r;
    size_t i = 0;

    // Clear page
    memset(vpage4padding, 0, PAGE_SIZE);

    while( s -= i ){
        i = min((size_t)PAGE_SIZE, s);
        r = write_vaddr(vpage4padding, i);
        if( r != i ){
            DBG("Error sending zero page : %zd", r);
            return r;
        }
    }

    return 0;
}

static ssize_t write_vaddr(void *vaddr, size_t is){

    ssize_t ret;

    if( is <= 0 ) return is;

    ret = RETRY_IF_INTERRUPTED(
        write_vaddr_disk(vaddr, is)
    );

    if( ret < 0 ){
        DBG("Write error: %zd", ret);
    }else if( ret != is ){
        DBG("Short write %zu instead of %zu.", ret, is);
        ret = -1;
    }

    return ret;
}

// Called when 'rmmod'
void cleanup_module(void){
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Engin Kuzu <enginkuzu@gmail.com>");
MODULE_DESCRIPTION("Memory image dump Linux kernel module for B!nalyze Coding Challenge");

