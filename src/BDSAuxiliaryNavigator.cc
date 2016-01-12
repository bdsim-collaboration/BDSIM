#include "BDSDebug.hh"
#include "BDSAuxiliaryNavigator.hh"

#include "G4AffineTransform.hh"
#include "G4Navigator.hh"

class G4VPhysicalVolume;

G4Navigator* BDSAuxiliaryNavigator::auxNavigator = new G4Navigator();

BDSAuxiliaryNavigator::BDSAuxiliaryNavigator():
  initialised(new G4bool),
  globalToLocal(new G4AffineTransform()),
  localToGlobal(new G4AffineTransform())
{
  (*initialised) = false;
}

BDSAuxiliaryNavigator::~BDSAuxiliaryNavigator()
{
  delete initialised;
  delete globalToLocal;
  delete localToGlobal;
  // do not delete static auxNavigator as required by other instances of BDSAuxiliaryNavigator
}

void BDSAuxiliaryNavigator::AttachWorldVolumeToNavigator(G4VPhysicalVolume* worldPV)
{
  auxNavigator->SetWorldVolume(worldPV);
}

void BDSAuxiliaryNavigator::InitialiseTransform(const G4ThreeVector& globalPosition) const
{
  auxNavigator->LocateGlobalPointAndSetup(globalPosition);  
  (*globalToLocal) = auxNavigator->GetGlobalToLocalTransform();
  (*localToGlobal) = auxNavigator->GetLocalToGlobalTransform();
  (*initialised)   = true;
}
