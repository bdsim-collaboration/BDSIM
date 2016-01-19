
#include "BDSField.hh"
#include "BDSFieldFactory.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <cmath>
#include <fstream>
#include <ostream>
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
  (*st2)["k3"] = 0.003;
  (*st2)["k4"] = 0.0004;
  (*st2)["k5"] = 0.00005;

  const G4double brho = 0.3456;

  std::vector<BDSFieldType> types = {
    BDSFieldType::quadrupole,
    BDSFieldType::sextupole,
    BDSFieldType::octupole,
    BDSFieldType::decapole,
    BDSFieldType::multipole
  };
  std::vector<BDSField*> fields;
  for (auto t : types)
    {
      if (t != BDSFieldType::multipole)
	{fields.push_back(BDSFieldFactory::Instance()->CreateFieldMagLocal(t,st,brho));}
      else
	{fields.push_back(BDSFieldFactory::Instance()->CreateFieldMagLocal(t,st2,brho));}
    }

  // Angular data
  const G4int    nR    = 20;
  const G4int    nPhi  = 100;
  const G4double rMax  = 20; // mm
  const G4double rStep = rMax / (G4double) nR;
  const G4double pStep = CLHEP::twopi / (G4double) (nPhi-1);

  // Regular carteasian grid - symmetric for x,y just now
  const G4int    nX    = 10;
  const G4double xMin  = -20; // mm 
  const G4double xMax  = 20;  // mm
  const G4double xStep = (xMax - xMin) / (G4double) (nX-1);
   
  for (auto field : fields)
    {
      G4cout << "Generating field for type \"" << field->Name() << "\"" << G4endl;
      std::ofstream rfile;
      rfile.open((field->Name()+"_radial.dat").c_str());
      rfile << "> nR = "   << nR   << "\n";
      rfile << "> nPhi = " << nPhi << "\n";
      rfile << "> brho = " << brho << "\n";
      rfile << "# (x,y,z)\t\tField\n";
      G4double r, phi;
      G4int i, j;
      for (r=0.0, i=0; i < nR; r+= rStep, i++)
	{
	  for (phi=0, j=0; j < nPhi; phi+=pStep, j++)
	    {
	      G4double x = r*cos(phi);
	      G4double y = r*sin(phi);
	      G4ThreeVector position(x,y,0);
	      rfile << position << "\t" << field->GetFieldValue(position) << "\n";
	    }
	}
      rfile.close();
      
      std::ofstream cfile;
      cfile.open((field->Name()+"_carteasian.dat").c_str());
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
	      cfile << position << "\t" << field->GetFieldValue(position) << "\n";
	    }
	}
      cfile.close();
    }

  return 0;
}
