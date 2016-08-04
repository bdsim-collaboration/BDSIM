#include "Analysis.hh"


Analysis::Analysis()
{
}

Analysis::~Analysis()
{
  delete chain;
}

void Analysis::Execute()
{
  Process();
  SimpleHistograms();
  Terminate();
}
