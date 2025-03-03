#ifndef __ROMCODE_H__
#define __ROMCODE_H__


// ROM function prototypes
// typedef void (*puts_t)(const char *str);


  #if defined(PCB100466824)
    #include "PCB/PCB100466824.h"

  #elif defined(PCB100434407)
    #include "PCB/PCB100434407.h"

  #else
    #error "Specify PCB number"

  #endif

  #define puts(X)     ((void(*)(const char * ))ROM_PUTS)(X)
  #define uartInit()  ((void(*)())ROM_UARTINIT)()
  #define putchar(X)  ((void(*)(const char ))ROM_PUTCHAR)(X)
  #define getchare()  ((char(*)())ROM_GETCHARE)()



#endif
