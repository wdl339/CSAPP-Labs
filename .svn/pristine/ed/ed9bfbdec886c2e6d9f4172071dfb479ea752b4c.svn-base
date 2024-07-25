/*
 * mm.c - A fast, least memory-efficient malloc package based on segregated fits.
 * 
 * 522031910516 韦东良
 * 
 * The structure of block:
 * A block has a header and footer, which is 4 bytes and packs the block size and allocation status.
 * For the free block, it has two pointers to the previous and next free block, which is after the header and 4 bytes each.
 * The machine is 64-bit but the pointer is only 4 bytes, because I use the offset to the heap_list to represent the pointer.
 *
 * The organization of the free list:
 * The heap_list is a pointer to the start of the heap, which is also the start of the array of free lists.
 * I use the first 20 * 4 bytes to store the head of the free list of each size class.
 * 
 * How my allocator manipulates the free list:
 * When a block is freed, it will be inserted into the free list of the corresponding class.
 * The class i is determined by the size of the block, which is 2^(i+4) to 2^(i+5) bytes.
 * The block is inserted at the beginning of the free list because of the LIFO policy.
 * The best fit policy is used to find the free block to allocate.
 * 
 * The structure of the heap:
 * The array of free lists comes first, then comes the prologue block, common blocks and epilogue block.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define MINBLOCKSIZE 16
#define CLASSNUM 20

// different extend size for different situation
#define INITCHUNKSIZE (1 << 8)
#define MCHUNKSIZE (1 << 12)
#define RECHUNKSIZE (1 << 4)

#define MAX(x, y) ((x) > (y) ? (x) : (y))

// pack a size and allocated bit into a word
#define PACK(size, alloc) ((size) | (alloc))

// read and write a word at address p
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

// read the size and allocated fields from address p
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

// given block ptr bp, compute address of its header and footer
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

// given block ptr bp, compute address of next and previous blocks
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

// get the offset of the block from the heap_list
#define GET_OFFSET(bp) ((char *)bp - (char *)heap_list)
// get the address of the block from the offset
#define GET_ADDR(offset) (heap_list + offset)

// get the head address of the free list of the class num
#define GET_HEAD_POS(num) (heap_list + (num * WSIZE))
// get the head block of the free list of the class num
#define GET_HEAD(num) (GET(GET_HEAD_POS(num)) ? GET_ADDR(GET(GET_HEAD_POS(num))) : NULL)

// get the prev and next free block of the free block
#define GET_PREV(bp) (GET(bp) ? (GET_ADDR(GET(bp))) : NULL)
#define GET_NEXT(bp) (GET((char *)(bp) + WSIZE) ? (GET_ADDR(GET((char *)(bp) + WSIZE))) : NULL)
// set the prev and next free block of the free block
#define PUT_PREV(bp, val) PUT(bp, val)
#define PUT_NEXT(bp, val) PUT((char *)(bp) + WSIZE, val)

// the start of the heap
void *heap_list;

// flag to determine the strategy to place the block according to the size
int block_size_flag = 1;
// the definiton of the "big block"
int big_block_size = (1 << 8);

// check if it's the first time to call realloc or malloc
int first_re = 1;
int first_m = 1;

void *extend_heap(size_t words);
void *coalesce(void *bp);
void *find_fit(size_t asize);
void *best_fit(size_t asize);
void *place(void *bp, size_t asize);
int search_list(size_t asize);
void insert_block(void *bp);
void delete_block(void *bp);
int mm_check(void);

/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    // create the initial empty heap
    if ((heap_list = mem_sbrk((CLASSNUM + 4) * WSIZE)) == (void *)-1)
	    return -1;

    for (int i = 0; i < CLASSNUM; i++) {
        PUT(heap_list + (i * WSIZE), 0);
    }

    heap_list += (CLASSNUM * WSIZE);

    PUT(heap_list, 0);
    PUT(heap_list + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_list + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_list + (3 * WSIZE), PACK(0, 1));

    heap_list -= (CLASSNUM * WSIZE);

    first_re = 1;
    first_m = 1;

    // extend the empty heap with a free block of INITCHUNKSIZE bytes
    if (extend_heap(INITCHUNKSIZE / WSIZE) == NULL)
	    return -1;
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    // just a trick, if the first malloc is a small size, 
    // then the definition of "big block" is reduced to 64
    if (first_m){
        if (size <= (1 << 4)) {
            big_block_size =  (1 << 6);
        }
        first_m = 0;
    }

    // printf("malloc size: %d\n", size);
    // if (mm_check() == 0)
    //     exit(1);
    // if (size == 0)
    //     return NULL;

    char *bp;
    size_t asize = ALIGN(size + DSIZE);

    // search the free list to find a fit
    if ((bp = best_fit(asize)) != NULL) 
        return place(bp, asize);

    // no fit found, extend the heap
    size_t extend_size = MAX(asize, MCHUNKSIZE);
    if ((bp = extend_heap(extend_size / WSIZE)) == NULL)
        return NULL;
        
    return place(bp, asize);
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
    // if (mm_check() == 0)
    //     exit(1);
    // if (ptr == NULL)
    //     return;

    size_t size = GET_SIZE(HDRP(ptr));
    // printf("free size: %d\n", size);

    // set the block as free
    PUT(HDRP(ptr), PACK(size, 0));
    PUT(FTRP(ptr), PACK(size, 0));

    coalesce(ptr);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    // if (mm_check() == 0)
    //     exit(1);

    // just a trick, if the first realloc is a big size,
    // then the strategy to place the block is changed
    if (first_re && size > (1 << 10)){
        block_size_flag = 0;
        big_block_size = (1 << 8);
        first_re = 0;
    } else if (first_re){
        big_block_size = (1 << 8);
        first_re = 0;
    }
    
    // if ptr is NULL, then mm_malloc(size)
    if (ptr == NULL)
        return mm_malloc(size);
    
    // if size is 0, then mm_free(ptr)
    if (size == 0) {
        mm_free(ptr);
        return NULL;
    }

    size_t oldsize = GET_SIZE(HDRP(ptr));
    size_t asize = ALIGN(size + DSIZE);
    size_t old_sub_new_size = oldsize - asize;

    // if the size is the same, just return
    if (oldsize == asize)
        return ptr;

    // if the size change smaller
    else if (oldsize > asize) {

        // if the size is big enough, split the block
        if (old_sub_new_size >= MINBLOCKSIZE) {

            PUT(HDRP(ptr), PACK(asize, 1));
            PUT(FTRP(ptr), PACK(asize, 1));
            void *next_bp = NEXT_BLKP(ptr);
            PUT(HDRP(next_bp), PACK(old_sub_new_size, 0));
            PUT(FTRP(next_bp), PACK(old_sub_new_size, 0));
            coalesce(next_bp);

        } 

        return ptr;

    // if the size change bigger
    } else {

        void *next_bp = NEXT_BLKP(ptr);
        size_t next_size = GET_ALLOC(HDRP(next_bp)) ? 0 : GET_SIZE(HDRP(next_bp));

        int can_extend = !GET_SIZE(HDRP(next_bp)) || (next_size && !GET_SIZE(HDRP(NEXT_BLKP(next_bp))));

        // if there is no enough space but can extend the heap, then extend the heap
        if (can_extend && asize > oldsize + next_size) {
            size_t extend_size = MAX(asize - oldsize - next_size, RECHUNKSIZE);
            extend_heap(extend_size / WSIZE);
            next_size = GET_SIZE(HDRP(next_bp));
        }

        size_t all_size = oldsize + next_size;
        size_t can_use_size = old_sub_new_size + next_size;

        // if there is enough space now, split the block
        if (all_size >= asize) {
            delete_block(NEXT_BLKP(ptr));

            // if the rest size is big enough, insert the rest block into the free list after split
            if (can_use_size >= MINBLOCKSIZE) {

                PUT(HDRP(ptr), PACK(asize, 1));
                PUT(FTRP(ptr), PACK(asize, 1));
                next_bp = NEXT_BLKP(ptr);
                PUT(HDRP(next_bp), PACK(can_use_size, 0));
                PUT(FTRP(next_bp), PACK(can_use_size, 0));
                insert_block(next_bp);

            // if the size is not big enough, just use the whole space
            }  else {
                    
                PUT(HDRP(ptr), PACK(all_size, 1));
                PUT(FTRP(ptr), PACK(all_size, 1));

            }

            return ptr;
        }

        // if there is no enough space, then malloc a new block
        void *newptr = mm_malloc(size);
        if (newptr == NULL)
            return NULL;
        memcpy(newptr, ptr, oldsize - DSIZE);
        mm_free(ptr);
        return newptr;

    }
}

/*
 * extend_heap - Extend the heap if there is no fit block.
 */
