#include "BDSBeamline.hh"
#include "BDSBeamlineSet.hh"


void BDSBeamlineSet::DeleteContents()
{
  delete massWorld;
  delete curvilinearWorld;
  delete curvilinearBridgeWorld;
  delete endPieces;
}
