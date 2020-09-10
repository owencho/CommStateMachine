#ifndef SM_COMMON_H
#define SM_COMMON_H

#ifndef TEST
    #define STATIC static
#else
    #define STATIC
#endif

#define freeMem(x)                                      \
            do{if(x) free(x);}while(0)

#endif // SM_COMMON_H
