#ifndef _KALLOC_H_
#define _KALLOC_H_

#include <stddef.h> /* for size_t */

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        size_t capacity, available, n_blocks, n_cores, largest;
    } pbkm_stat_t;

    void* pbkmalloc(void* km, size_t size);
    void* pbkrealloc(void* km, void* ptr, size_t size);
    void* pbkcalloc(void* km, size_t count, size_t size);
    void pbkfree(void* km, void* ptr);

    void* pbkm_init(void);
    void pbkm_destroy(void* km);
    void pbkm_stat(const void* _km, pbkm_stat_t* s);

#ifdef __cplusplus
}
#endif

#endif
