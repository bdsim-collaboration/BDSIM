#include "BDSAcceleratorComponent.hh"
#include "BDSCavity.hh"
#include "BDSCavityInfo.hh"
#include "BDSCavityType.hh"
#include "BDSColours.hh"
#include "BDSExtent.hh"
#include "BDSGlobalConstants.hh"

#include "globals.hh" // geant4 globals / types
#include "G4ChordFinder.hh"
#include "G4ElectroMagneticField.hh"
#include "G4EqMagElectricField.hh"
#include "G4FieldManager.hh"
#include "G4GenericPolycone.hh"
#include "G4LogicalVolume.hh"
#include "G4MagIntegratorDriver.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4SubtractionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4Tubs.hh"
#include "G4UnionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4VSolid.hh"

#include <cmath>
#include <vector>

BDSCavity::BDSCavity(G4String       name,
		     G4double       length,
		     G4double       fieldAmplitudeIn,
		     BDSCavityInfo* cavityInfoIn):
  BDSAcceleratorComponent(name, length, 0, "cavity_"+cavityInfoIn->cavityType.ToString()),
  fieldAmplitude(fieldAmplitudeIn),
  cavityInfo(cavityInfoIn)
{
  cavityRadius = cavityInfo->equatorRadius;
  thickness    = cavityInfo->thickness;
  irisRadius   = cavityInfo->irisRadius;

  if (cavityRadius < (irisRadius + thickness))
    {
      G4cout << "Invalid geometry for \"" << name << "\" - increase equatorRadius" << G4endl;
      exit(1);
    }
}

BDSCavity::~BDSCavity()
{
  delete cavityInfo;
}

void BDSCavity::Build()
{
  switch(cavityInfo->cavityType.underlying())
    {
    case BDSCavityType::elliptical:
      BuildEllipticalCavityGeometry();
      break;
    case BDSCavityType::rectangular:
      BuildPillBoxCavityGeometry();
      break;
    case BDSCavityType::pillbox:
      BuildPillBoxCavityGeometry();
      break;
    default:
      BuildPillBoxCavityGeometry();
      break;
    }
  
  BDSAcceleratorComponent::Build();
  BuildField();
  AttachField();
  PlaceComponents();
}

void BDSCavity::BuildField()
{;}

void BDSCavity::AttachField()
{;}

void BDSCavity::PlaceComponents()
{
  G4PVPlacement* vacuumPV = new G4PVPlacement(nullptr,               //Rotation
					      G4ThreeVector(0,0,0),  //Position
					      vacuumLV,              //Logical Volume to be place
					      name + "_vacuum_pv",   //placement name
					      containerLogicalVolume,//mother volume
					      false,                 //pMany unused
					      0,                     //copy number
					      checkOverlaps);        //check overlaps
  
  RegisterPhysicalVolume(vacuumPV);
  
  G4PVPlacement* cavityPV = new G4PVPlacement(nullptr,               //Rotation
					      G4ThreeVector(0,0,0),  //Position
					      cavityLV,              //Logical Volume to be place
					      name + "_cavity_pv",   //placement name
					      containerLogicalVolume,//mother volume
					      false,                 //pMany unused
					      0,                     //copy number
					      checkOverlaps);        //check overlaps
  
  RegisterPhysicalVolume(cavityPV); 
}

void BDSCavity::BuildContainerLogicalVolume() 
{
  G4double outerRadius = cavityRadius + thickness + lengthSafety;
  containerSolid = new G4Tubs(name + "_container_solid",   //name
			      0.0,                         //innerRadius
			      outerRadius,                 //outerRadius
			      chordLength*0.5,             //half length
			      0.0,                         //starting angle
			      2.0*CLHEP::pi);              //spanning angle

  SetExtent(BDSExtent(outerRadius, outerRadius,  chordLength*0.5));
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
}

