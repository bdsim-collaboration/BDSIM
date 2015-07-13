#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"
#include "ggmad.hh"

#include "BDSMaterials.hh"

#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"


GGmadDriver::GGmadDriver(G4String file)
{
  G4cout << __METHOD_NAME__ << "reading file: " << file << G4endl;
  inputf.open(file);
  if(!inputf.good()) {G4cerr<<"ggmad driver: error opening input file "<<file<<G4endl; exit(1);}
}

void GGmadDriver::Construct(G4LogicalVolume *marker)
{
  
  G4String token;
  G4String token1;

  G4double x0=0, y0=0, z0=0, x=0, y=0, z=0; // box parameters
  G4double rmin=0, rmax=0, rmin2=0, rmax2=0, phi0=0, dphi=0; // cylindrical parameters
  G4double x1=0, x2=0, y1=0, y2=0;
  G4double phi=0, theta=0, psi=0; // Euler angles - for rotation of components
  G4String material;

  G4Material *theMaterial;
  G4LogicalVolume *lvol;

  G4VisAttributes *visAttr = new G4VisAttributes(true, G4Colour(0.2,0.2,0.2));
  visAttr->SetForceSolid(true);

  G4int count = 0;

  while (inputf.good())
    {
      token = getWord();
 
      if(token=="Box") {
	if(getWord() == "{")
	  {
	    
	    while((token = getWord()))
	      {
		if (token == "}") break;
		
		getParameter(x0,"x0",token); // origin
		getParameter(y0,"y0",token);
		getParameter(z0,"z0",token);
		getParameter(x,"x",token); //half lengths
		getParameter(y,"y",token);
		getParameter(z,"z",token);
		getParameter(phi,"phi",token);
		getParameter(theta,"theta",token);
		getParameter(psi,"psi",token);
		getParameter(material,"material",token);
	      }

	    //create Box

	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	  
	    G4cout<<"creating box : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;


	    G4Box* aBox = new G4Box("aBox" + G4String(count),
				    x,   // half x
				    y, // half y
				    z ); // half z
	    
	    lvol = new G4LogicalVolume(aBox,
			theMaterial,
			"_bmp_logical");


	    // place into mother volume

	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);

	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
	      rot,		       // rotation
	      G4ThreeVector(x0,y0,z0), // at (x0,y0,z0)
	      lvol,                    // its logical volume
	      "vol_"+G4String(count),  // its name
	      marker,                  // its mother  volume
	      false,		       // no boolean operation
	      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
	    count++;

	  } else error();
	
      }// G4Box

      if(token=="Tubs") {
	if(getWord() == "{")
	  {
	    
	    while((token = getWord()))
	      {
		if (token == "}") break;
		
		getParameter(x0,"x0",token); // origin
		getParameter(y0,"y0",token);
		getParameter(z0,"z0",token);
		getParameter(rmin,"rmin",token); //half lengths
		getParameter(rmax,"rmax",token);
		getParameter(z,"z",token);
		getParameter(phi0,"phi0",token);
		getParameter(dphi,"dphi",token);
		getParameter(phi,"phi",token);
		getParameter(theta,"theta",token);
		getParameter(psi,"psi",token);
		getParameter(material,"material",token);
	      }
	    
	    //create Box
	    
	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	    
	    G4cout<<"creating tubs : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;
	    
	    // default - tube
	    phi0 = 0;
	    dphi = 360; // degrees
	    
	    G4Tubs* aTubs = new G4Tubs("aTubs" + G4String(count),
				       rmin,   // inner R
				       rmax, // outer R
				       z, //z
				       phi0,//phi 0 
				       CLHEP::twopi * dphi / 360  ); //delta phi
	    
	    lvol = new G4LogicalVolume(aTubs,
				       theMaterial,
				       "_bmp_logical");
	    
	    
	    // place into mother volume
	    
	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);

	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
			      rot,			     // rotation
			      G4ThreeVector(x0,y0,z0),     // at (0,0,0)
			      lvol,  // its logical volume
			      "vol_"+G4String(count),	     // its name
			      marker,     // its mother  volume
			      false,		     // no boolean operation
			      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
	    
	    count++;
	    
	  } else error();
      }

      if(token=="Cons") {
	if(getWord() == "{")
	  {
	    
	    while((token = getWord()))
	      {
		if (token == "}") break;
		
		getParameter(x0,"x0",token); // origin
		getParameter(y0,"y0",token);
		getParameter(z0,"z0",token);
		getParameter(rmin,"rmin1",token); //half lengths
		getParameter(rmax,"rmax1",token);
		getParameter(rmin2,"rmin2",token); //half lengths
		getParameter(rmax2,"rmax2",token);
		getParameter(z,"z",token);
		getParameter(phi0,"phi0",token);
		getParameter(dphi,"dphi",token);
		getParameter(phi,"phi",token);
		getParameter(theta,"theta",token);
		getParameter(psi,"psi",token);
		getParameter(material,"material",token);
	      }
	    
	    //create Box
	    
	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	    
	    G4cout<<"creating cons : "<<x0<<"  "<<y0<<" "<<z0<<G4endl;
	    G4cout<<"rotation : "<<phi<<"  "<<theta<<" "<<psi<<G4endl;
	    
	    // default - tube
	    phi0 = 0;
	    dphi = 360; // degrees
	    
	    G4Cons* aCons = new G4Cons("aCons" + G4String(count),
				       rmin,   // inner R
				       rmax,
				       rmin2,
				       rmax2,
				       z, //z
				       phi0,//phi 0 
				       CLHEP::twopi * dphi / 360  ); //delta phi
	    
	    lvol = new G4LogicalVolume(aCons,
				       theMaterial,
				       "_bmp_logical");

	    lvol->SetVisAttributes( visAttr );
	    
	    
	    // place into mother volume

	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);

	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
			      rot,			     // rotation
			      G4ThreeVector(x0,y0,z0),     // at (0,0,0)
			      lvol,  // its logical volume
			      "vol_"+G4String(count),	     // its name
			      marker,     // its mother  volume
			      false,		     // no boolean operation
			      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
	    
	    count++;
	    
	  } else error();
      }
      if(token=="Trd") {   // trapezoid
	if(getWord() == "{")
	  {
	    
	    while((token = getWord()))
	      {
		if (token == "}") break;
		
		getParameter(x0,"x0",token); // origin
		getParameter(y0,"y0",token);
		getParameter(z0,"z0",token);
		getParameter(x1,"x1",token); //half length at wider side
		getParameter(x2,"x2",token); //half length at narrow side
		getParameter(y1,"y1",token); //half lengths
		getParameter(y2,"y2",token); //half lengths
		getParameter(z,"z",token);
		getParameter(phi,"phi",token);
		getParameter(theta,"theta",token);
		getParameter(psi,"psi",token);
		getParameter(material,"material",token);
	      }
	    
	    //create Box
	    
	    theMaterial = BDSMaterials::Instance()->GetMaterial(material);
	    
	    
	    G4cout<<"creating trd : "<<x0<<"  "<<y0<<" "<<z0<<
		  z<<" "<<y<<" "<<x1<<" "<<x2<<G4endl;
	    
	    
	    G4Trd* aTrd = new G4Trd("aTrd" + G4String(count),
				    x1,x2,   // inner R
				    y1,y2,
				    z);
	    
	    lvol = new G4LogicalVolume(aTrd,
				       theMaterial,
				       "_bmp_logical");

	    // place into mother volume

	    G4RotationMatrix *rot = new G4RotationMatrix;
	    rot->rotateX(CLHEP::twopi*phi/360.);
	    rot->rotateY(CLHEP::twopi*theta/360.);
	    rot->rotateZ(CLHEP::twopi*psi/360.);
	    
	    
	    // G4VPhysicalVolume* PhysiComp =
	    new G4PVPlacement(
			      rot,			     // rotation
			      G4ThreeVector(x0,y0,z0),     // at (0,0,0)
			      lvol,  // its logical volume
			      "vol_"+G4String(count),	     // its name
			      marker,     // its mother  volume
			      false,		     // no boolean operation
			      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());
	    
	    count++;
	    
	  } else error();
      }
    }


  return;
}


G4String GGmadDriver::getWord()
{

  G4String str="";
  char c = 'a';

  while (inputf.good())     // loop while extraction from file is possible
  {
    inputf.get(c);       // get character from file

    // return char tokens 
    if(c=='=') return G4String(c);

    // skip whitespace
    if( (c != ' ' ) && (c != '\t' )&& (c != ',' ) && (c != '\n' )) break;
  }

  while (inputf.good())     // loop while extraction from file is possible
  {
    str += c;
    inputf.get(c);       // get character from file
    if( (c == ' ' ) || (c == '\t' )|| (c == ',' )|| (c == '\n' )|| (c == '=' ) ) 
      {
	inputf.putback(c);
	break;
      }
  }

  return str;

}

void GGmadDriver::getParameter(G4double& x, G4String name, G4String lastToken)
{
  G4String token;

  if(lastToken == name)
    {
      token = getWord();
      if(token == "=")
	{
	  token = getWord();
	  x = strtod(token.c_str(),NULL);
	}
    }
}

void GGmadDriver::getParameter(G4String& lval, G4String name, G4String lastToken)
{
  G4String token;

  if(lastToken == name)
    {
      token = getWord();
      if(token == "=")
	{
	  token = getWord();
	  lval = token;
	}
    }
}