void *extend_heap(size_t words) 
{
    // printf("extend_heap_size %d\n", words);
    char *bp;
    size_t size;

    // allocate an even number of words to maintain alignment
    size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
    if ((long)(bp = mem_sbrk(size)) == -1)
	    return NULL;
    
    // initialize free block header/footer and the epilogue header
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

    // coalesce if the previous block was free
    return coalesce(bp);
}

/*
 * coalesce - Coalesce the contiguous free blocks and insert the block into the free list.
 */
void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    // printf("prev_alloc: %d, next_alloc: %d\n", prev_alloc, next_alloc);
    size_t size = GET_SIZE(HDRP(bp));
    // printf("size: %d\n", size);

    // case 1: no coalesce
    if (prev_alloc && next_alloc) {

        insert_block(bp);
	    return bp;

    // case 2: coalesce with the next block
    } else if (prev_alloc && !next_alloc) {

        void *next_bp = NEXT_BLKP(bp);
        delete_block(next_bp);
	    size += GET_SIZE(HDRP(next_bp));
	    PUT(HDRP(bp), PACK(size, 0));
	    PUT(FTRP(bp), PACK(size, 0));

    // case 3: coalesce with the previous block
    } else if (!prev_alloc && next_alloc) {

        void *prev_bp = PREV_BLKP(bp);
        delete_block(prev_bp);
	    size += GET_SIZE(HDRP(prev_bp));
	    PUT(FTRP(bp), PACK(size, 0));
	    PUT(HDRP(prev_bp), PACK(size, 0));
	    bp = prev_bp;

    // case 4: coalesce with the previous and next block
    } else {

        void *prev_bp = PREV_BLKP(bp);
        void *next_bp = NEXT_BLKP(bp);
        delete_block(prev_bp);
        delete_block(next_bp);
	    size += (GET_SIZE(HDRP(prev_bp)) + GET_SIZE(HDRP(next_bp)));
	    PUT(HDRP(prev_bp), PACK(size, 0));
	    PUT(FTRP(next_bp), PACK(size, 0));
	    bp = prev_bp;

    }
    
    insert_block(bp);
    return bp;
}

