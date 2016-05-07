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
