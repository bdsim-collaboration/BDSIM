#include "TRint.h"
#include "TRootApplication.h"

#include "Config.hh"
#include "EventDisplay.hh"

extern EventDisplay *gEventDisplay = nullptr; 

int main(int argc, char *argv[])
{
  int rintArgc;
  char** rintArgv;
  TRint  *a = new TRint("App", &rintArgc, rintArgv);

  Config *c = Config::Instance(argv[1]);
  EventDisplay *ed = new EventDisplay(argv[2]);
  gEventDisplay = ed;
  a->Run(kTRUE);
  
}
