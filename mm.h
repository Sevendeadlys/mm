#include "csapp.h"

#define WSIZE      4        /*Word and head/footer size*/
#define DSIZE      8        /*Double word size*/
#define CHUNKSIZE  (1<<12)  /*Extend heap by this amount*/

#define MAX(x,y) ((x) > (y)? (x) : (y))

#define PACK(size,alloc) ((size) | (alloc))

#define GET(p)      (*(unsigned int *)(p))
#define PUT(p,val)  (*(unsigned int *)(p) = (val))

#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

extern int mm_init(void);
extern void* mm_malloc(size_t size);
extern void mm_free(void* ptr);
