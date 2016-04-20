#include "TRint.h"
#include "TRootApplication.h"
#include "EventDisplay.hh"

int main(int argc, char *argv[])
{
  TRint  *a = new TRint("App", &argc, argv);
  EventDisplay *ed = new EventDisplay("io.gdml","output_event-2.root");
  a->Run(kTRUE);
}