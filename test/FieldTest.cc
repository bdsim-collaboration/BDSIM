
#include "BDSFieldMag.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldType.hh"
#include "BDSIntegratorType.hh"
#include "BDSMagnetStrength.hh"

#include "BDSFieldMagDipole.hh"
#include "BDSFieldMagQuadrupole.hh"
#include "BDSFieldMagSextupole.hh"
#include "BDSFieldMagOctupole.hh"
#include "BDSFieldMagDecapole.hh"
#include "BDSFieldMagSkewOwn.hh"
#include "BDSFieldMagMuonSpoiler.hh"
#include "BDSFieldMagMultipole.hh"
#include "BDSFieldMagMultipoleOuter.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
//#include <string>
#include <vector>


int main(int /*argc*/, char** /*argv*/)
{
  BDSMagnetStrength* st = new BDSMagnetStrength();
  (*st)["field"] = 1.3*CLHEP::tesla;   // T
  (*st)["angle"] = 0.014; // mrad
  (*st)["k1"] = 0.12;
  (*st)["k2"] = 0.02;
  (*st)["k3"] = 0.003;
  (*st)["k4"] = 0.0004;
  (*st)["k5"] = 0.00005;

  BDSMagnetStrength* st2 = new BDSMagnetStrength();
  (*st2)["k1"] = 0.12;
  (*st2)["k2"] = 0.02;
  (*st2)["k3"] = -0.003;
  (*st2)["k4"] = 0.0004;
  (*st2)["k5"] = -0.00005;

  BDSMagnetStrength* st3 = new BDSMagnetStrength();
  (*st3)["k1"] = -0.532;

  const G4double brho = 0.3456;

  std::vector<std::string> names = {"dipole", "quadrupole", "sextupole", "octupole", "decapole",
				    "skewqaudrupole", "skewsextupole", "skewoctupole",
				    "skewdecapole", "muonspoiler", "multipole",
				    "multipoleouterdipole", "multipoleouterquadrupole",
				    "multipoleoutersextupole", "multipoleouteroctupole",
				    "multipoleouterdecapole", "multipoleouterquadrupole-ve",
				    "skewmultipoleouterquadrupole", "skewmultipoleouterssextupole",
				    "skewmultipoleouteroctupole", "skewmultipoleouterdecapole"};
  
  std::vector<BDSFieldMag*> fields;
  
  G4ThreeVector unitDirection = G4ThreeVector(0,(*st)["field"],0);
  unitDirection = unitDirection.unit(); // ensure unit vector
  fields.push_back(new BDSFieldMagDipole(st, brho, unitDirection));
  fields.push_back(new BDSFieldMagQuadrupole(st, brho));
  fields.push_back(new BDSFieldMagSextupole(st, brho));
  fields.push_back(new BDSFieldMagOctupole(st, brho));
  fields.push_back(new BDSFieldMagDecapole(st, brho));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagQuadrupole(st, brho), CLHEP::pi/4.));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagSextupole(st, brho), CLHEP::pi/6.));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagOctupole(st, brho), CLHEP::pi/8.));
  fields.push_back(new BDSFieldMagSkewOwn(new BDSFieldMagDecapole(st, brho), CLHEP::pi/10.));
  fields.push_back(new BDSFieldMagMuonSpoiler(st, brho));
  fields.push_back(new BDSFieldMagMultipole(st2, brho));

  G4double poleTipRadius  = 40;
  G4double beamPipeRadius = 25;
  BDSFieldMag* innerField;
  BDSFieldMag* field;

  // outer dipole
  innerField = new BDSFieldMagDipole(st, brho, unitDirection);
  field = new BDSFieldMagMultipoleOuter(1, st, poleTipRadius, innerField, beamPipeRadius);
  fields.push_back(field);

  // outer quadrupole
  innerField = new BDSFieldMagQuadrupole(st, brho);
  field = new BDSFieldMagMultipoleOuter(2, st, poleTipRadius, innerField, beamPipeRadius);
  fields.push_back(field);

  // outer sextupole
  innerField = new BDSFieldMagSextupole(st, brho);
  field = new BDSFieldMagMultipoleOuter(3, st, poleTipRadius, innerField, beamPipeRadius);
  fields.push_back(field);

  // outer octupole
  innerField = new BDSFieldMagOctupole(st, brho);
  field = new BDSFieldMagMultipoleOuter(4, st, poleTipRadius, innerField, beamPipeRadius);
  fields.push_back(field);

  // outer decapole
  innerField = new BDSFieldMagDecapole(st, brho);
  field = new BDSFieldMagMultipoleOuter(5, st, poleTipRadius, innerField, beamPipeRadius);
  fields.push_back(field);

  // outer quad with -ve k1
  innerField = new BDSFieldMagQuadrupole(st3, brho); // -ve k1
  field = new BDSFieldMagMultipoleOuter(2, st3, poleTipRadius, innerField, beamPipeRadius);
  fields.push_back(field);

  // outer skew quadrupole
  BDSFieldMag* normalField;
  innerField = new BDSFieldMagQuadrupole(st, brho);
  normalField = new BDSFieldMagMultipoleOuter(2, st, poleTipRadius, innerField, beamPipeRadius);
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/4.);
  fields.push_back(field);

  // outer skew sextupole
  innerField = new BDSFieldMagSextupole(st, brho);
  normalField = new BDSFieldMagMultipoleOuter(3, st, poleTipRadius, innerField, beamPipeRadius);
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/6.);
  fields.push_back(field);

  // outer skew octupole
  innerField = new BDSFieldMagOctupole(st, brho);
  normalField = new BDSFieldMagMultipoleOuter(4, st, poleTipRadius, innerField, beamPipeRadius);
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/8.);
  fields.push_back(field);

  // outer skew decapole
  innerField = new BDSFieldMagDecapole(st, brho);
  normalField = new BDSFieldMagMultipoleOuter(5, st, poleTipRadius, innerField, beamPipeRadius);
  field = new BDSFieldMagSkewOwn(normalField, CLHEP::pi/10.);
  fields.push_back(field);
  
  // Angular data
  const G4int    nR    = 20;
  const G4int    nPhi  = 100;
  const G4double rMax  = 100; // mm
  const G4double rStep = rMax / (G4double) nR;
  const G4double pStep = CLHEP::twopi / (G4double) (nPhi-1);

  // Regular carteasian grid - symmetric for x,y just now
  const G4int    nX    = 100;
  const G4double xMin  = -100; // mm 
  const G4double xMax  = 100;  // mm
  const G4double xStep = (xMax - xMin) / (G4double) (nX-1);
   
  for (int f = 0; f < (int)fields.size(); ++f)
    {
      field = fields[f]; // overwrite local pointer variable
      std::string nm = names[f];
      G4cout << "Generating field for type \"" << nm << "\"" << G4endl;
      std::ofstream rfile;
      rfile.open(std::string(nm+"_radial.dat").c_str());
      rfile << "> nR = "   << nR   << "\n";
      rfile << "> nPhi = " << nPhi << "\n";
      rfile << "> brho = " << brho << "\n";
      rfile << "# (x,y,z)\t\tField\n";
      G4double r, phi;
      G4int i, j;
      for (r=rStep, i=0; i < nR; r+= rStep, i++)
	{
	  for (phi=0, j=0; j < nPhi; phi+=pStep, j++)
	    {
	      G4double x = r*cos(phi);
	      G4double y = r*sin(phi);
	      G4ThreeVector position(x,y,0);
	      rfile << position << "\t" << field->GetField(position)/CLHEP::tesla << "\n";
	    }
	}
      rfile.close();
      
      std::ofstream cfile;
      cfile.open(std::string(nm+"_carteasian.dat").c_str());
      cfile << "> nX = "   << nX   << "\n";
      cfile << "> nY = "   << nX   << "\n";
      cfile << "> brho = " << brho << "\n";
      cfile << "# (x,y,z)\t\tField\n";
      G4double x,y;
      for (y=xMin, i=0; i < nX; y+=xStep, ++i)
	{
	  for (x=xMin, j=0; j < nX; x+=xStep, ++j)
	    {
	      G4ThreeVector position(x,y,0);
	      cfile << position << "\t" << field->GetField(position)/CLHEP::tesla << "\n";
	    }
	}
      cfile.close();
    }


  /*
  field = fields[fields.size() - 4]; // should be quadrupole
  G4double xMin2 = -100;
  G4double xMax2 = 100;
  G4double xStep2 = (xMax2 - xMin2)/ (G4double) (nX-1);
  for (G4double x = xMin2, i=0; i < nX; x += xStep2, ++i)
    {
      G4ThreeVector pos(x,10,0);
      G4cout << field->GetField(pos)/CLHEP::tesla << G4endl;
    }
  */
  return 0;
}
