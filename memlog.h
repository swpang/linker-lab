#ifndef __MEMLOG_H__
#define __MEMLOG_H__

#include <stdarg.h>
#include <stdio.h>

//
// log a call to one of the dynamic memory management functions to stderr
//
//   res        result pointer (if any)
//
//   nmemb,
//   size,      correspond to the parameters of the respective function call
//   ptr
//
// returns the number of characters printed
//

#define LOG_MALLOC(size, res)         mlog("%9c malloc( %zu ) = %p", ' ', size, res)
#define LOG_CALLOC(nmemb, size, res)  mlog("%9c calloc( %zu , %zu ) = %p", ' ', nmemb, size, res)
#define LOG_REALLOC(ptr, size, res)   mlog("%9c realloc( %p , %zu ) = %p", ' ', ptr, size, res)
#define LOG_FREE(ptr)                 mlog("%9c free( %p )", ' ', ptr)


//
// log statistics
//
#define LOG_STATISTICS(alloc_total, alloc_avg, free_total) \
  { mlog(""); \
    mlog("Statistics"); \
    mlog("  allocated_total      %lu", alloc_total); \
    mlog("  allocated_avg        %lu", alloc_avg); \
    mlog("  freed_total          %lu", free_total); \
  }

//
// log statistics about memory blocks
//
#define LOG_NONFREED_START() \
  { mlog(""); \
    mlog("Non-deallocated memory blocks"); \
    mlog("  %-16s   %-8s   %-7s", "block", "size", "ref cnt"); \
  }
#define LOG_BLOCK(ptr, size, cnt) mlog("  %-16p   %-8zd   %-7d", ptr, size, cnt)

//
// log invalid deallocation requests
//
#define LOG_DOUBLE_FREE()             mlog("%2c  *** DOUBLE_FREE  *** (ignoring)", ' ')
#define LOG_ILL_FREE()                mlog("%2c  *** ILLEGAL_FREE *** (ignoring)", ' ')

//
// log start/end messages
//
#define LOG_START()                   mlog("Memory tracer started.")
#define LOG_STOP() \
  { mlog(""); \
    mlog("Memory tracer stopped."); \
  }

//
// do not use this directly. Invoke through one of the macros above
//
int mlog(const char *fmt, ...);

#endif
