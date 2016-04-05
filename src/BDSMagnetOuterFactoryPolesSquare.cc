#include "BDSMagnetOuterFactoryPolesSquare.hh"

#include "BDSBeamPipe.hh"
#include "BDSDebug.hh"
#include "BDSMagnetOuter.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMaterials.hh"

#include "globals.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4EllipticalTube.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <vector>
#include <cmath>

BDSMagnetOuterFactoryPolesSquare* BDSMagnetOuterFactoryPolesSquare::_instance = nullptr;

BDSMagnetOuterFactoryPolesSquare* BDSMagnetOuterFactoryPolesSquare::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSMagnetOuterFactoryPolesSquare();}
  return _instance;
}

BDSMagnetOuterFactoryPolesSquare::BDSMagnetOuterFactoryPolesSquare()
{
  CleanUp();
}

BDSMagnetOuterFactoryPolesSquare::~BDSMagnetOuterFactoryPolesSquare()
{
  _instance = nullptr;
}

void BDSMagnetOuterFactoryPolesSquare::CleanUp()
{
  BDSMagnetOuterFactoryPolesBase::CleanUp();
  poleRotations.clear();
  poleSolids.clear();
  poleLVs.clear();
  order = 0;
}

void BDSMagnetOuterFactoryPolesSquare::CreatePoleSolid(G4String     name,
						       G4double     length,
						       G4int        orderIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // record order to this class - this is the first method that uses it
  order = orderIn;
  
  G4int nPoles = 2*order;
  
  // full circle is divided into segments for each pole
  G4double segmentAngleWidth = CLHEP::twopi / (G4double)nPoles;
  G4double poleAngle = segmentAngleWidth * poleAngularFraction;

  // compose pole of an annulus of a certain angle plus
  // an ellipsoid at the inner side. make the union of the
  // two to give a typical pole shape - curved inner surface
  // with increasing width outwards
  
  // make some % of pole length curved ellipsoidal surface

  G4double  croppingBoxRadius = yokeStartRadius - lengthSafety;
  G4VSolid* croppingBoxSolid = new G4Box(name + "_pole_intersection_solid", // name
					 croppingBoxRadius,                 // x half width
					 croppingBoxRadius,                 // y half width
					 length);                           // z length
  allSolids.push_back(croppingBoxSolid);
  // z length long for unambiguous intersection
  
  G4double poleLength          = poleFinishRadius - poleStartRadius - 2*lengthSafety;
  G4double ellipsoidHeight     = poleTipFraction*poleLength;
  //G4double annulusHeight       = poleLength - ellipsoidHeight;
  G4double ellipsoidCentre     = poleStartRadius + ellipsoidHeight*0.5;
  
  // work out the chord length
  G4double ellipsoidWidth      = 2*(poleStartRadius + 0.5*ellipsoidHeight) * tan(poleAngle*0.5);
  
  G4double annulusStartRadius  = ellipsoidCentre / cos(poleAngle*0.5); //so edges line up with ellipsoid edges
  G4double annulusFinishRadius = poleFinishRadius*2; // long for unambiguous intersection with yoke

  G4VSolid* poleTip = new G4EllipticalTube(name + "_pole_tip_solid",   // name
					       ellipsoidHeight*0.5,        // x half width
					       ellipsoidWidth*0.5,         // y half width
					       length*0.5 - lengthSafety); // z half width

  G4VSolid* poleAnnulus = new G4Tubs(name + "_pole_annulus_solid", // name
				     annulusStartRadius,           // start radius
				     annulusFinishRadius,          // finish radius
				     length*0.5 - lengthSafety,    // z half length
				     -poleAngle*0.5,               // start angle
				     poleAngle);                   // sweep angle
  
  // note annulus is defined from centre of rotation
  G4ThreeVector ellipsoidTranslation(poleStartRadius + ellipsoidHeight*0.5, 0, 0);
  // the centre of the union solid will be that of the 1st solid - the annulus - ie 0,0,0
  G4VSolid* aSinglePoleSolid = new G4UnionSolid(name + "_pole_solid", // name
						poleAnnulus,          // solid 1
						poleTip,              // solid 2
						0,                    // rotation matrix
						ellipsoidTranslation);// translation of poleTip
  allSolids.push_back(poleTip);
  allSolids.push_back(poleAnnulus);
  allSolids.push_back(aSinglePoleSolid);
  
  // create different poles to fit inside square yoke
  G4RotationMatrix* iPoleRM;
  for (G4int i = 0; i < nPoles; ++i)
    {
      iPoleRM = new G4RotationMatrix();
      G4double segmentAngle = CLHEP::twopi/nPoles; // angle per pole
      G4double rotationAngle = (0.5-i)*segmentAngle + CLHEP::pi*0.5;
      iPoleRM->rotateZ(rotationAngle);
      allRotationMatrices.push_back(iPoleRM);
      // crop the singlepolesolid with the cropping box so it'll fit inside the outer square yoke
      G4IntersectionSolid* aSolid = new G4IntersectionSolid(name + "_pole_solid", // name
							    aSinglePoleSolid,
							    croppingBoxSolid,     // solid 2 - the one to be shifted
							    iPoleRM,              // rotation matrix
							    (G4ThreeVector)0);    // translation vector
					  
								
      poleSolids.push_back(aSolid);
    }
}

