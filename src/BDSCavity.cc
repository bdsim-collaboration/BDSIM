#include "BDSCavity.hh"
#include "BDSAcceleratorComponent.hh"
#include "globals.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"


#include "G4GenericPolycone.hh"
#include "G4Polycone.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"

#include "G4ElectroMagneticField.hh"

#include <cmath>
#include <vector>
BDSCavity::BDSCavity(G4String name, //Any others to add here? 
		     G4double length,
		     G4String type,
		     G4Material* cavityMaterialIn,
		     G4Material* vacuumMaterialIn,
		     G4double cavityRadiusIn,
		     G4double irisRadiusIn,
		     G4double thicknessIn
		     ) : 
  BDSAcceleratorComponent(name, length, 0, type),
  cavityMaterial(cavityMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  cavityRadius(cavityRadiusIn),
  irisRadius(irisRadiusIn),
  thickness(thicknessIn)
{
}

void BDSCavity::BuildField()
{
}

void BDSCavity::AttachField()
{ 
}


void BDSCavity::PlaceComponents()
{
  G4PVPlacement* vacuumPV = new G4PVPlacement(0, //Rotation
					      G4ThreeVector(0,0,0), //Position
					      vacuumLV,             //Logical Volume to be place
					      name + "_vacuum_pv",  //placement name
					      containerLogicalVolume,          //mother volume
					      false,                //pMany unused
					      0,                    //copy number
					      checkOverlaps         //check overlaps
					      );
  
  RegisterPhysicalVolume(vacuumPV);
  
  G4PVPlacement* cavityPV = new G4PVPlacement(0, //Rotation
					      G4ThreeVector(0,0,0), //Position
					      cavityLV,             //Logical Volume to be place
					      name + "_cavity_pv",  //placement name
					      containerLogicalVolume,          //mother volume
					      false,                //pMany unused
					      0,                    //copy number
					      checkOverlaps         //check overlaps
					      );
  RegisterPhysicalVolume(cavityPV); 


}


void BDSCavity::BuildContainerLogicalVolume() 
{
  containerSolid = new G4Tubs(name + "_container_solid", //name
			      0.0,                       //innerRadius
			      cavityRadius + thickness + lengthSafety, //outerRadius
			      chordLength *0.5 + lengthSafety,           //half length
			      0.0,                        //starting angle
			      2.0*CLHEP::pi);               //spanning angle

  SetExtentX(-cavityRadius*0.5,cavityRadius*0.5);
  SetExtentY(-cavityRadius*0.5,cavityRadius*0.5);
  SetExtentZ(-chordLength*0.5,chordLength*0.5);
  
  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");

    }

void BDSCavity::BuildEllipticalCavityGeometry(/*G4double irisRSemiAxis, //iris ellipse vertical semiaxis.
					      G4double irisZSemiAxis, //iris ellipse horizontal semiaxis
					      G4double equatorRSemiAxis, //equator ellipse vertical semiaxis
					      G4double equatorZSemiAxis, //equator ellipse horizontal semiaxis
					      G4double tangentAngle, //Angle of line connecting ellipses to the vertical. Ought to be the common tangent with most negative gradient for reasonable output
					      G4int noPoints //number of points forming each curved section.  Total points for single cavity cell will be 4*noPoints.
					      */)
  {

    //irisRSemiAxis    --> Semi-axis of the iris ellipse perpendicular to the length of the cavity.
    //irisZSemiAxis    --> Semi-axis of the iris ellipse along the length of the cavity.
    //equatorRSemiAxis --> Semi-axis of the equator ellipse perpendicular to the length of the cavity.
    //equatorZSemiAxis --> Semi-axis of the equator ellipse along the length of the cavity.
    //tangentAngle     --> Angle between the line vertical to the z-axis and the line tangent to both ellipses.
    //irisRadius       --> Radius of the iris.
    //noPoints         --> Number of points used to define the shape.  This is divided up between the 3 constituent shapes, 1/4 in each iris ellipse and 1/2 in central dome.
    //length           --> Length of the cell.
    //equatorRadius    --> Radius at the equator.
    G4double equatorRadius = cavityRadius;  //cavityRadius in SRF is the equator radius for clarity.
    
    //::::::::::::::::::::HARD CODED FOR THE TIME BEING:::::::::::::::::::::::::::::::::::::::::::::::
    G4double irisRSemiAxis = 19 * CLHEP::mm ;  //iris ellipse vertical semiaxis.
    G4double irisZSemiAxis = 12 * CLHEP::mm ; //iris ellipse horizontal semiaxis
    G4double equatorRSemiAxis = 42 * CLHEP::mm ;//equator ellipse vertical semiaxis
    G4double equatorZSemiAxis = 42 * CLHEP::mm ; //equator ellipse horizontal semiaxis
    G4double tangentAngle = 13.3 * CLHEP::deg ;
    G4double irisRadius = 35.0 * CLHEP::mm ;
    G4int noPoints = 24;

    //2D spherical coordinates, z along the beamline:
    G4double zi = chordLength/2;   //z coord of iris ellipse centre
    G4double ri = irisRadius + irisRSemiAxis;  //r coord of iris ellipse centre
    G4double ze = 0.0;        //z coord of equator ellipse centre
    G4double re = equatorRadius - equatorRSemiAxis;  //r coord of equator ellipse centre.

    G4double m = tan(tangentAngle + 0.5*CLHEP::pi);  //gradient of line connecting the ellipses
       
   
    // gradient from tangentAngle.  Find the derivative of ellipses.  equate and solve for the parameter.
    G4double irisParameterTangentPoint = atan(-irisRSemiAxis/(m*irisZSemiAxis)) + CLHEP::pi; //Add pi to get desired solution (third quadrant)
    G4double equatorParameterTangentPoint = atan(-equatorRSemiAxis/(m*equatorZSemiAxis));  //first quadrant as desired.
       
    noPoints = noPoints - (noPoints % 4);  //rounding down to a multiple of 4.  This is so that number of points are share equally and consistently between the constituent ellipses.
    //Coordinate names:
    //equatorParameter --> the values of a parameter for generating the central ellipses
    //irisParameter    --> the values of a parameter for generating the iris ellipse
    //rInnerCoord      --> the values of radius for the inner geometry
    //rOuterCoord      --> the values of radius for the outer geometry
    //zInnerCoord      --> the values of z for the inner geometry
    //zOuterCoord      --> the values of z for the outer goemetry

    
    //Vectors for holding said coordinates:
    std::vector<G4double> equatorParameter;
    std::vector<G4double> irisParameter;   
    std::vector<G4double> zInnerCoord;     
    std::vector<G4double> rInnerCoord;     
    std::vector<G4double> zOuterCoord;     
    std::vector<G4double> rOuterCoord;     
    
    //---Generating values for the parameters used to define the ellipse shapes---
    
    //Vector of parameter values for generating the iris ellipse
    for (G4int i = 0; i < noPoints/4; i++) //iris allocated one quarter of points.
      {
	irisParameter.push_back(1.5*CLHEP::pi + (1.5*CLHEP::pi - irisParameterTangentPoint )*i/((noPoints/4)-1));  
      };

    //Vector of parameter values for generating the equator ellipse
    for (G4int i = 0; i < noPoints/2; i++) //equator ellipse allocated half of the points
      {
	equatorParameter.push_back((CLHEP::pi - equatorParameterTangentPoint) - (CLHEP::pi - 2* equatorParameterTangentPoint)*i/((noPoints/2)-1)); 
      };


    //---
    //---Generating the inner cavity shape, starting from -z through to +z, in 3 sections: -z iris -> equator -> +z iris.---
    //---
    
    //-z ellipse:
    for (G4int i = 0; i < noPoints/4; i++) //noPoints/4 because of the total noPoints, the iris ellipse in -z is allocated a quarter of points.
      {
	if (i == 0) //An extra point at the very start for unambigious boolean subtraction later on.
	  {
	    zInnerCoord.push_back(- zi + irisZSemiAxis * cos(irisParameter[i]) - 10*CLHEP::mm); //-10mm so boundaries don't lay upon each other when using boolean operation
	    rInnerCoord.push_back(ri + irisRSemiAxis * sin(irisParameter[i]));
	  }
	zInnerCoord.push_back(- zi + irisZSemiAxis * cos(irisParameter[i]));
	rInnerCoord.push_back(ri + irisRSemiAxis * sin(irisParameter[i]));
      };
      

    //equator ellipse:
    for (G4int i = 0; i < noPoints/2; i++)  //central ellipse is allocated half the total points.
      {
	zInnerCoord.push_back(ze + equatorZSemiAxis * cos(equatorParameter[i]));
	rInnerCoord.push_back(re + equatorRSemiAxis * sin(equatorParameter[i]));
      };
    
    //+z iris ellipse:
    for (G4int i = 0; i < noPoints/4; i++)  //the +z iris ellipse is allocated noPoints/4.
      {
	zInnerCoord.push_back(zi + irisZSemiAxis * cos(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));
	rInnerCoord.push_back(ri + irisRSemiAxis * sin(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));	 
	if (i == (noPoints/4 - 1)) //If last point, add an extra point purely for unambigious boolean subtraction later.
	  {
	    zInnerCoord.push_back(zi + irisZSemiAxis * cos(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]) + 10*CLHEP::mm); //+10mm so boundaries don't lay upon each other
	    rInnerCoord.push_back(ri + irisRSemiAxis * sin(CLHEP::pi - irisParameter[noPoints/4 - 1 - i]));
	  }
      };
    

     //---Generating points for the entire outer cavity shape by using the inner cavity vecotr of coordinates---
     
    for (G4int i = 1; i < zInnerCoord.size() - 1; i++)  //Loop ignores first and last elements which are only for boolean operation.  not wanted for outer cavity geometry.
      {
	G4double gradientAt_i = (rInnerCoord[i-1]-rInnerCoord[i+1])/(zInnerCoord[i-1]-zInnerCoord[i+1]); //calc gradient at point i
	
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
    
    G4cout << "Now printing the values of (zInnerCoord,rInnerCoord):" << G4endl;
    G4cout << "Length of zInnerCoord = " << zInnerCoord.size() << G4endl;
    G4cout << "Length of rInnerCoord = " << rInnerCoord.size() << G4endl;
    for (G4int i = 0; i < zInnerCoord.size(); i++)
      {
	G4cout << "(" << zInnerCoord[i] << "," << rInnerCoord[i] << ")" << G4endl;
      };
    
    G4cout << "Now printing the values of (zOuterCoord,rOuterCoord):" << G4endl;
    G4cout << "Length of zOuterCoord = " << zOuterCoord.size() << G4endl;
    G4cout << "Length of rOuterCoord = " << rOuterCoord.size() << G4endl;
    for (G4int i = 0; i < zOuterCoord.size(); i++)
      {
	G4cout << "(" << zOuterCoord[i] << "," << rOuterCoord[i] << ")" << G4endl;
      };
    
    
    //-------------------------CAVITY SHAPE DEFINITION----------------------------------
    
    std::vector<G4double> solidArrayOuter(noPoints, 0.0); //Array of inner r coordinates.  zeroes ensures the polycone will be solid.
    std::vector<G4double> solidArrayInner(noPoints+2, 0.0); //Array of inner r coordinates.  zeroes ensures the polycone will be solid. 1 extra point either side for unambiguous subtraction.
    
    innerSolid = new G4Polycone(name + "_inner_solid", //name
				0.0,                 //start angle
				2*CLHEP::pi,         //sweep angle
				zInnerCoord.size(),
				zInnerCoord.data(),
				solidArrayInner.data(),
				rInnerCoord.data()   //r coordinates
				);
    
    G4VSolid* outerSolid = new G4Polycone(name + "_outer_solid", //name
					  0.0,                   //start angle
					  2*CLHEP::pi,           //sweep angle
					  zOuterCoord.size(),    //number of corners
					  zOuterCoord.data(),    //zOuterCoord array of points
					  solidArrayOuter.data(),//array of zeroes for solid shape
					  rOuterCoord.data()     //rOuterCoord array of points.
					  );
    
    
    cavitySolid = new G4SubtractionSolid(name + "_cavity_solid",  //name
					 outerSolid,                //solid1
					 innerSolid                 //minus solid2
					 );
    
    
    cavityLV = new G4LogicalVolume(cavitySolid,            //solid
				   cavityMaterial,         //material
				   name + "_cavity_lv"     //name
				    );                      
    
    G4VisAttributes* cavityVis = new G4VisAttributes();
    
    cavityVis->SetColour(0.69,0.769,0.871); //light steel blue
    cavityVis->SetVisibility(true);          //visible
    cavityLV->SetVisAttributes(cavityVis);   //give  colour+visibility to the cavity logical volume
     
    //Delete first and last elements of the InnerCoord Vectors, as these entries
    // were only there to handle boolean operation concerns.  Then Build the vacuum
    //from these two arrays, making sure to take away lengthsafety.
    
    rInnerCoord.erase (rInnerCoord.begin());
    rInnerCoord.pop_back();
    zInnerCoord.erase (zInnerCoord.begin());
    zInnerCoord.pop_back();
    
    G4cout << "The length of rInnerCoord is now:  " << rInnerCoord.size() << G4endl;
    G4cout << "the length of zInnerCoord is now:  " << zInnerCoord.size() << G4endl;
    
    for (G4int i = 0; i < noPoints; i++)
      {
	rInnerCoord[i] = rInnerCoord[i] - lengthSafety;
	
	if (i == 0 || (i == zInnerCoord.size() - 1)) 
	  {
	    zInnerCoord[i] = zInnerCoord[i]- lengthSafety;
	  }
	
      };
    
    
    vacuumSolid = new G4Polycone(name + "_inner_solid", //name
				 0.0,                 //start angle
				 2*CLHEP::pi,         //sweep angle
				 zInnerCoord.size(),
				 zInnerCoord.data(),
				 solidArrayInner.data(),
				 rInnerCoord.data()   //r coordinates
				 );
    
    
    vacuumLV = new G4LogicalVolume(vacuumSolid,           //solid
				   vacuumMaterial,              //material
				   name + "_vacuum_lv");
    
    G4VisAttributes* vacuumVis = new G4VisAttributes(); //vistattributes instance 
    vacuumVis->SetVisibility(true);                    //Make invisible
    vacuumLV->SetVisAttributes(vacuumVis);              //give invisiblity to the vacuum LV.
    
    
  }
//Do some work here on the vacuum. 
void BDSCavity::BuildPillBoxCavityGeometry()
{
  
  //irisRadius should perhaps be changed to some variable higher up the class
  //structure using beamPipeInfo?  
  G4VSolid* outerSolid = new G4Tubs(name + "_outer_solid", //name
				    irisRadius,                   //inner radius
				    cavityRadius - lengthSafety, //cavityRadius
				    0.5 * chordLength - lengthSafety, //half length
				    0.0,                           //startAngle
				    2*CLHEP::pi                    //spanningAngle
				    );
  
  
  //Creates a cylinder from which to subtract from larger cylinder.
  //i.e hollows out the cavity
  innerSolid = new G4Tubs(name + "_inner_solid", //name
			  0.0,            //inner radius 
			  cavityRadius - thickness,           //cavityRadius
			  0.5 * (chordLength - thickness),     //Half length
			  0.0,                   //starAngle
			  2*CLHEP::pi           //spanningAngle
			  );
  
  cavitySolid = new G4SubtractionSolid(name + "_cavity_solid",  //name
				       outerSolid,                //solid1
				       innerSolid                 //minus solid2
				       );

  //Vacuum:  Union of two solids.  One cylinder (VacuumInnerCavity) to fill the centre, and a longer,
  // thinner cylinder (vaccumAperture) to fill the irises -> vacuumSolid
  
  G4VSolid* vacuumInnerCavity = new G4Tubs(name + "_vacuum_inner_cavity_solid",                       //name
					   0.0,                                                //inner radius
					   (cavityRadius - thickness) - lengthSafety,           //outer radius
					   0.5 * (chordLength - thickness) - lengthSafety,     //half length
					   0.0,                                                //start angle
					   2*CLHEP::pi                                        //spanning angle
					   );
  
  G4VSolid* vacuumAperture = new G4Tubs(name +"_vacuum_aperture_solid",        //name
					0.0,                             //inner radius
					irisRadius - lengthSafety,       //outer radius
					0.5 * chordLength - lengthSafety,//length
					0.0,                             //start angle
					2*CLHEP::pi                     //spanning angle
					);
  vacuumSolid = new G4UnionSolid(name + "_vacuum_solid",  //name
				 vacuumInnerCavity,       //solid one
				 vacuumAperture           //Added to solid two.
				 );
  //vacuumSolid = vacuumAperture;
  
  cavityLV = new G4LogicalVolume(cavitySolid,          // solid
				 cavityMaterial,          // material
				 name + "_cavity_lv"); // name

  G4VisAttributes* cavityVis = new G4VisAttributes();

  cavityVis->SetColour(0.69,0.769,0.871); //light steel blue
  cavityVis->SetVisibility(true);          //visible
  cavityLV->SetVisAttributes(cavityVis);   //give  colour+visibility to the cavity logical volume

  
  vacuumLV = new G4LogicalVolume(vacuumSolid,           //solid
				 vacuumMaterial,              //material
				 name + "_vacuum_lv");

  G4VisAttributes* vacuumVis = new G4VisAttributes(); //vistattributes instance 
  vacuumVis->SetVisibility(false);                    //Make invisible
  vacuumLV->SetVisAttributes(vacuumVis);              //give invisiblity to the vacuum LV.

  
  //There is thickness*radius*radius*pi voulme in either end which has no vacuum..  use boolean ops (addition) to build
  
}
