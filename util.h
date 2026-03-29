#ifndef UTIL_LUKAS_H
#define UTIL_LUKAS_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L
    #define NO_RETURN [[noreturn]]
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
    #define NO_RETURN _Noreturn
#else
    #ifdef __GNUC__
        #define NO_RETURN __attribute__((noreturn))
    #else
        #define NO_RETURN
    #endif
#endif

/*=== common ===*/

NO_RETURN
static inline void TODO(char* err)
{
    fprintf(stdout, "%s\n", err);
    fflush(stdout);
    abort();
}

NO_RETURN
static inline void giveUp(char* err)
{
    fprintf(stderr, "%s\n", err);
    fflush(stderr);
    abort();
}

/*=== Int - definition ===*/

// small int stuff
#define KILOBYTE 1024
#define KB_SIZE(x) ((x) * 1024)

#include <stdint.h>

// Signed integers
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

// Unsigned integers
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// Floating point
typedef float f32;
typedef double f64;

/*=== Arena - Allocator ===*/

typedef struct {
    size_t reserved;
    size_t used;
} arena;

#define ARENA_BASE_SIZE sizeof(arena)

static inline arena* arena_alloc(size_t size)
{
    arena* ret = (arena*)malloc(size + ARENA_BASE_SIZE);
    ret->reserved = size + ARENA_BASE_SIZE;
    ret->used = 0 + ARENA_BASE_SIZE;
    return ret;
}

static inline void* arena_push(arena* arenaIn, size_t size)
{
    if (arenaIn->used + size <= arenaIn->reserved) {
        void* ret = (char*)arenaIn + arenaIn->used;
        arenaIn->used += size;
        return ret;
    }
    return NULL;
}

static inline void* arena_pop(arena* arenaIn, size_t size)
{
    if (arenaIn->used - size >= ARENA_BASE_SIZE) {
        arenaIn->used -= size;
        void* ret = (char*)arenaIn + arenaIn->used;
        return ret;
    }
    return NULL;
}

static inline void arena_free(arena* arenaIn)
{
    if (arenaIn != NULL) {
        free(arenaIn);
    }
}

/*=== dynamic - list ===*/

#define DYNAMIC_LIST_PROTOTYPE(t)                                           \
    typedef struct dyn_##t dyn_##t;                                         \
    static inline dyn_##t dyn_##t##_create(size_t maxLen);                  \
    static inline bool dyn_##t##_push(dyn_##t* listIn, t element);          \
    static inline t dyn_##t##_get(dyn_##t* listIn, size_t idx);             \
    static inline bool dyn_##t##_set(dyn_##t* listIn, size_t idx, t value); \
    static inline size_t dyn_##t##_len(dyn_##t* listIn);                    \
    static inline bool dyn_##t##_remove(dyn_##t* listIn, size_t idx);

#define dyn_create(t, maxLen) dyn_##t##_create(maxLen)
#define dyn_push(t, listIn, element) dyn_##t##_push(listIn, element)
#define dyn_get(t, listIn, idx) dyn_##t##_get(listIn, idx)
#define dyn_set(t, listIn, idx, value) dyn_##t##_set(listIn, idx, value)
#define dyn_len(t, listIn) dyn_##t##_len(listIn)
#define dyn_remove(t, idx) dyn_##t##_remove(listIn, idx)

#define DYNAMIC_LIST_IMPL(t)                                                                \
    struct dyn_##t {                                                                        \
        size_t cLen;                                                                        \
        size_t maxLen;                                                                      \
        t* content;                                                                         \
    };                                                                                      \
                                                                                            \
    static inline dyn_##t dyn_##t##_create(size_t maxLen)                                   \
    {                                                                                       \
        dyn_##t temp;                                                                       \
        temp.content = (t*)calloc(maxLen, sizeof(t));                                       \
        temp.maxLen = maxLen;                                                               \
        temp.cLen = 0;                                                                      \
        return temp;                                                                        \
    }                                                                                       \
                                                                                            \
    static inline bool dyn_##t##_push(dyn_##t* listIn, t element)                           \
    {                                                                                       \
        if (listIn->maxLen == 0 || listIn->content == NULL)                                 \
            return false;                                                                   \
                                                                                            \
        if (listIn->cLen < listIn->maxLen) {                                                \
            listIn->content[listIn->cLen] = element;                                        \
            listIn->cLen++;                                                                 \
            return true;                                                                    \
        }                                                                                   \
                                                                                            \
        while (listIn->cLen >= listIn->maxLen) {                                            \
            listIn->content = (t*)realloc(listIn->content, 2 * listIn->maxLen * sizeof(t)); \
            if (listIn->content != NULL) {                                                  \
                listIn->maxLen *= 2;                                                        \
            } else {                                                                        \
                return false;                                                               \
            }                                                                               \
        }                                                                                   \
                                                                                            \
        return dyn_##t##_push(listIn, element);                                             \
    }                                                                                       \
                                                                                            \
    static inline t dyn_##t##_get(dyn_##t* listIn, size_t idx)                              \
    {                                                                                       \
        if (idx > listIn->maxLen - 1)                                                       \
            giveUp("list out of bound");                                                    \
        if (listIn->content == NULL)                                                        \
            giveUp("list content empty");                                                   \
        return listIn->content[idx];                                                        \
    }                                                                                       \
                                                                                            \
    static inline bool dyn_##t##_set(dyn_##t* listIn, size_t idx, t value)                  \
    {                                                                                       \
        if (idx > listIn->maxLen - 1 || listIn->content == NULL)                            \
            return false;                                                                   \
                                                                                            \
        listIn->content[idx] = value;                                                       \
        return true;                                                                        \
    }                                                                                       \
                                                                                            \
    static inline size_t dyn_##t##_len(dyn_##t* listIn)                                     \
    {                                                                                       \
        return listIn->cLen;                                                                \
    }                                                                                       \
                                                                                            \
    static inline bool dyn_##t##_remove(dyn_##t* listIn, size_t idx)                        \
    {                                                                                       \
        if (listIn->maxLen == 0                                                             \
            || listIn->cLen == 0                                                            \
            || idx >= listIn->cLen                                                          \
            || idx >= listIn->maxLen                                                        \
            || listIn->content == NULL)                                                     \
            return false;                                                                   \
                                                                                            \
        for (size_t i = idx; i < listIn->cLen - 1; i++) {                                   \
            t temp = dyn_##t##_get(listIn, i + 1);                                          \
            dyn_##t##_set(listIn, i, temp);                                                 \
        }                                                                                   \
        listIn->cLen -= 1;                                                                  \
        return true;                                                                        \
    }

#endif

/*
    DYNAMIC_LIST_PROTOTYPE(long)
    DYNAMIC_LIST_IMPL(long)

    arena* a = arena_alloc(20);
    printf("%p\n", (void*)a);
    printf("%p\n", (void*)((char*)a + 16));
    printf("%p\n", arena_push(a, 1));
    printf("%p\n", arena_push(a, 1));
    printf("%p\n", arena_push(a, 1));
    printf("%lu\n", a->reserved);
    printf("%lu\n", a->used);
    printf("%p\n", arena_pop(a, 1));
    printf("%p\n", arena_pop(a, 1));
    printf("%p\n", arena_pop(a, 1));

    dyn_long b = dyn_create(long, 5);
    for (int i = 0; i < 10; i++) {
        // dyn_long_push(&b, i);
        dyn_push(long, &b, i);
        printf("%lu\n", dyn_len(long, &b));
    }
    for (int i = 0; i < 10; i++) {
        printf("%ld\n", dyn_get(long, &b, i));
    }
 */
