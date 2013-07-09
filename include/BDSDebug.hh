#ifndef BDSDebug_h
#define BDSDebug_h

#include <stddef.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>

inline std::string methodName(const std::string& prettyFunction)
{
  size_t colons = prettyFunction.find("::");
  size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
  size_t end = prettyFunction.rfind("(") - begin;
  return prettyFunction.substr(begin,end) +"> ";
}
#define __METHOD_NAME__ methodName(__PRETTY_FUNCTION__)


#endif
