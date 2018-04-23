#ifndef _UTILS_H
#define _UTILS_H

#define len(array) (sizeof(array) / sizeof(*array))

#define LC_INIT(name) static int __##name = 0
#define LC_ALLOC(name) do { printf("[%s] ALLOC %d\n", #name, ++__##name); } while(0)
#define LC_FREE(name) do { printf("[%s] FREE %d\n", #name, --__##name); } while(0)

#endif
