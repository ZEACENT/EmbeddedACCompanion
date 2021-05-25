#ifndef __COMMON_H__
#define __COMMON_H__

#define DEBUG           0
#define BAN_ONE_NET     0
#define BAN_ALI_CLD     0
#define PRINT_X_Y       0
#define LOCAL_CTRL      1

// OneNET device
#define DEVICE_ID  "632339932"
#define AUTH_KEY   "Vu8o99rROrIWyWbV5gJ9qx90E0A="

/*---------------------------------------------------------------------------*/
/* Type Definition Macros                                                    */
/*---------------------------------------------------------------------------*/
#ifndef __WORDSIZE
  /* Assume 32 */
  #define __WORDSIZE 32
#endif

#if defined(_LINUX) || defined (WIN32)
    typedef unsigned char   uint8;
    typedef char            int8;
    typedef unsigned short  uint16;
    typedef short           int16;
    typedef unsigned int    uint32;
    typedef int             int32;
#endif

#ifdef WIN32
    typedef int socklen_t;
#endif

#if defined(WIN32)
    typedef unsigned long long int  uint64;
    typedef long long int           int64;
#elif (__WORDSIZE == 32)
    __extension__
    typedef long long int           int64;
    __extension__
    typedef unsigned long long int  uint64;
#elif (__WORDSIZE == 64)
    typedef unsigned long int       uint64;
    typedef long int                int64;
#endif

#endif /* __COMMON_H__ */
