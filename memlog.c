#include <stdarg.h>
#include <stdio.h>

int mlog(const char *fmt, ...)
{
  static unsigned int id = 1;
  va_list ap;
  int res;

  res = fprintf(stderr, "[%04u] ", id++);

  va_start(ap, fmt);
  res += vfprintf(stderr, fmt, ap);
  va_end(ap);

  fprintf(stderr, "\n");

  return res;
}