void BDSCavity::BuildEllipticalCavityGeometry()
{
  //-----Elliptical Cavity Parameters-----
  //irisRSemiAxis    --> Semi-axis of the iris ellipse perpendicular to the length of the cavity.
  //irisZSemiAxis    --> Semi-axis of the iris ellipse along the length of the cavity.
  //equatorRSemiAxis --> Semi-axis of the equator ellipse perpendicular to the length of the cavity.
  //equatorZSemiAxis --> Semi-axis of the equator ellipse along the length of the cavity.
  //tangentAngle     --> Angle between the line vertical to the z-axis and the line tangent to both ellipses.
  //irisRadius       --> Radius of the iris.
  //noPoints         --> Number of points used to define the shape.  This is divided up between the 3 constituent shapes, 1/4 in each iris ellipse and 1/2 in central dome.
  //length           --> Length of the cell.
  //equatorRadius    --> Radius at the equator.
  G4double equatorRadius = cavityRadius; // cavityRadius in SRF is the equator readius for cavity
  
  G4double irisRSemiAxis    = cavityInfo->irisVerticalAxis;
  G4double irisZSemiAxis    = cavityInfo->irisHorizontalAxis; //iris ellipse horizontal semiaxis
  G4double equatorRSemiAxis = cavityInfo->equatorEllipseSemiAxis ;//equator ellipse vertical semiaxis
  G4double equatorZSemiAxis = cavityInfo->equatorEllipseSemiAxis; //equator ellipse horizontal semiaxis
  G4double tangentAngle     = cavityInfo->tangentLineAngle;
  G4double irisRadius       = cavityInfo->irisRadius;
  unsigned int noPoints            = cavityInfo->numberOfPoints;

  //Calculatecartesian coordinates (z, r) from parameters.
  //2D spherical coordinates, z along the beamline:
  G4double zi = chordLength/2;   //z coord of iris ellipse centre
  G4double ri = irisRadius + irisRSemiAxis;  //r coord of iris ellipse centre
  G4double ze = 0.0;        //z coord of equator ellipse centre
  G4double re = equatorRadius - equatorRSemiAxis;  //r coord of equator ellipse centre.
  G4double m = tan(tangentAngle + 0.5*CLHEP::pi);  //gradient of line connecting the ellipses.  Add a pi/2 because angle is defined from the vertical, clockwise.
   
  // gradient from tangentAngle.  Find the derivative of ellipses.  equate and solve for the parameter.
  G4double equatorParameterTangentPoint = atan(-equatorRSemiAxis/(m*equatorZSemiAxis));  //atan finds solution in the first quadrant.
  G4double irisParameterTangentPoint = atan(-irisRSemiAxis/(m*irisZSemiAxis)) + CLHEP::pi; //Add pi to get desired solution (third quadrant)
 
       
  noPoints = noPoints - (noPoints % 4);  //rounding down to a multiple of 4.  This is so that number of points are share equally and consistently between the constituent ellipses.

  //Vector Definitions:
  //Parametric equation parameters:
  //equatorParameter --> the values of a parameter  for generating the central ellipses
  //irisParameter    --> the values of a parameter  for generating the iris ellipse
  
  //The Cartesian coordinates to be calculated from above parameters:
  //rInnerCoord      --> the values of radius for the inner geometry
  //rOuterCoord      --> the values of radius for the outer geometry
  //zInnerCoord      --> the values of z for the inner geometry
  //zOuterCoord      --> the values of z for the outer goemetry

    
  //Vector declaration:
  std::vector<G4double> equatorParameter;
  std::vector<G4double> irisParameter;   
  std::vector<G4double> rInnerCoord;
  std::vector<G4double> rOuterCoord;
  std::vector<G4double> zInnerCoord;     
  std::vector<G4double> zOuterCoord;     
 
    
  //---Generating values for the parameters used to define the ellipse shapes---
    
  //Vector of parameter values for generating the iris ellipse.  This can then be reflected to find the iris ellipse on the other side.
  for (unsigned int i = 0; i < noPoints/4; i++) //iris allocated one quarter of points.
    {
      irisParameter.push_back(1.5*CLHEP::pi + (1.5*CLHEP::pi - irisParameterTangentPoint )*i/((noPoints/4)-1));  
    };

  //Vector of parameter values for generating the equator ellipse
  for (unsigned int i = 0; i < noPoints/2; i++) //equator ellipse allocated half of the points
    {
      equatorParameter.push_back((CLHEP::pi - equatorParameterTangentPoint) - (CLHEP::pi - 2* equatorParameterTangentPoint)*i/((noPoints/2)-1)); 
    };
  
  
  //---Calculating the coordinates of the inner cavity shape, starting from -z through to +z, in 3 sections: -z iris ellipse -> equator ellipse -> +z iris ellipse.---
  //---Geant requires the coordinates to be done in this (negative to positive)
  
  //-z iris ellipse:
  for (unsigned int i = 0; i < noPoints/4; i++) //noPoints/4 because of the total noPoints, the iris ellipse in -z is allocated a quarter of points.
    {
      if (i == 0) //An extra point at the very start for unambigious boolean subtraction later on.  Will not actually be part of the final geometry.
	{
	  zInnerCoord.push_back(- zi + irisZSemiAxis * cos(irisParameter[i]) - 10*CLHEP::mm); //-10mm so boundaries don't lay upon each other when using boolean operation
	  rInnerCoord.push_back(ri + irisRSemiAxis * sin(irisParameter[i]));
	}
      zInnerCoord.push_back(- zi + irisZSemiAxis * cos(irisParameter[i]));
      rInnerCoord.push_back(ri + irisRSemiAxis * sin(irisParameter[i]));
    };
  
  //equator ellipse (middle):
  for (unsigned int i = 0; i < noPoints/2; i++)  //central ellipse is allocated half the total points.  
    {
      zInnerCoord.push_back(ze + equatorZSemiAxis * cos(equatorParameter[i]));
      rInnerCoord.push_back(re + equatorRSemiAxis * sin(equatorParameter[i]));
    };
  
  //+z iris ellipse:
  for (unsigned int i = 0; i < noPoints/4; i++)  //the +z iris ellipse is allocated noPoints/4.
    {
      zInnerCoord.push_back(zi + irisZSemiAxis * cos(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));
      rInnerCoord.push_back(ri + irisRSemiAxis * sin(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));	 
      if (i == (noPoints/4 - 1)) //If last point, add an extra point purely for unambigious boolean subtraction later.
	{
	  zInnerCoord.push_back(zi + irisZSemiAxis * cos(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]) + 10*CLHEP::mm); //+10mm so boundaries don't lay upon each other
	  rInnerCoord.push_back(ri + irisRSemiAxis * sin(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));
	}
    };
  
  
  //---Generating points for the entire outer cavity shape by using the inner cavity vector of coordinates---
  //This works by calculating the gradient at each point on the curve.  It then finds a unit vector pointing perpendicularly outwards from this point and multiplies
  //by the thickness.  This creates a second vector of points for the outer geometry.
  for (unsigned int i = 1; i < zInnerCoord.size() - 1; i++)  //starting from 1 and ending at n-1 so that the coordinates used for the boolean subtraction are excluded.
    {
      G4double gradientAt_i = (rInnerCoord[i-1]-rInnerCoord[i+1])/(zInnerCoord[i-1]-zInnerCoord[i+1]); //calculate the gradient at point i

      //Since the calculation relies on using the two points either side, and the ends have only one coordinate either side, take the ends as having gradient = 0.
      //So that the vector points vertically upwards.  Otherwise just do the calculation described above.
      if (i == 1 || i == (zInnerCoord.size() - 2))
	{
	  zOuterCoord.push_back(zInnerCoord[i]);
	  rOuterCoord.push_back(rInnerCoord[i] + thickness);
	}
      else
	{
	  zOuterCoord.push_back(- gradientAt_i*thickness * pow( (gradientAt_i * gradientAt_i + 1), -0.5) + zInnerCoord[i]);
	  rOuterCoord.push_back(+ thickness * pow( (gradientAt_i*gradientAt_i + 1), -0.5) + rInnerCoord[i]);
	}
    };

  //---Printing out things:---
