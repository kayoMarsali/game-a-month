#pragma once

//Signed Integer Types
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;

//Unsigned Integer Types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
//Floating Point Types
typedef float f32;
typedef double f64;

//Boolean Types
typedef char b8;
typedef int b32;

#define FALSE 0
#define TRUE 1

#ifdef _MSC_VER
#define KINLINE __forceinline
#define KNOINLINE __declspec(noinline)
#else
#define KINLINE static inline
#define KNOINLINE 
#endif