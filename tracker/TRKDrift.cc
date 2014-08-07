#include "TRKDrift.hh"

TRKDrift::TRKDrift() {}
TRKDrift::TRKDrift(std::string nameIn, double lengthIn, double size_xIn, double size_yIn, 
		   TRKApertureInterface *apertureIn, TRKPlacement *placementIn) :
  TRKElement(nameIn,lengthIn, size_xIn, size_yIn, apertureIn, placementIn)
{
  
}

TRKDrift::~TRKDrift() {}

void TRKDrift::Track(const double vIn[], double vOut[], double h) {   
} 

void TRKDrift::Track(const double vIn[], double vOut[]) { 
}
