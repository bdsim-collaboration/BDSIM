#include "TRint.h"
#include "TRootApplication.h"

#include "Config.hh"
#include "EventDisplay.hh"

int main(int /*argc*/, char *argv[])
{
  int rintArgc;
  char** rintArgv = nullptr;
  TRint  *a = new TRint("App", &rintArgc, rintArgv);

  Config::Instance(argv[1]);
  /*EventDisplay *ed = */ new EventDisplay(argv[2]);
  a->Run(kTRUE);
  
}
