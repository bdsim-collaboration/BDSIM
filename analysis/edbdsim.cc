#include "TRint.h"
#include "TRootApplication.h"

#include "Config.hh"
#include "EventDisplay.hh"

int main(int /*argc*/, char *argv[])
{
  int rintArgc;
  char** rintArgv = nullptr;
  TRint  *a = new TRint("App", &rintArgc, rintArgv);

  Config::Instance("analysisConfig.txt");
  EventDisplay::Instance(argv[2], argv[1]);
  a->Run(kTRUE);
  
}
