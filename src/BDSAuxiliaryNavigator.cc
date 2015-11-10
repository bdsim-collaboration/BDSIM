#include "BDSDebug.hh"
#include "BDSAuxiliaryNavigator.hh"

#include "G4Navigator.hh"

class G4VPhysicalVolume;

G4Navigator* BDSAuxiliaryNavigator::auxNavigator = new G4Navigator();

BDSAuxiliaryNavigator::BDSAuxiliaryNavigator()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

BDSAuxiliaryNavigator::~BDSAuxiliaryNavigator()
{
  // do not delete auxNavigator as required by other instances of BDSAuxiliaryNavigator
}

void BDSAuxiliaryNavigator::AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPV)
{
  auxNavigator->SetWorldVolume(worldPV);
}
