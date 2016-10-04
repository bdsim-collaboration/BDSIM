#include "BDSArray2DCoords.hh"
#include "BDSFieldFormat.hh"
#include "BDSFieldInfo.hh"
#include "BDSFieldLoader.hh"
#include "BDSFieldMag.hh"
#include "BDSFieldMagInterpolated2D.hh"
#include "BDSFieldType.hh"
#include "BDSFieldValue.hh"
#include "BDSIntegratorType.hh"
#include "BDSInterpolator2D.hh"
#include "BDSInterpolatorType.hh"

#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <fstream>
#include <ostream>
#include <string>

int main(int /*argc*/, char** /*argv*/)
{
  double xmin = 0;
  double xmax = 25;
  int    nX   = 100;
  double ymin = 0;
  double ymax = 20;
  int    nY   = 100;
  std::string outputName = "outfile.dat";
  
  BDSFieldInfo* info = new BDSFieldInfo(BDSFieldType::xy,
					0,
					BDSIntegratorType::g4classicalrk4,
					nullptr,
					false,
					G4Transform3D(),
					nullptr,
					"OUTSF7.TXT",
					BDSFieldFormat::poisson2d,
					BDSInterpolatorType::nearest2d);
  
  BDSFieldMag* field = BDSFieldLoader::Instance()->LoadMagField(*info);

  double xStep = (xmax - xmin) / (double)nX;
  double yStep = (ymax - ymin) / (double)nY;
  
  std::ofstream ofile;
  ofile.open(outputName);

  int i = 0;
  for (double y = ymin; y < ymax; y += yStep)
    {
      for (double x = xmin; x < xmax; x += xStep)
	{
	  std::cout << "\r" << i;
	  G4ThreeVector result = field->GetField(G4ThreeVector(x,y,0));
	  ofile << x          << "\t"
		<< y          << "\t"
		<< result.x() / CLHEP::tesla << "\t"
		<< result.y() / CLHEP::tesla << "\t"
		<< result.z() / CLHEP::tesla << "\n";
	  i++;
	}
    }
  ofile.close();
  std::cout << std::endl;

  std::ofstream ofile2;
  ofile2.open("test.dat");
  ofile2 << *(dynamic_cast<BDSFieldMagInterpolated2D*>(field)->Interpolator()->Array());
  ofile2.close();

  return 0;
}
