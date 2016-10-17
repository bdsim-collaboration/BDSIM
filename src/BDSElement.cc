#include "BDSAcceleratorComponent.hh"
#include "BDSDebug.hh"
#include "BDSExtent.hh"
#include "BDSExecOptions.hh"
#include "BDSElement.hh"
#include "BDSFieldBuilder.hh"
#include "BDSFieldFactory.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGlobalConstants.hh"
#include "BDSFieldFactory.hh"

#include "globals.hh" // geant4 globals / types
#include "G4Box.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"

#include <vector>

class BDSFieldInfo;

BDSElement::BDSElement(G4String      name,
		       G4double      length,
		       G4double      outerDiameterIn,
		       G4String      geometry,
		       G4String      bmap):
  BDSAcceleratorComponent(name, length, 0, "element"),
  outerDiameter(outerDiameterIn),
  geometryFileName(geometry),
  bMapFileName(bmap)
{;}

void BDSElement::BuildContainerLogicalVolume()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ <<G4endl;
#endif

  BDSGeometryExternal* geom = BDSGeometryFactory::Instance()->BuildGeometry(geometryFileName);
  
  if (!geom)
    {
      G4cerr << __METHOD_NAME__ << "Error loading geometry in component \""
	     << name << "\"" << G4endl;
      exit(1);
    }
  
  // We don't registe the geometry as a daughter as the geometry factory retains
  // ownership of the geometry and will clean it up at the end.
  
  // make the beam pipe container, this object's container
  containerLogicalVolume = geom->GetContainerLogicalVolume();
  containerSolid         = geom->GetContainerSolid();
  
  // update extents
  InheritExtents(geom);

  const BDSExtent geomExtent = geom->GetExtent();
  BDSExtent nominalExt = BDSExtent(outerDiameter*0.5, outerDiameter*0.5, chordLength*0.5);
  if (nominalExt.TransverselyGreaterThan(geomExtent))
    {SetExtent(nominalExt);}

  G4double extLength = GetExtent().DZ();
  if (extLength > chordLength)
    {
      G4cerr << "BDSElement> The loaded geometry is larger than the specified length"
	     << " of the element, which will cause overlaps!" << G4endl
	     << "Calculated extent along z: " << extLength << " mm, vs specified "
	     << chordLength << G4endl;
      exit(1);
    }

  // Get the field definition from the parser
  auto fieldInfo = BDSFieldFactory::Instance()->GetDefinition(bMapFileName);

  // In case there was no field, the info might but nullptr - check
  if (fieldInfo)
    {// valid field specification - register for construction.
      BDSFieldBuilder::Instance()->RegisterFieldForConstruction(fieldInfo,
								containerLogicalVolume,
								true);
    }
}

BDSElement::~BDSElement()
{;}
