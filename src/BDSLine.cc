#include "BDSLine.hh"


BDSLine::BDSLine(G4String aName, std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta): 
  BDSAcceleratorComponent (aName, 
			   0.0,
			   0.0,
			   0.0,
			   0.0,
			   blmLocZ, 
			   blmLocTheta)
{
  itsType="line";
}

void BDSLine::addComponent(BDSAcceleratorComponent* component) {
  line.push_back(component);
}
