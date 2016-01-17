
#include "BDSField.hh"
#include "BDSFieldFactory.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"

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

  const G4double brho = 3.456;

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
  
  const G4int    nX    = 100;
  const G4double xMin  = -5*CLHEP::cm;
  const G4double xMax  = +5*CLHEP::cm;
  const G4double xStep = (xMax - xMin) / (G4double) nX;
  const G4int    nY    = 100;
  const G4double yMin  = -5*CLHEP::cm;
  const G4double yMax  = +5*CLHEP::cm;
  const G4double yStep = (yMax - yMin) / (G4double) nY;
  
  for (auto field : fields)
    {
      std::ofstream file;
      file.open((field->Name()+".dat").c_str());
      file << "(x,y,z)\t\tField\n";
      for (G4double x = xMin; x <= xMax; x+=xStep)
	{
	  for (G4double y = yMin; y <= yMax; y+=yStep)
	    {
	      G4ThreeVector position(x,y,0);
	      file << position << "\t" << field->GetFieldValue(position) << "\n";
	    }
	}
      file.close();
    }

  return 0;
}