/*
 * find_fit - Find a fit block in the free list.
 */
void *find_fit(size_t asize)
{
    void *bp;
    int idx = search_list(asize);

    // search
    while(idx < CLASSNUM) {
        bp = GET_HEAD(idx);
        while (bp != NULL) {
            if (GET_SIZE(HDRP(bp)) >= asize) {
                // when the first fit is found, return
                return bp;
            }
            bp = GET_NEXT(bp);
        }
        idx++;
    }

    return NULL;
}

/*
 * best_fit - Find the best fit block in the free list.
 */
void *best_fit(size_t asize)
{
    void *bp;
    void *best_bp = NULL;
    size_t min_size = 0;

    int idx = search_list(asize);

    while(idx < CLASSNUM) {
        bp = GET_HEAD(idx);
        while (bp != NULL) {
            // find a better fit
            if (GET_SIZE(HDRP(bp)) >= asize && (min_size == 0 || GET_SIZE(HDRP(bp)) < min_size)) {
                min_size = GET_SIZE(HDRP(bp));
                best_bp = bp;
            }
            bp = GET_NEXT(bp);
        }
        idx++;
    }

    return best_bp;
}

/*
 * place - Place the given size to the given free block.
 */
void *place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    // printf("place_size: %d\n", csize);
    delete_block(bp);
    size_t sub_size = csize - asize;

    if (sub_size >= MINBLOCKSIZE) {

        int flag;
        // determine the strategy to place the block
        if (block_size_flag)
            flag = asize >= big_block_size;
        else
            flag = asize < big_block_size;

        if(flag) {

            // place the block in the left side
            PUT(HDRP(bp), PACK(asize, 1));
            PUT(FTRP(bp), PACK(asize, 1));
            void* next_bp = NEXT_BLKP(bp);
            PUT(HDRP(next_bp), PACK(sub_size, 0));
            PUT(FTRP(next_bp), PACK(sub_size, 0));
            insert_block(next_bp);
            return bp;

        } else {
            
            // place the block in the right side
            PUT(HDRP(bp), PACK(sub_size, 0));
            PUT(FTRP(bp), PACK(sub_size, 0));
            void* next_bp = NEXT_BLKP(bp);
            PUT(HDRP(next_bp), PACK(asize, 1));
            PUT(FTRP(next_bp), PACK(asize, 1));
            insert_block(bp);
            return next_bp;

        }

    // if the rest size is not big enough, no need to split
    } else {

        PUT(HDRP(bp), PACK(csize, 1));
        PUT(FTRP(bp), PACK(csize, 1));
        return bp;

    }
}

/*
 * search_list - Search the free list to find the class number of the block.
 */
int search_list(size_t asize)
{
    // printf("search_list_size: %d\n", asize);
    int i = 0;
    size_t size = MINBLOCKSIZE;

    // The class i is determined by the size of the block, which is 2^(i+4) to 2^(i+5) bytes
    while (i < CLASSNUM - 1 && size < asize) {
        size <<= 1;
        i++;
    }

    return i;
}

/*
 * insert_block - Insert the block into the free list.
 */
