#include "mm.h"

static char *heap_listp;

static void *extend_heap(size_t words);
static void *coalesce(void* bp);
void *find_fit(size_t size);
void place(void *bp,size_t size);

int mm_init(void)
{
    if((heap_listp = mem_brk(4 * WSIZE)) == (void *)-1) return -1;

    PUT(heap_listp,0);
    PUT(heap_listp + (1 * WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (2 * WSIZE),PACK(DSIZE,1));
    PUT(heap_listp + (3 * WSIZE),PACK(0,1));
    heap_listp += DSIZE;

    if(extend_heap(CHUNKSIZE/WSIZE) == NULL) return -1;

    return 0;
}

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    size = (words % 2)? (words + 1) * WSIZE : words * WSIZE;

    if((long)(bp == mem_brk(size)) == -1) return NULL;

    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));

    return coalesce(bp);
}

void mm_free(void* bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    coalesce(bp);
}

static void *coalesce(void* bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if(prev_alloc && next_alloc)
    {
        return bp;
    }
    else if(prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(bp),PACK(size,0));
    }
    else if(!prev_alloc && next_alloc)
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else
    {
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }

    return bp;
}

void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extendsize;
    char *bp;

    if(size == 0) return NULL;

    if(size <= DSIZE)
    {
        asize = 2 * DSIZE;
    }
    else
    {
        asize = DSIZE * ((size + DSIZE + DSIZE - 1) / (DSIZE));
    }

    if((bp = find_fit(asize)) != NULL)
    {
        place(bp,asize);
        return bp;
    }
    extendsize = MAX(asize,CHUNKSIZE);
    if((bp = extend_heap(extendsize/WSIZE)) == NULL) return NULL;

    place(bp,asize);
    return bp;
}

void *find_fit(size_t size)
{
    char *bp = NEXT_BLKP(heap_listp);
    size_t tail = PACK(0,1);
    size_t alloc;
    size_t blksize;


    while(*HDRP(bp) != tail)
    {
        blksize = GET_SIZE(HDRP(bp));
        alloc = GET_ALLOC(HDRP(bp));
        if(blksize >= size && alloc == 0)
        {
            return bp;
        }
        bp = NEXT_BLKP(bp);
    }
    return NULL;
}

void place(void *bp,size_t size)
{
    size_t asize;

    asize = GET_SIZE(HDRP(bp)) - size;

    if(!asize)
    {
        PUT(HDRP(bp),PACK(size,1));
        PUT(FTRP(bp),PACK(size,1));
    }
    else
    {
        PUT(HDRP(bp),PACK(size,1));
        PUT(FTRP(bp),PACK(size,1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp),PACK(asize,0));
        PUT(FTRP(bp),PACK(asize,0));
    }
}