void BDSMagnetOuterFactoryPolesSquare::CreateYokeAndContainerSolid(G4String name,
								   G4double length,
								   G4int    /*order*/,
								   G4double magnetContainerLength)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // square yoke - have to do subtraction between two solid boxes
  G4VSolid* yokeOuter = new G4Box(name + "_yoke_outer_solid", // name
				  yokeFinishRadius,           // x half width
				  yokeFinishRadius,           // y half width
				  length*0.5 - lengthSafety); // z half length

  G4VSolid* yokeInner = new G4Box(name + "_yoke_inner_solid", // name
				  yokeStartRadius,            // x half width
				  yokeStartRadius,            // y half width
				  length);                    // z half length
  // inner length is 2x as long for unambiguous subtraction
  allSolids.push_back(yokeOuter);
  allSolids.push_back(yokeInner);

  yokeSolid = new G4SubtractionSolid(name + "_yoke_solid",    // name
				     yokeOuter,
				     yokeInner);

  // note container must have hole in it for the beampipe to fit in!
  // poled geometry doesn't fit tightly to beampipe so can alays use a circular aperture

  G4VSolid* containerOuter = new G4Box(name + "_container_outer_solid", // name
				       yokeFinishRadius,                // x half width
				       yokeFinishRadius,                // y half width
				       length*0.5);                     // x half width

  G4VSolid* containerInner = new G4Tubs(name + "_container_inner_solid", // name
					0,                               // inner radius
					poleStartRadius,                 // finish radius
					length,                          // z half length
					0,                               // start angle
					CLHEP::twopi);                   // sweep angle
  // length of inner is long for unambiguous subtraction
  allSolids.push_back(containerOuter);
  allSolids.push_back(containerInner);

  containerSolid = new G4SubtractionSolid(name + "_outer_container_solid", // name
					  containerOuter,
					  containerInner);

  magnetContainerSolid = new G4Box(name + "_container_solid", // name
				   magnetContainerRadius,     // x half length
				   magnetContainerRadius,     // y half length
				   magnetContainerLength*0.5);// z half length

  magContExtentX = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentY = std::make_pair(-magnetContainerRadius, magnetContainerRadius);
  magContExtentX = std::make_pair(-magnetContainerLength*0.5, magnetContainerLength*0.5);
}

