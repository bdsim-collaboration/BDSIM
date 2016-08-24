#include "BDSDebug.hh"
#include "BDSAuxiliaryNavigator.hh"

#include "G4Navigator.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4StepStatus.hh"
#include "G4ThreeVector.hh"

G4Navigator* BDSAuxiliaryNavigator::auxNavigator      = new G4Navigator();
G4Navigator* BDSAuxiliaryNavigator::auxNavigatorCL    = new G4Navigator();
G4int        BDSAuxiliaryNavigator::numberOfInstances = 0;

BDSAuxiliaryNavigator::BDSAuxiliaryNavigator():
  BDSAuxiliaryNavigator(true)
{;}

BDSAuxiliaryNavigator::BDSAuxiliaryNavigator(G4bool cacheTransformsIn):
  initialised(false),
  globalToLocal(new G4AffineTransform()),
  localToGlobal(new G4AffineTransform()),
  globalToLocalCL(new G4AffineTransform()),
  localToGlobalCL(new G4AffineTransform()),
  cacheTransforms(cacheTransformsIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  numberOfInstances++;
}

BDSAuxiliaryNavigator::~BDSAuxiliaryNavigator()
{
  delete globalToLocal;
  delete localToGlobal;
  delete globalToLocalCL;
  delete localToGlobalCL;
  // Only delete navigator objects
  if (numberOfInstances == 1)
    {
      delete auxNavigator;   auxNavigator   = nullptr;
      delete auxNavigatorCL; auxNavigatorCL = nullptr;
    }
  numberOfInstances--;
}

G4VPhysicalVolume* BDSAuxiliaryNavigator::LocateGlobalPointAndSetup(const G4ThreeVector& point,
								    const G4ThreeVector* direction,
								    const G4bool pRelativeSearch,
								    const G4bool ignoreDirection,
								    G4bool useCurvilinear)
{
  G4Navigator* nav = Navigator(useCurvilinear);
  return nav->LocateGlobalPointAndSetup(point, direction,
					pRelativeSearch, ignoreDirection);
}

G4VPhysicalVolume* BDSAuxiliaryNavigator::LocateGlobalPointAndSetup(G4Step const* const step,
								    G4bool useCurvilinear)
{ // const pointer to const G4Step
  // 'pos' = post but has same length as pre so code looks better
  G4StepPoint* preStepPoint = step->GetPreStepPoint();
  G4StepPoint* posStepPoint = step->GetPostStepPoint();
  G4StepStatus preStatus    = preStepPoint->GetStepStatus();
  G4StepStatus posStatus    = posStepPoint->GetStepStatus();

  G4ThreeVector position;
  if (preStatus == G4StepStatus::fGeomBoundary && posStatus == G4StepStatus::fGeomBoundary)
    {// both are on a boundary - use average - assume chord is inside the solid if points are
      position = (posStepPoint->GetPosition() - preStepPoint->GetPosition())/2.0;
    }
  else if (posStatus == G4StepStatus::fGeomBoundary)
    {// pos on boundary - use pre
      position = preStepPoint->GetPosition();
    }
  else
    {// pre on boundary - use post
      position = posStepPoint->GetPosition();
    }
  // order this way as it's more common the post point is on the boundary and belongs to
  // next volume
  
  G4Navigator* nav = Navigator(useCurvilinear);  // select navigator
  G4VPhysicalVolume* selectedVol = nav->LocateGlobalPointAndSetup(position);
  return selectedVol;
}

void BDSAuxiliaryNavigator::InitialiseTransform(const G4ThreeVector& globalPosition) const
{
  auxNavigator->LocateGlobalPointAndSetup(globalPosition);
  auxNavigatorCL->LocateGlobalPointAndSetup(globalPosition);  
  (*globalToLocal) = auxNavigator->GetGlobalToLocalTransform();
  (*localToGlobal) = auxNavigator->GetLocalToGlobalTransform();
  (*globalToLocalCL) = auxNavigatorCL->GetGlobalToLocalTransform();
  (*localToGlobalCL) = auxNavigatorCL->GetLocalToGlobalTransform();
  if (cacheTransforms)
    {initialised = true;} // else always remains false
}

