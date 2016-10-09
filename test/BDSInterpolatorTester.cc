#include "BDSArray2DCoords.hh"
#include "BDSArray2DCoordsRQuad.hh"
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

void Query(BDSFieldMag* field,
	   G4double ymin, G4double ymax, G4double xmin, G4double xmax,
	   G4int nX, G4int nY,
	   G4String outputName)
{
  double xStep = (xmax - xmin) / (double)nX;
  double yStep = (ymax - ymin) / (double)nY;
  
  std::ofstream ofile;
  ofile.open(outputName+".dat");

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
  ofile2.open(outputName+"_raw.dat");
  auto r = dynamic_cast<BDSFieldMagInterpolated2D*>(field)->Interpolator()->Array();
  ofile2 << *r;
  ofile2.close();
}

int main(int /*argc*/, char** /*argv*/)
{
  double xmin = -25*CLHEP::cm;
  double xmax = 25*CLHEP::cm;
  int    nX   = 100;
  double ymin = -25*CLHEP::cm;
  double ymax = 25*CLHEP::cm;
  int    nY   = 100;
  std::string outputNameNearest = "nearest.dat";
  std::string outputNameLinear  = "linear.dat";
  std::string arrayName         = "array.dat";

  // Nearest Neighbour
  BDSFieldInfo* infoNearest = new BDSFieldInfo(BDSFieldType::xy,
					       0,
					       BDSIntegratorType::g4classicalrk4,
					       nullptr,
					       false,
					       G4Transform3D(),
					       nullptr,
					       "OUTSF7.TXT",
					       BDSFieldFormat::poisson2dquad,
					       BDSInterpolatorType::nearest2d);
  
  BDSFieldMag* nearest = BDSFieldLoader::Instance()->LoadMagField(*infoNearest);

  // Nearest Neighbour
  BDSFieldInfo* infoLinear = new BDSFieldInfo(BDSFieldType::xy,
					      0,
					      BDSIntegratorType::g4classicalrk4,
					      nullptr,
					      false,
					      G4Transform3D(),
					      nullptr,
					      "OUTSF7.TXT",
					      BDSFieldFormat::poisson2dquad,
					      BDSInterpolatorType::linear2d);
  
  BDSFieldMag* linear = BDSFieldLoader::Instance()->LoadMagField(*infoLinear);

  // Query across full range of magnet including just outside range too.
  Query(nearest, ymin, ymax, xmin, xmax, nX, nY, "nearest");
  Query(linear, ymin, ymax, xmin, xmax, 3*nX, 3*nY, "linear");

  // Now query in small region where there's large variation.
  Query(nearest, 50, 110, 110, 170, nX, nY, "nearest_zoom");
  Query(linear, 50, 110, 110, 170, nX, nY, "linear_zoom");

  G4cout << nearest->GetField(G4ThreeVector(130, 74, 0)) << G4endl;
  G4cout << linear->GetField(G4ThreeVector(130, 74, 0)) << G4endl;

  return 0;
}
