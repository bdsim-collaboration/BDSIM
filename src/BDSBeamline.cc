#include "globals.hh" // geant4 globals / types
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include "BDSDebug.hh"
#include "BDSAcceleratorComponent.hh"
#include "BDSBeamline.hh"
#include "BDSBeamlineElement.hh"

#include <iterator>
#include <ostream>
#include <utility>  // for std::pair
#include <vector>

BDSBeamline::BDSBeamline()
{;}

BDSBeamline::~BDSBeamline()
{
  std::vector<BDSBeamlineElement*>::iterator it = begin();
  for (; it != end(); ++it)
    {delete (*it);}
}

void BDSBeamline::PrintAllComponents(std::ostream& out) const
{
  std::vector<BDSBeamlineElement*>::const_iterator it = cbegin();
  for (; it != cend(); ++it)
    {out << *(it);}
}

std::ostream& operator<< (std::ostream& out, BDSBeamline const &beamline)
{
  out << "BDSBeamline with " << beamline.size() << " elements"<< G4endl
      << "Elements are: " << G4endl;
  beamline.PrintAllComponents(out);

  return out;
}

void BDSBeamline::AddComponent(BDSAcceleratorComponent* component)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " adding component" << G4endl;
#endif
  // test if component has finite length (different action in that case)

  // calculate the placement position
  G4ThreeVector positionStart  = G4ThreeVector(0,0,0);
  G4ThreeVector positionMiddle = G4ThreeVector(0,0,0);
  G4ThreeVector positionEnd    = G4ThreeVector(0,0,0);

  // calculate the placement rotation
  G4RotationMatrix* rotationStart  = new G4RotationMatrix();
  G4RotationMatrix* rotationMiddle = new G4RotationMatrix();
  G4RotationMatrix* rotationEnd    = new G4RotationMatrix();

  // calculate the s position
  G4double sPositionStart  = 0;
  G4double sPositionMiddle = 0;
  G4double sPositionEnd    = 0;

  // construct beamline element
  BDSBeamlineElement* element = new BDSBeamlineElement(component,
						       positionStart,
						       positionMiddle,
						       positionEnd,
						       rotationStart,
						       rotationMiddle,
						       rotationEnd,
						       sPositionStart,
						       sPositionMiddle,
						       sPositionEnd);
  
  // append it to the beam line
  beamline.push_back(element);
}