#ifdef BDSDEBUG    
  G4cout << "Now printing the values of (zInnerCoord,rInnerCoord):" << G4endl;
  G4cout << "Length of zInnerCoord = " << zInnerCoord.size() << G4endl;
  G4cout << "Length of rInnerCoord = " << rInnerCoord.size() << G4endl;
  for (G4int i = 0; i < (G4int)zInnerCoord.size(); i++)
    {
      G4cout << "(" << zInnerCoord[i] << "," << rInnerCoord[i] << ")" << G4endl;
    };
  
  G4cout << "Now printing the values of (zOuterCoord,rOuterCoord):" << G4endl;
  G4cout << "Length of zOuterCoord = " << zOuterCoord.size() << G4endl;
  G4cout << "Length of rOuterCoord = " << rOuterCoord.size() << G4endl;
  for (G4int i = 0; i < (G4int)zOuterCoord.size(); i++)
    {
      G4cout << "(" << zOuterCoord[i] << "," << rOuterCoord[i] << ")" << G4endl;
    };
#endif

  //Array of inner r coordinates.  zeroes ensures the polycone will be solid.
  std::vector<G4double> solidArrayOuter(noPoints, 0.0); 
  //Array of inner r coordinates.  zeroes ensures the polycone will be solid. 1 extra point either side for unambiguous  boolean subtraction.
  std::vector<G4double> solidArrayInner(noPoints+2, 0.0); 

  //Define the inner solid which is to be subtracted from the outer and also used to define the vacuum.
  innerSolid = new G4Polycone(name + "_inner_solid", //name
			      0.0,                  //start angle
			      2*CLHEP::pi,          //sweep angle
			      zInnerCoord.size(),
			      zInnerCoord.data(),
			      solidArrayInner.data(),
			      rInnerCoord.data());  //r coordinates

  //Define the outer solid from which the inner is subtracted.
  G4VSolid* outerSolid = new G4Polycone(name + "_outer_solid", // name
					0.0,                   // start angle
					2*CLHEP::pi,           // sweep angle
					zOuterCoord.size(),    // number of corners
					zOuterCoord.data(),    // zOuterCoord array of points
					solidArrayOuter.data(),// array of zeroes for solid shape
					rOuterCoord.data());   // rOuterCoord array of points.
  RegisterSolid(outerSolid);
  
  //Do the subtraction
  cavitySolid = new G4SubtractionSolid(name + "_cavity_solid",  // name
				       outerSolid,              // solid1
				       innerSolid);             // minus solid2
  
  //define the logical volume.
  cavityLV = new G4LogicalVolume(cavitySolid,            //solid
				 cavityInfo->material,   //material
				 name + "_cavity_lv");   //name

  //Define the visual attributes in the following 4 lines:
  auto col = BDSColours::Instance()->GetColour("srfcavity");
  G4VisAttributes* cavityVis = new G4VisAttributes(*col);
  cavityVis->SetVisibility(true);
  // wont render if this is adjusted
  //cavityVis->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
  cavityLV->SetVisAttributes(cavityVis);   //give  colour+visibility to the cavity logical volume
  RegisterVisAttributes(cavityVis);
 
  //Delete first and last elements of the InnerCoord Vectors, as these entries.
  //The reason we need to do this is the same vector is also used for making the vacuum filling the cavity, but without the extra points for subtraction.
  rInnerCoord.erase (rInnerCoord.begin());
  rInnerCoord.pop_back();
  zInnerCoord.erase (zInnerCoord.begin());
  zInnerCoord.pop_back();
  
  //Take lengthsafety from all r points and the end z points, to avoid overlap with the cavity.
  for (unsigned int i = 0; i < noPoints; i++)
    {
      rInnerCoord[i] = rInnerCoord[i] - lengthSafety;
      
      if (i == 0 || (i == zInnerCoord.size() - 1)) 
	{zInnerCoord[i] = zInnerCoord[i]- lengthSafety;}
    }
  
  //Initializing the vacuum solid.
  vacuumSolid = new G4Polycone(name + "_inner_solid", //name
			       0.0,                 //start angle
			       2*CLHEP::pi,         //sweep angle
			       zInnerCoord.size(),
			       zInnerCoord.data(),
			       solidArrayInner.data(),
			       rInnerCoord.data()   //r coordinates
			       );
  
  //Initializing the logical volume
  vacuumLV = new G4LogicalVolume(vacuumSolid,           //solid
				 cavityInfo->vacuumMaterial,//material
				 name + "_vacuum_lv");

  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
}

