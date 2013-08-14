#pragma once

#include <string>
#include <stdio.h>
#include <stdarg.h>

std::string strprintf(const char *format, ...)
{
  const int buffer_size = 1024;
  char buffer[buffer_size];
  va_list arg;
  int done;

  va_start(arg, format);
  done = vsprintf(buffer, format, arg);
  va_end(arg);

  return std::string(buffer);
}
