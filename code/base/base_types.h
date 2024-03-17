#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef int32_t b32;

typedef float f32;
typedef double f64;

typedef size_t usize;

typedef struct {
    f32 x0;
    f32 y0;
    f32 x1;
    f32 y1;
} RectF32;

typedef struct {
    u8 *data;
    usize size;
} String8;

#endif // TYPES_H