void insert_block(void *bp)
{
    if (bp == NULL)
        return;

    size_t size = GET_SIZE(HDRP(bp));
    int idx = search_list(size);
    // printf("insert_block: %p, size: %d, idx: %d\n", bp, size, idx);
    void *head = GET_HEAD_POS(idx);
    void *next = GET_HEAD(idx);

    // if the free list is empty
    if (next == NULL){

        PUT_PREV(bp, 0);
        PUT_NEXT(bp, 0);
        PUT(head, GET_OFFSET(bp));

    } else {

        PUT_PREV(bp, 0);
        PUT_NEXT(bp, GET(head));
        PUT_PREV(next, GET_OFFSET(bp));
        PUT(head, GET_OFFSET(bp));

    }

}

/*
 * delete_block - Delete the block from the free list.
 */
void delete_block(void *bp)
{
    // printf("delete_block: %p\n", bp);
    if (bp == NULL)
        return;

    void *prev = GET_PREV(bp);
    void *next = GET_NEXT(bp);

    // case 1: no prev and next (only one)
    if (prev == NULL && next == NULL) {

        size_t size = GET_SIZE(HDRP(bp));
        int idx = search_list(size);

        void *head = GET_HEAD_POS(idx);
        PUT(head, 0);

    // case 2: no prev (the first)
    } else if (prev == NULL) {

        PUT_PREV(next, 0);

        size_t size = GET_SIZE(HDRP(bp));
        int idx = search_list(size);

        void *head = GET_HEAD_POS(idx);
        PUT(head, GET_OFFSET(next));
    
    // case 3: no next (the last)
    } else if (next == NULL) {

        PUT_NEXT(prev, 0);

    // case 4: both prev and next
    } else {

        PUT_NEXT(prev, GET_OFFSET(next));
        PUT_PREV(next, GET_OFFSET(prev));

    }
}

/*
 * mm_check - Check the consistency of the heap.
 */
int mm_check(void)
{
    char *bp;
    for (bp = heap_list + CLASSNUM * WSIZE + DSIZE; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)) {
        // 2 check there is not contiguous free blocks
        if (!GET_ALLOC(HDRP(bp)) && !GET_ALLOC(HDRP(NEXT_BLKP(bp)))) {
            printf("Error: Contiguous free blocks %p and %p escaped coalescing!!!\n", bp, NEXT_BLKP(bp));
            return 0;
        }

        // 3 check if all free blocks are in the free list
        if (!GET_ALLOC(HDRP(bp))) {
            int in_list = 0;
            size_t size = GET_SIZE(HDRP(bp));
            int idx = search_list(size);
            void *head = GET_HEAD(idx);
            if (head != NULL) {
                void *p = head;
                while (p) {
                    if (p == bp) {
                        in_list = 1;
                        break;
                    }
                    p = GET_NEXT(p);
                }
            }
            if (!in_list) {
                printf("Error: Free block %p is not in the free list!!!\n", bp);
                return 0;
            }
        }

        // 5 check if the header and footer are consistent
        if (GET_SIZE(HDRP(bp)) != GET_SIZE(FTRP(bp))) {
            printf("Error: Block %p header and footer size inconsistent!!!\n", bp);
            return 0;
        }

        if (GET_ALLOC(HDRP(bp)) != GET_ALLOC(FTRP(bp))) {
            printf("Error: Block %p header and footer alloc inconsistent!!!\n", bp);
            return 0;
        }
    }

    // 1 check if all free blocks in the free list are marked as free
    for (int i = 0; i < CLASSNUM; i++) {
        void *bp = GET_HEAD(i);
        while (bp) {
            if (GET_ALLOC(HDRP(bp))) {
                printf("Error: Block %p in the free list is not free!!!\n", bp);
                return 0;
            }
            bp = GET_NEXT(bp);
        }
    }

    // 4 check if the pointers in the free list point to valid free blocks
    for (int i = 0; i < CLASSNUM; i++) {
        void *bp = GET_HEAD(i);
        while (bp) {
            if (GET_PREV(bp) != NULL && (GET_PREV(bp) < heap_list + CLASSNUM * WSIZE || GET_PREV(bp) > mem_heap_hi())) {
                printf("Error: Block %p in the free list has invalid prev pointer!!!\n", bp);
                return 0;
            }
            if (GET_NEXT(bp) != NULL && (GET_NEXT(bp) < heap_list + CLASSNUM * WSIZE || GET_NEXT(bp) > mem_heap_hi())) {
                printf("Error: Block %p in the free list has invalid next pointer!!!\n", bp);
                return 0;
            }
            bp = GET_NEXT(bp);
        }
    }

    return 1;
}





