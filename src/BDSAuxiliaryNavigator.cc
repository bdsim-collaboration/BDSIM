#include "BDSDebug.hh"
#include "BDSAuxiliaryNavigator.hh"

#include "G4AffineTransform.hh"
#include "G4Navigator.hh"

#include <utility>

G4Navigator* BDSAuxiliaryNavigator::auxNavigator = new G4Navigator();

BDSAuxiliaryNavigator::BDSAuxiliaryNavigator():
  initialised(false),
  globalToLocal(new G4AffineTransform()),
  localToGlobal(new G4AffineTransform())
{
}

BDSAuxiliaryNavigator::~BDSAuxiliaryNavigator()
{
  delete globalToLocal;
  delete localToGlobal;
  // do not delete static *auxNavigator as required by other instances of BDSAuxiliaryNavigator
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
  initialised      = true;
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToLocal(const G4double globalPosition[3]) const
{
  G4ThreeVector globalPositionV(globalPosition[0], globalPosition[1], globalPosition[2]);
  return ConvertToLocal(globalPositionV);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToLocal(const G4ThreeVector& globalPosition) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return globalToLocal->TransformPoint(globalPosition);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToLocal(const G4double globalPosition[3],
							const G4double globalAxis[3]) const
{
  G4ThreeVector globalPositionV(globalPosition[0], globalPosition[1], globalPosition[2]);
  G4ThreeVector globalAxisV(globalAxis[0], globalAxis[1], globalAxis[2]);
  return ConvertAxisToLocal(globalPositionV, globalAxisV);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToLocal(const G4ThreeVector& globalPosition,
							const G4ThreeVector& globalAxis) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return globalToLocal->TransformAxis(globalAxis);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToGlobal(const G4ThreeVector& localAxis) const
{return localToGlobal->TransformAxis(localAxis);}

std::pair<G4ThreeVector, G4ThreeVector> BDSAuxiliaryNavigator::ConvertAxisToGlobal(const std::pair<G4ThreeVector, G4ThreeVector>& localAxis) const
{
  G4ThreeVector globalB = localToGlobal->TransformAxis(localAxis.first);
  G4ThreeVector globalE = localToGlobal->TransformAxis(localAxis.second);
  return std::make_pair(globalB, globalE);
}
										 

G4ThreeVector BDSAuxiliaryNavigator::ConvertToGlobal(const G4ThreeVector& localPosition) const
{return localToGlobal->TransformPoint(localPosition);}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToGlobal(const G4ThreeVector& globalPosition,
							 const G4ThreeVector& localAxis) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return localToGlobal->TransformAxis(localAxis);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToGlobal(const G4ThreeVector& globalPosition,
						     const G4ThreeVector& localPosition) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return localToGlobal->TransformPoint(localPosition);
}
