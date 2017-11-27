/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "G4UnionSolid.hh"
#include "G4SubtractionSolid.hh"
#include "G4EllipticalTube.hh"
#include "G4Box.hh"
#include "G4RotationMatrix.hh"
#include "G4ThreeVector.hh"
#include "G4String.hh"

#include "BDSPCLTube.hh"


BDSPCLTube::BDSPCLTube(G4double sizeX, G4double sizeYUp, G4double sizeYDown, G4double sizeDy, G4double thickness, G4double length, G4String name){
  itsSizeX=sizeX;
  itsSizeYUp=sizeYUp;
  itsSizeYDown=sizeYDown;
  itsSizeDy=sizeDy;
  itsThickness=thickness;
  itsLength=length;
  itsName=name;

  if (itsThickness<=0){
    Build();
  }  else {
    BuildHollow();
  }
}

void BDSPCLTube::BuildHollow(){
#ifdef BDSDEBUG
  G4cout << "BDSPCLTube itsSizeX: " << itsSizeX/CLHEP::m << " m" << G4endl;
  G4cout << "BDSPCLTube itsSizeYUp: " << itsSizeYUp/CLHEP::m << " m" << G4endl;
  G4cout << "BDSPCLTube itsSizeYDown: " << itsSizeYDown/CLHEP::m << " m" << G4endl;
  G4cout << "BDSPCLTube Dy: " << itsSizeDy/CLHEP::m << " m" << G4endl;
#endif

  G4RotationMatrix* nullrot  = new G4RotationMatrix();
  G4ThreeVector threeVector1;

  threeVector1.setX(0);
  threeVector1.setZ(0);

  //Make the upper ellipse
  threeVector1.setY(0);
  G4VSolid* tmp_solid_1 =new G4SubtractionSolid("tmp_solid_1",
						new G4EllipticalTube(itsName+"_tmp_solid_1_a",
								     itsSizeX+itsThickness,
								     itsSizeYUp+itsThickness,
								     itsLength/2.0),
						new G4EllipticalTube(itsName+"_tmp_solid_1_b",
								     itsSizeX,
								     itsSizeYUp,
								     itsLength),					
						nullrot,
						threeVector1);


  threeVector1.setY(-1*(itsSizeYUp*2));
  
  upper_solid =   new G4SubtractionSolid("upper_solid",
					 tmp_solid_1,
					 new G4Box(itsName+"_upper_solid_a",
						   itsSizeX*2.0,
						   itsSizeYUp*2.0,
						   itsLength),
					 nullrot,
					 threeVector1);

  threeVector1.setY(0);

   middle_solid =new G4SubtractionSolid(itsName+"middle_solid",
						new G4Box(itsName+"_middle_solid_a",
							  itsSizeX+itsThickness,
							  itsSizeDy/2.0,//+1mu
							  itsLength/2.0),
						new G4Box(itsName+"_middle_solid_b",
							  itsSizeX,
							  itsSizeDy*2.0,
							  itsLength),
						nullrot,
						threeVector1);

    threeVector1.setY(0);
  G4VSolid* tmp_solid_2 =new G4SubtractionSolid("tmp_solid_2",
						new G4EllipticalTube(itsName+"_tmp_solid_2_a",
								     itsSizeX+itsThickness,
								     itsSizeYDown+itsThickness,
								     itsLength/2),
						new G4EllipticalTube(itsName+"_tmp_solid_2_b",
								     itsSizeX,
								     itsSizeYDown,
								     itsLength),					
						nullrot,
						threeVector1);

  threeVector1.setY(itsSizeYDown*2);
  lower_solid =new G4SubtractionSolid("lower_solid",
					    tmp_solid_2,
					    new G4Box(itsName+"_lower_solid_a",
						      itsSizeX*2.0,
						      itsSizeYDown*2.0,
						      itsLength),
					    nullrot,
					    threeVector1);

  //Fuse together the three shapes...
  threeVector1.setY(-itsSizeDy/2.0);
  temp_solid = new G4UnionSolid("temp_solid",
				      upper_solid,
				      middle_solid,
				      nullrot,
				      threeVector1
				      );

  threeVector1.setY(-itsSizeDy);
  solid = new G4UnionSolid("temp_solid",
				 temp_solid,
				 lower_solid,
				 nullrot,
				 threeVector1
				 );

}

void BDSPCLTube::Build(){

#ifdef BDSDEBUG
  G4cout << "BDSPCLTube itsSizeX: " << itsSizeX/CLHEP::m << " m" << G4endl;
  G4cout << "BDSPCLTube itsSizeYUp: " << itsSizeYUp/CLHEP::m << " m" << G4endl;
  G4cout << "BDSPCLTube itsSizeYDown: " << itsSizeYDown/CLHEP::m << " m" << G4endl;
  G4cout << "BDSPCLTube Dy: " << itsSizeDy/CLHEP::m << " m" << G4endl;
#endif

  G4RotationMatrix* nullrot  = new G4RotationMatrix();
  G4ThreeVector threeVector1;

  threeVector1.setX(0);
  threeVector1.setZ(0);

  //Make the upper ellipse
  threeVector1.setY(0);
  G4VSolid* tmp_solid_1 =new G4EllipticalTube("tmp_solid_1",
					      itsSizeX,
					      itsSizeYUp,
					      itsLength/2);
  threeVector1.setY(-1*(itsSizeYUp*2));
  
  upper_solid =   new G4SubtractionSolid("upper_solid",
					 tmp_solid_1,
					 new G4Box(itsName+"_upper_solid_a",
						   itsSizeX*2.0,
						   itsSizeYUp*2.0,
						   itsLength),
					 nullrot,
					 threeVector1);

  threeVector1.setY(0);

   middle_solid =new G4Box(itsName+"_middle_solid",
			   itsSizeX,
			   itsSizeDy/2.0+1*CLHEP::um,
			   itsLength/2.0);
   
   threeVector1.setY(0);

   G4VSolid* tmp_solid_2 =new G4EllipticalTube(itsName+"_tmp_solid_2",
						   itsSizeX,
						   itsSizeYDown,
						   itsLength/2);
   
   threeVector1.setY(itsSizeYDown*2);
   lower_solid =new G4SubtractionSolid("lower_solid",
				       tmp_solid_2,
				       new G4Box(+"_lower_solid_a",
						 itsSizeX*2.0,
						 itsSizeYDown*2.0,
						 itsLength),
				       nullrot,
				       threeVector1);

  //Fuse together the three shapes...
  threeVector1.setY(-itsSizeDy/2.0);
  temp_solid = new G4UnionSolid("temp_solid",
				      upper_solid,
				      middle_solid,
				      nullrot,
				      threeVector1
				      );

  threeVector1.setY(-itsSizeDy);
  solid = new G4UnionSolid("temp_solid",
				 temp_solid,
				 lower_solid,
				 nullrot,
				 threeVector1
				 );
  
}

G4VSolid* BDSPCLTube::GetSolid(){
#ifdef BDSDEBUG
  G4cout << "Returning solid..." << G4endl;
#endif
  return solid;
}

BDSPCLTube::~BDSPCLTube()
{
}
