#include "BDSIQuery.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Field.hh"
#include "G4ThreeVector.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <cmath>
#include <fstream>
#include <iostream>


void BDSI::Query2D(G4Field* field,
		   G4String outputName,
		   G4int    nX,   G4int    nY,
		   G4double xmin, G4double xmax,
		   G4double ymin, G4double ymax)
{
  G4cout << "Querying " << outputName << G4endl;
  
  double xStep = (xmax - xmin) / (G4double)nX;
  double yStep = (ymax - ymin) / (G4double)nY;
  
  std::ofstream ofile;
  ofile.open(outputName+".dat");

  ofile << "nx> " << nX << "\n";
  ofile << "ny> " << nY << "\n";
  
  G4double totalN = (G4double)nX * (G4double)nY;

  G4int i = 0;
  for (G4double y = ymin; y < ymax; y += yStep)
    {
      for (G4double x = xmin; x < xmax; x += xStep)
	{
	  G4double percentage = ((G4double)i / totalN) *100;
	  if (std::remainder(percentage,1) == 0)
	    {std::cout << "\r" << percentage << "%";}
	  G4double result[6] = {0,0,0,0,0,0};
	  G4double coords[4] = {x,y,0,0};
	  field->GetFieldValue(coords, result);
	  ofile << x          << "\t"
		<< y          << "\t"
		<< result[0] / CLHEP::tesla << "\t"
		<< result[1] / CLHEP::tesla << "\t"
		<< result[2] / CLHEP::tesla << "\t"
		<< result[3] / CLHEP::volt  << "\t"
		<< result[4] / CLHEP::volt  << "\t"
		<< result[5] / CLHEP::volt  << "\n";
	  i++;
	}
    }
  ofile.close();
  std::cout << std::endl;
}
