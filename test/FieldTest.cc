
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
#include "BDSFieldMagOuterMultipole.hh"

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
  (*st)["field"] = 1.3;   // T
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

  const G4double brho = 0.3456;

  std::vector<std::string> names = {"dipole", "quadrupole", "sextupole", "octupole", "decapole",
				    "skewqaudrupole", "skewsextupole", "skewoctupole",
				    "skewdecapole", "muonspoiler", "multipole", "multipoleouter"};
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
  fields.push_back(new BDSFieldMagOuterMultipole(4, 3.3, 0));

  // Angular data
  const G4int    nR    = 20;
  const G4int    nPhi  = 100;
  const G4double rMax  = 20; // mm
  const G4double rStep = rMax / (G4double) nR;
  const G4double pStep = CLHEP::twopi / (G4double) (nPhi-1);

  // Regular carteasian grid - symmetric for x,y just now
  const G4int    nX    = 100;
  const G4double xMin  = -20; // mm 
  const G4double xMax  = 20;  // mm
  const G4double xStep = (xMax - xMin) / (G4double) (nX-1);
   
  for (int f = 0; f < (int)fields.size(); ++f)
    {
      BDSFieldMag* field = fields[f];
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
	      rfile << position << "\t" << field->GetField(position) << "\n";
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
	      cfile << position << "\t" << field->GetField(position) << "\n";
	    }
	}
      cfile.close();
    }

  return 0;
}
