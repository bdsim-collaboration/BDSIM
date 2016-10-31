#include "BDSFieldClassType.hh"
#include "BDSFieldType.hh"

#include "BDSIQuery.hh"

#include "parser/query.h"

#include "globals.hh" // geant4 types / globals
#include "G4Field.hh"
#include "G4String.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <fstream>
#include <iostream>

void BDSI::Query(G4Field* field,
		 const GMAD::Query& params,
		 const BDSFieldType fieldType)
{
  BDSFieldClassType fieldClass = BDS::DetermineFieldClassType(fieldType);
  
  switch (params.nDimensions)
    {
    case 1:
      {BDSI::Query1D(field, params, fieldClass); break;}
    case 2:
      {BDSI::Query2D(field, params, fieldClass); break;}
    case 3:
      {BDSI::Query3D(field, params, fieldClass); break;}
    case 4:
      {BDSI::Query4D(field, params, fieldClass); break;}
    default:
      {
	G4cout << "Invalid number of dimensions in query definition:" << G4endl;
	params.print();
	break;
      }
    }
}

void BDSI::Query1D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4int    nX   = G4int(params.nx);
  G4String outputName = G4String(params.outfile);
  
  G4cout << "Querying " << outputName << G4endl;
  
  double xStep = (xmax - xmin) / (G4double)nX;
  
  std::ofstream ofile;
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "xmin> " << xmin/CLHEP::m << "\n";
  ofile << "xmax> " << xmax/CLHEP::m << "\n";
  ofile << "! X     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX;

  G4int i = 0;
  for (G4double x = xmin; x < xmax; x += xStep)
    {
      G4double percentage = ((G4double)i / totalN) *100;
      std::cout << "\r" << floor(percentage) << "%";
      G4double result[6] = {0,0,0,0,0,0};
      G4double coords[4] = {x,0,0,0};
      field->GetFieldValue(coords, result);
      WriteOut(ofile, 1, coords, result, type);
      i++;
    }
  
  ofile.close();
  std::cout << std::endl;
}

void BDSI::Query2D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4double ymin = G4double(params.ymin*CLHEP::m);
  G4double ymax = G4double(params.ymax*CLHEP::m);
  G4int    nX   = G4int(params.nx);
  G4int    nY   = G4int(params.ny);
  G4String outputName = G4String(params.outfile);
  
  G4cout << "Querying " << outputName << G4endl;
  
  double xStep = (xmax - xmin) / (G4double)nX;
  double yStep = (ymax - ymin) / (G4double)nY;
  
  std::ofstream ofile;
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "ny> "   << nY << "\n";
  ofile << "xmin> " << xmin/CLHEP::m << "\n";
  ofile << "xmax> " << xmax/CLHEP::m << "\n";
  ofile << "ymin> " << ymin/CLHEP::m << "\n";
  ofile << "ymax> " << ymax/CLHEP::m << "\n";
  ofile << "! X     Y     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX * (G4double)nY;

  G4int i = 0;
  for (G4double y = ymin; y < ymax; y += yStep)
    {
      G4double percentage = ((G4double)i / totalN) *100;
      std::cout << "\r" << floor(percentage) << "%";
      for (G4double x = xmin; x < xmax; x += xStep)
	{
	  G4double result[6] = {0,0,0,0,0,0};
	  G4double coords[4] = {x,y,0,0};
	  field->GetFieldValue(coords, result);
	  WriteOut(ofile, 2, coords, result, type);
	  i++;
	}
    }
  ofile.close();
  std::cout << std::endl;
}

void BDSI::Query3D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4double ymin = G4double(params.ymin*CLHEP::m);
  G4double ymax = G4double(params.ymax*CLHEP::m);
  G4double zmin = G4double(params.zmin*CLHEP::m);
  G4double zmax = G4double(params.zmax*CLHEP::m);
  G4int    nX   = G4int(params.nx);
  G4int    nY   = G4int(params.ny);
  G4int    nZ   = G4int(params.nz);
  G4String outputName = G4String(params.outfile);
  
  G4cout << "Querying " << outputName << G4endl;
  
  double xStep = (xmax - xmin) / (G4double)nX;
  double yStep = (ymax - ymin) / (G4double)nY;
  double zStep = (ymax - ymin) / (G4double)nZ;
  
  std::ofstream ofile;
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "ny> "   << nY << "\n";
  ofile << "nz> "   << nZ << "\n";
  ofile << "xmin> " << xmin/CLHEP::m << "\n";
  ofile << "xmax> " << xmax/CLHEP::m << "\n";
  ofile << "ymin> " << ymin/CLHEP::m << "\n";
  ofile << "ymax> " << ymax/CLHEP::m << "\n";
  ofile << "zmin> " << zmin/CLHEP::m << "\n";
  ofile << "zmax> " << zmax/CLHEP::m << "\n";
  ofile << "! X     Y     Z     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX * (G4double)nY * (G4double)nZ;

  G4int i = 0;
  for (G4double z = zmin; z < zmax; z += zStep)
    {
      G4double percentage = ((G4double)i / totalN) *100;
      std::cout << "\r" << floor(percentage) << "%";
      for (G4double y = ymin; y < ymax; y += yStep)
	{
	  for (G4double x = xmin; x < xmax; x += xStep)
	    {
	      G4double result[6] = {0,0,0,0,0,0};
	      G4double coords[4] = {x,y,z,0};
	      field->GetFieldValue(coords, result);
	      WriteOut(ofile, 3, coords, result, type);
	      i++;
	    }
	}
    }
  ofile.close();
  std::cout << std::endl;
}

