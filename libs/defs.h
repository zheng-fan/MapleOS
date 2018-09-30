#ifndef __LIBS_DEFS_H__
#define __LIBS_DEFS_H__

#ifndef NULL
#define NULL ((void *)0)
#endif

#define __always_inline inline __attribute__((always_inline))
#define __noinline __attribute__((noinline))
#define __noreturn __attribute__((noreturn))
#define __packed __attribute__((packed))

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
typedef int64_t longptr_t;
typedef uint64_t ulongptr_t;

typedef uintptr_t size_t;

#endif /* !__LIBS_DEFS_H__ */
