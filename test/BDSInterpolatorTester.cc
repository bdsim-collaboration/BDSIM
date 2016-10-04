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

#include <fstream>
#include <ostream>
#include <string>

int main(int /*argc*/, char** /*argv*/)
{
  double xmin = 0;
  double xmax = 50;
  int    nX   = 1000;
  double ymin = 0;
  double ymax = 50;
  int    nY   = 1000;
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
  int nQueries = nX * nY;
  int modulo   = (int)( (double)nQueries *0.01);

  std::ofstream ofile;
  ofile.open(outputName);

  int i = 0;
  for (double y = ymin; y < ymax; y += yStep)
    {
      for (double x = xmin; x < xmax; x += xStep)
	{
	  if (i/modulo == 0)
	    {
	      std::cout << "\r" << i << "             ";
	    }
	  G4ThreeVector result = field->GetField(G4ThreeVector(x,y,0));
	  ofile << x          << "\t"
		<< y          << "\t"
		<< result.x() << "\t"
		<< result.y() << "\t"
		<< result.z() << "\n";
	  i++;
	}
    }
  ofile.close();
  std::cout << std::endl;

  if (BDSFieldMagInterpolated2D* f = dynamic_cast<BDSFieldMagInterpolated2D*>(field))
    {
      std::cout << *(f->Interpolator()->Array()) << std::endl;
    }

  return 0;
}