void BDSI::Query4D(G4Field* field, const GMAD::Query& params, const BDSFieldClassType type)
{
  G4double xmin = G4double(params.xmin*CLHEP::m);
  G4double xmax = G4double(params.xmax*CLHEP::m);
  G4double ymin = G4double(params.ymin*CLHEP::m);
  G4double ymax = G4double(params.ymax*CLHEP::m);
  G4double zmin = G4double(params.zmin*CLHEP::m);
  G4double zmax = G4double(params.zmax*CLHEP::m);
  G4double tmin = G4double(params.zmin*CLHEP::s);
  G4double tmax = G4double(params.zmax*CLHEP::s);
  G4int    nX   = G4int(params.nx);
  G4int    nY   = G4int(params.ny);
  G4int    nZ   = G4int(params.nz);
  G4int    nT   = G4int(params.nz);
  G4String outputName = G4String(params.outfile);
  
  G4cout << "Querying " << outputName << G4endl;
  
  double xStep = (xmax - xmin) / (G4double)nX;
  double yStep = (ymax - ymin) / (G4double)nY;
  double zStep = (ymax - ymin) / (G4double)nZ;
  double tStep = (tmax - tmin) / (G4double)nT;
  
  std::ofstream ofile;
  ofile.open(outputName);

  ofile << "nx> "   << nX << "\n";
  ofile << "ny> "   << nY << "\n";
  ofile << "nz> "   << nZ << "\n";
  ofile << "xmin> " << xmin/CLHEP::m << "\n";
  ofile << "xmax> " << xmax/CLHEP::m << "\n";
  ofile << "ymin> " << ymin/CLHEP::m << "\n";
  ofile << "ymax> " << ymax/CLHEP::m << "\n";
  ofile << "zmin> " << zmin/CLHEP::m << "\n";
  ofile << "zmax> " << zmax/CLHEP::m << "\n";
  ofile << "tmin> " << tmin/CLHEP::s << "\n";
  ofile << "tmax> " << tmax/CLHEP::s << "\n";
  ofile << "! X     Y     Z     Fx    Fy    Fz\n";
  
  G4double totalN = (G4double)nX * (G4double)nY * (G4double)nZ;

  G4int i = 0;
  for (G4double t = tmin; t < tmax; t += tStep)
    {
      G4double percentage = ((G4double)i / totalN) *100;
      std::cout << "\r" << floor(percentage) << "%";
      for (G4double z = zmin; z < zmax; z += zStep)
	{
	  for (G4double y = ymin; y < ymax; y += yStep)
	    {
	      for (G4double x = xmin; x < xmax; x += xStep)
		{
		  G4double result[6] = {0,0,0,0,0,0};
		  G4double coords[4] = {x,y,z,t};
		  field->GetFieldValue(coords, result);
		  WriteOut(ofile, 4, coords, result, type);
		  i++;
		}
	    }
	}
    }
  ofile.close();
  std::cout << std::endl;
}

void BDSI::WriteOut(std::ofstream& out,
		    const G4int    nDim,
		    const G4double coords[4],
		    const G4double result[6],
		    const BDSFieldClassType type)
{
  // write coordinates
  for (G4int i = 0; i < nDim; i++)
    {out << coords[i]/CLHEP::m << "\t";}

  // write field components
  switch (type.underlying())
    {
    case BDSFieldClassType::magnetic:
      {	
	out << result[0] / CLHEP::tesla << "\t"
	    << result[1] / CLHEP::tesla << "\t"
	    << result[2] / CLHEP::tesla << "\n";
	break;
      }
    case BDSFieldClassType::electric:
      {
	out << result[3] / CLHEP::volt << "\t"
	    << result[4] / CLHEP::volt << "\t"
	    << result[5] / CLHEP::volt << "\n";
	break;
      }
    case BDSFieldClassType::electromagnetic:
      {
	out << result[0] / CLHEP::tesla << "\t"
	    << result[1] / CLHEP::tesla << "\t"
	    << result[2] / CLHEP::tesla << "\t"
	    << result[3] / CLHEP::volt  << "\t"
	    << result[4] / CLHEP::volt  << "\t"
	    << result[5] / CLHEP::volt  << "\n";
	break;
      }
    default:
      {break;}
    }
}
