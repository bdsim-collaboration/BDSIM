#include "BDSCavity.hh"





BDSCavity::BDSCavity(G4string name, //Any others to add here? 
		     G4String length,
		     G4double type,
		     G4String cavityMaterialIn,
		     G4String vacuumMaterialIn,
		     G4double equatorRadiusIn,
		     G4double irisRadiusIn,
		     G4double thicknessIn
		     ) : 
  BDSAcceleratorComponent(name, length, 0, type),
  cavityMaterial(cavityMaterialIn),
  vacuumMaterial(vacuumMaterialIn),
  equatorRadius(equatorRadiusIn),
  irisRadius(irisRadiusIn),
  thickness(thicknessIn)
{
}

/*void BDSCavity::Build()
{
  BDSAcceleratorComponent::Build();
  BuildEllipticalCavityGeometry(
  }*/
void BuildContainerLogicalVolume()
{
  G4double outerRadius = equatorRadiusIn + lengthSafety;
  containerSolid = new G4Tubs(name + "_container_solid", //name
			      0.0,                       //innerRadius
			      outerRadius                //outerRadius
			      chordLength *0.5,           //half length
			      0.0,                        //starting angle
			      2.0*CLHEP::pi);               //spanning angle

  SetExtentX(-outerRadius*0.5,outerRadius*0.5);
  SetExtentY(-outerRadius*0.5,outerRadius*0.5);
  SetExtentZ(-chordLength*0.5,chordlength*0.5);
  
  cavityLV = new G4LogicalVolume(containerSolid,
					emptyMaterial,
					name + "_container_lv")

    }

  void BuildEllipticalCavityGeometry(G4double irisYSemiAxis, //iris ellipse vertical semiaxis.
				     G4double irisZSemiAxis, //iris ellipse horizontal semiaxis
				     G4double equatorYSemiAxis, //equator ellipse vertical semiaxis
				     G4double equatorZSemiAxis, //equator ellipse horizontal semiaxis
				     G4double tangentAngle, //Angle of line connecting ellipses to the vertical.
				     //Ought to be the common tangent with most negative gradient for reasonable output
				     G4int noPoints //number of points forming each curved section.  Total points for entire quarter section will be 2*nopoints.
				     )
  {




    //Build a cylinder at the moment.

    cavitySolid = new G4Tubs(name + "_cavity_solid",
				    irisRadius,
				    equatorRadius,
				    chordLength*0.5,
				    0.0,
				    2*CLHEP::pi
				    );


    vacuumSolid = new G4Tubs(name + "_vacuum_solid",
			     irisRadius - lengthSafety,
			     0.0,
			     chordLength*0.5 - lengthSafety,
			     0.0,
			     2*CLHEP::pi
			     );

    
    /*
    //Checks on sane input here...

    //Ellipse Centres (Ellipses defined parametrically), iris:
    G4double xi = chordLength;
    G4double yi = irisRadius + irisYSemiAxis;
    //and equator:
    G4double xe = 0.0;
    G4double ye = equatorRadius - equatorYSemiAxis;

    //gradient of line connecting the ellipses
    G4double m = tan(CLHEP::PI*tangentAngle/180 + 0.5*CLHEP::pi);

    //Values of parameters t and u for finding point where line touches each ellipse
    G4double irisUIntercept = atan(-irisYSemiAxis/(m*irisZSemiAxis)) + CLHEP::pi;
    G4double equatorTIntercept = atan(-equatorYSemiAxis/(m*equatorZSemiAxis));

    //Using values of u and t to find Cartesian coords of tangent points
    //iris ellipse tangent point:
    G4double irisXIntercept = xi + irisZSemiAxis*cos(irisUIntercept);
    G4double irisYIntercept = yi + irisYSemiAxis*sin(irisUIntercept);
    //equator ellipse tangent point:
    G4double equatorXIntercept = xe + equatorZSemiAxis*cos(equatorTIntercept);
    G4double equatorYIntercept = ye + equatorYSemiAxis*sin(equatorTIntercept);


    //I need to create something on the heap here.  Remember to delete it later.  I do not know how to do this admittedly yet..
    
    G4double t[nopoints];
    G4double u[nopoints];
    G4double x[nopoints];
    G4double y[nopoints];
    
    G4double *t = new G4double[nopoints];
    G4double *u = new G4double[nopoints];
    G4double *z = new G4double[nopoints];
    G4double *y = new G4double[nopoints];
    */

    
        
  }