void BDSMagnetOuterFactoryPolesSquare::CreateLogicalVolumes(G4String    name,
							    G4double    length,
							    G4Colour*   colour,
							    G4Material* outerMaterial)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4VisAttributes* outerVisAttr = new G4VisAttributes(*colour);
  outerVisAttr->SetVisibility(true);
  outerVisAttr->SetForceLineSegmentsPerCircle(nSegmentsPerCircle);
  allVisAttributes.push_back(outerVisAttr);

  for (G4int n = 0; n < 2*order; ++n)
    {
      G4LogicalVolume* thisPole = new G4LogicalVolume(poleSolids[n],
						      outerMaterial,
						      name + "_pole_lv");
      thisPole->SetVisAttributes(outerVisAttr);
      poleLVs.push_back(thisPole);
      allLogicalVolumes.push_back(thisPole);
    }

  // yoke
  yokeLV   = new G4LogicalVolume(yokeSolid,
				 outerMaterial,
				 name + "_yoke_lv");
  yokeLV->SetVisAttributes(outerVisAttr);

  // container
  G4Material* emptyMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->EmptyMaterial());
  containerLV = new G4LogicalVolume(containerSolid,
				    emptyMaterial,
				    name + "_container_lv");
  if (visDebug)
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetVisibleDebugVisAttr());}
  else
    {containerLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());}

  // USER LIMITS
  // set user limits based on bdsim user specified parameters
#ifndef NOUSERLIMITS
  G4UserLimits* outerUserLimits = new G4UserLimits("outer_cuts");
  outerUserLimits->SetMaxAllowedStep( length * maxStepFactor );
  outerUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->MaxTime());
  allUserLimits.push_back(outerUserLimits);
  //attach cuts to volumes
  yokeLV->SetUserLimits(outerUserLimits);
  containerLV->SetUserLimits(outerUserLimits);
  std::vector<G4LogicalVolume*>::iterator j;
  for(j = poleLVs.begin(); j != poleLVs.end(); ++j)
    {(*j)->SetUserLimits(outerUserLimits);}
#endif
}

void BDSMagnetOuterFactoryPolesSquare::PlaceComponents(G4String name,
						       G4int    order)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // PLACEMENT
  // place the components inside the container
  // note we don't need the pointer for placements - it's registered upon construction with g4
  yokePV = new G4PVPlacement((G4RotationMatrix*)nullptr,   // no rotation
			     (G4ThreeVector)0,             // position
			     yokeLV,                       // lv to be placed
			     name + "_yoke_pv",            // name
			     containerLV,                  // mother lv to be placed in
			     false,                        // no boolean operation
			     0,                            // copy number
			     checkOverlaps);               // whether to check overlaps
  
  // pole placement
  G4double nPoles = 2*order;
  G4PVPlacement* aPlacement = nullptr;
  for (G4int n = 0; n < 2*order; ++n)
    {
      G4RotationMatrix* rm = new G4RotationMatrix();
      G4double segmentAngle = CLHEP::twopi/nPoles; // angle per pole
      G4double rotationAngle = (0.5-n)*segmentAngle + CLHEP::pi*0.5;
      rm->rotateZ(-rotationAngle);
      allRotationMatrices.push_back(rm);
      // only need to test the end of one iterator as both should be the same length
      aPlacement = new G4PVPlacement(rm,                 // rotation
				     (G4ThreeVector)0,   // position
				     poleLVs[n],         // logical volume
				     name + "_pole_pv",  // name      
				     containerLV,        // mother lv to be placed in
				     false,              // no boolean operation
				     n,                  // copy number
				     checkOverlaps);     // check overlaps
      allPhysicalVolumes.push_back(aPlacement);
      //name + "_pole_" + printf("_%d_pv", n), // name
      }
}

BDSMagnetOuter* BDSMagnetOuterFactoryPolesSquare::CommonConstructor(G4String     name,
								    G4double     length,
								    BDSBeamPipe* beamPipe,
								    G4int        order,
								    G4double     outerDiameter,
								    G4Material*  outerMaterial,
								    G4double     magnetContainerRadius)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnetOuter* outer = BDSMagnetOuterFactoryPolesBase::CommonConstructor(name, length, beamPipe,
									    order, outerDiameter,
									    outerMaterial, magnetContainerRadius);
  
  outer->RegisterLogicalVolume(poleLVs);
  outer->RegisterSensitiveVolume(poleLVs);
  
  return outer;
}
  
