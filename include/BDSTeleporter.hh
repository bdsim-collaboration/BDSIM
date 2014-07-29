#ifndef BDSTeleporter_h
#define BDSTeleporter_h 1

#include "BDSTeleporterStepper.hh"
//#include "globals.hh"
//#include "BDSGlobalConstants.hh"
#include "parser/elementlist.h"
#include "BDSDrift.hh"
#include "BDSBeamline.hh"

class BDSTeleporter: public BDSDrift
{
public:
  BDSTeleporter(G4String name,
		  G4double length,
		  G4double apertureX,
		  G4double apertureY,
		  G4double phiAngleIn,
		G4double phiAngleOut);
    /*BDSDrift(name,
	       length,
	       std::list<G4double>(), //blmLocZ,
	       std::list<G4double>(), //blmLocTheta,
	       apertureX,
	       apertureY,
	       "",     //Tunnel Material
	       true,  //Aperture set
	       0.0,    //Similar to drift in componentfactory
	       0.0,    //Tunnel Offset X
	       phiAngleIn,
	       phiAngleOut)*/
  
  ~BDSTeleporter(){};

  //protected:
private:
  virtual void BuildBpFieldAndStepper();
};

void CalculateAndSetTeleporterDelta(BDSBeamline* thebeamline);
void AddTeleporterToEndOfBeamline(ElementList* beamline_list);


#endif
