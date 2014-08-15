#ifndef BDSDebug_h
#define BDSDebug_h

#include <cstddef>
#include <string>
#include <cstdio>
#include <cstdlib>

inline std::string methodName(const std::string& prettyFunction)
{
  size_t colons = prettyFunction.find("::");
  size_t begin = prettyFunction.substr(0,colons).rfind(" ") + 1;
  size_t len = prettyFunction.rfind("(") - begin;
  return prettyFunction.substr(begin,len) +"> ";
}
#define __METHOD_NAME__ methodName(__PRETTY_FUNCTION__)


inline std::string methodEnd(const std::string& prettyFunction)
{
  return methodName(prettyFunction) + "- end of method.";
}
#define __METHOD_END__ methodEnd(__PRETTY_FUNCTION__)



#endif
