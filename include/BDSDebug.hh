#ifndef BDSDebug_h
#define BDSDebug_h

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <string>

inline std::string methodName(const std::string& prettyFunction)
{
  size_t end    = prettyFunction.rfind("(");
  size_t begin  = prettyFunction.rfind(" ",end) + 1;
  // if function has pointer type then begin now is *, if so advance one more
  if (prettyFunction[begin] == '*') {begin++;}
  size_t len = end - begin;
  return prettyFunction.substr(begin,len) +"> ";
}
#define __METHOD_NAME__ methodName(__PRETTY_FUNCTION__)


inline std::string methodEnd(const std::string& prettyFunction)
{
  return methodName(prettyFunction) + "- end of method.";
}
#define __METHOD_END__ methodEnd(__PRETTY_FUNCTION__)



#endif