void BDSAuxiliaryNavigator::InitialiseTransform(const G4ThreeVector &globalPosition,
                                                const G4ThreeVector &globalMomentum, const G4double stepLength)
{
    G4ThreeVector endPoint = globalPosition + globalMomentum.unit()*stepLength;
    G4ThreeVector midPoint = (endPoint - globalPosition) / 2;
    InitialiseTransform(midPoint);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToLocal(const G4double globalPosition[3],
						    const G4bool&  useCurvilinear) const
{
  G4ThreeVector globalPositionV(globalPosition[0], globalPosition[1], globalPosition[2]);
  return ConvertToLocal(globalPositionV, useCurvilinear);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToLocal(const G4ThreeVector& globalPosition,
						    const G4bool&        useCurvilinear) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return GlobalToLocal(useCurvilinear)->TransformPoint(globalPosition);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToLocal(const G4double globalPosition[3],
							const G4double globalAxis[3],
							const G4bool&  useCurvilinear) const
{
  G4ThreeVector globalPositionV(globalPosition[0], globalPosition[1], globalPosition[2]);
  G4ThreeVector globalAxisV(globalAxis[0], globalAxis[1], globalAxis[2]);
  return ConvertAxisToLocal(globalPositionV, globalAxisV, useCurvilinear);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToLocal(const G4ThreeVector& globalPosition,
							const G4ThreeVector& globalAxis,
							const G4bool&        useCurvilinear) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return GlobalToLocal(useCurvilinear)->TransformAxis(globalAxis);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToGlobal(const G4ThreeVector& localAxis,
							 const G4bool&        useCurvilinear) const
{return LocalToGlobal(useCurvilinear)->TransformAxis(localAxis);}

std::pair<G4ThreeVector, G4ThreeVector> BDSAuxiliaryNavigator::ConvertAxisToGlobal(const std::pair<G4ThreeVector, G4ThreeVector>& localAxis,
										   const G4bool& useCurvilinear) const
{
  G4AffineTransform* lToG = LocalToGlobal(useCurvilinear);
  G4ThreeVector globalB = lToG->TransformAxis(localAxis.first);
  G4ThreeVector globalE = lToG->TransformAxis(localAxis.second);
  return std::make_pair(globalB, globalE);
}										 

G4ThreeVector BDSAuxiliaryNavigator::ConvertToGlobal(const G4ThreeVector& localPosition,
						     const G4bool&        useCurvilinear) const
{return LocalToGlobal(useCurvilinear)->TransformPoint(localPosition);}

G4ThreeVector BDSAuxiliaryNavigator::ConvertAxisToGlobal(const G4ThreeVector& globalPosition,
							 const G4ThreeVector& localAxis,
							 const G4bool&        useCurvilinear) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return LocalToGlobal(useCurvilinear)->TransformAxis(localAxis);
}

G4ThreeVector BDSAuxiliaryNavigator::ConvertToGlobal(const G4ThreeVector& globalPosition,
						     const G4ThreeVector& localPosition,
						     const G4bool&        useCurvilinear) const
{
  if (!initialised)
    {InitialiseTransform(globalPosition);}
  return LocalToGlobal(useCurvilinear)->TransformPoint(localPosition);
}

G4Navigator* BDSAuxiliaryNavigator::Navigator(G4bool curvilinear) const
{
  // condition ? case true : case false
  return curvilinear ? auxNavigatorCL : auxNavigator;
}

G4AffineTransform* BDSAuxiliaryNavigator::GlobalToLocal(G4bool curvilinear) const
{
  return curvilinear ? globalToLocalCL : globalToLocal;
}

G4AffineTransform* BDSAuxiliaryNavigator::LocalToGlobal(G4bool curvilinear) const
{
  return curvilinear ? localToGlobalCL : localToGlobal;
}
