
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
  (*st)["k1"] = 1.0;
  (*st)["k2"] = 0.2;
  (*st)["k3"] = 0.03;
  (*st)["k4"] = 0.004;
  (*st)["k5"] = 0.0005;

  const G4double brho = 34.456;

  std::vector<BDSFieldType> types = {
    BDSFieldType::quadrupole,
    BDSFieldType::sextupole,
    BDSFieldType::octupole,
    BDSFieldType::decapole,
    BDSFieldType::multipole
  };
  std::vector<BDSField*> fields;
  for (auto t : types)
    {fields.push_back(BDSFieldFactory::Instance()->CreateFieldMagLocal(t,st,brho));}


  const G4int    nR    = 20;
  const G4int    nPhi  = 40;
  const G4double rMax  = 20; // mm
  const G4double rStep = rMax / (G4double) nR;
  const G4double pStep = CLHEP::twopi / (G4double) nPhi;
  
  for (auto field : fields)
    {
      std::ofstream file;
      file.open((field->Name()+".dat").c_str());
      file << "(x,y,z)\t\tField\n";
      for (G4double r = 0.0; r <= rMax; r+= rStep)
	{
	  for (G4double phi = 0; phi <= CLHEP::twopi; phi+=pStep)
	    {
	      G4double x = r*cos(phi);
	      G4double y = r*sin(phi);
	      G4ThreeVector position(x,y,0);
	      file << position << "\t" << field->GetFieldValue(position) << "\n";
	    }
	}
      file.close();
    }

  return 0;
}
