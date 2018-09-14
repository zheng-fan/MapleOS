#ifndef __LIBS_DEFS_H__
#define __LIBS_DEFS_H__

#ifndef NULL
#define NULL ((void *)0)
#endif

#define __always_inline inline __attribute__((always_inline))
#define __noinline __attribute__((noinline))
#define __noreturn __attribute__((noreturn))

typedef int bool;

// 定义整数类型
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

// 定义地址长度变量
typedef int32_t intptr_t;
typedef uint32_t uintptr_t;
typedef uint64_t longptr_t;
typedef uint64_t ulongptr_t;

typedef uintptr_t size_t;

// 定义ERROR
/* kernel error codes -- keep in sync with list in lib/printfmt.c */
#define E_UNSPECIFIED 1  // Unspecified or unknown problem
#define E_BAD_PROC 2     // Process doesn't exist or otherwise
#define E_INVAL 3        // Invalid parameter
#define E_NO_MEM 4       // Request failed due to memory shortage
#define E_NO_FREE_PROC 5 // Attempt to create a new process beyond
#define E_FAULT 6        // Memory fault

/* the maximum allowed */
#define MAXERROR 6

// 操作系统链表的写法
/* Return the offset of 'member' relative to the beginning of a struct type */
#define offsetof(type, member) \
    ((size_t)(&((type *)0)->member))

/* *
 * to_struct - get the struct from a ptr
 * @ptr:    a struct pointer of member
 * @type:   the type of the struct this is embedded in
 * @member: the name of the member within the struct
 * */
#define to_struct(ptr, type, member) \
    ((type *)((char *)(ptr)-offsetof(type, member)))

#endif /* !__LIBS_DEFS_H__ */