void BDSCavity::BuildPillBoxCavityGeometry()
{
  //Creates a solid 
  G4VSolid* outerSolid = new G4Tubs(name + "_outer_solid",            // name
				    irisRadius,                       // inner radius
				    cavityRadius + thickness,         // outer radius
				    0.5 * chordLength - lengthSafety, // half length
				    0.0,                              // start angle
				    2*CLHEP::pi);                     // sweep angle
  RegisterSolid(outerSolid);
   
  //Creates a cylinder  to subtract from larger cylinder. 
  innerSolid = new G4Tubs(name + "_inner_solid",        // name
			  0.0,                          // inner radius 
			  cavityRadius,                 // outer radius
			  0.5*chordLength - thickness,  // galf length
			  0.0,                          // star angle
			  2*CLHEP::pi);                 // sweep angle

  //Do the subtraction
  cavitySolid = new G4SubtractionSolid(name + "_cavity_solid",    //name
				       outerSolid,                //solid1
				       innerSolid);               //minus solid2

  //Logical volume from cavity solid
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
				 cavityInfo->material, // material
				 name + "_cavity_lv"); // name

  

  //Vacuum:  Union of two solids.  One cylinder (VacuumInnerCavity) to fill the centre, and a longer,
  //thinner cylinder (vacuumAperture) to fill the ends provided by the thickness.

  G4double vacuumInnerRadius = cavityRadius - thickness - lengthSafety;
  G4double vacuumHalfLength  = 0.5*chordLength - thickness - lengthSafety;
  G4VSolid* vacuumInnerCavity = new G4Tubs(name + "_vacuum_inner_cavity_solid",// name
					   0.0,                                // inner radius
					   vacuumInnerRadius,                  // outer radius
					   vacuumHalfLength,                   // half length
					   0.0,                                // start angle
					   2*CLHEP::pi);                       // sweep angle

  G4VSolid* vacuumAperture = new G4Tubs(name + "_vacuum_aperture_solid",   // name
					0.0,                               // inner radius
					irisRadius - lengthSafety,         // outer radius
					0.5 * chordLength - lengthSafety,  // half length
					0.0,                               // start angle
					2*CLHEP::pi);                      // sweep angle
  
  // Create the vacuum as a union of the two solides defined prior
  vacuumSolid = new G4UnionSolid(name + "_vacuum_solid",  // name
				 vacuumInnerCavity,       // solid one
				 vacuumAperture);         // added to solid two.

  // Logical volume from the solid.
  vacuumLV = new G4LogicalVolume(vacuumSolid,                // solid
				 cavityInfo->vacuumMaterial, // material
				 name + "_vacuum_lv");       // name

  // visualisation attributes
  auto col = BDSColours::Instance()->GetColour("srfcavity");
  G4VisAttributes* cavityVis = new G4VisAttributes(*col);
  cavityVis->SetVisibility(true);
  cavityVis->SetForceLineSegmentsPerCircle(BDSGlobalConstants::Instance()->NSegmentsPerCircle());
  cavityLV->SetVisAttributes(cavityVis);
  RegisterVisAttributes(cavityVis);
  
  vacuumLV->SetVisAttributes(BDSGlobalConstants::Instance()->GetInvisibleVisAttr());
}
