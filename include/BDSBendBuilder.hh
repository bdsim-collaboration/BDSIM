#ifndef BDSBENDBUILDER_H
#define BDSBENDBUILDER_H

#include "globals.hh" // geant4 globals / types
#include "parser/element.h"
#include "BDSComponentFactory.hh"
#include "BDSMagnet.hh"
#include "BDSElement.hh"
#include "BDSLine.hh"
#include "BDSMagnetStrength.hh"

#include <iterator>
#include <ostream>
#include <utility>    //for std::pair
#include <vector>


/**
 * @brief A vector of BDSBeamlineElement instances - a beamline.
 * 
 * This will calculate and construct a beamline as BDSAcceleratorComponents
 * are added in sequence - ie it calculates their placement positions and 
 * rotations with respect to the start of the beamline as well as their s
 * position in curvilinear coordinates.
 * 
 * Note, this is not a singleton as geometry hierarchy can be introduced
 * by placing beamline components inside parent volumes and therefore creating
 * a new beamline of parents. It can also be used to create multiple beam lines.
 * 
 * @author Laurie Nevay
 */

class BDSBendBuilder : public BDSComponentFactory
{
public:

    virtual ~BDSBendBuilder();

    BDSLine* RBendLine(GMAD::Element* element,
                       GMAD::Element* prevElement,
                       GMAD::Element* nextElement,
                       G4double angelIn,
                       G4double angleOut,
                       G4double rho,
                       BDSMagnetStrength* st);

    BDSLine* SBendLine(GMAD::Element*  element,
                             G4double angleIn,
                             G4double angleOut,
                             BDSMagnetStrength* st);

    static BDSBendBuilder* Instance();

protected:
    void Build();

    BDSMagnet* DipoleFringe(GMAD::Element *element,
                                  G4double angleIn,
                                  G4double angleOut,
                                  G4String name,
                                  BDSMagnetType magType,
                                  BDSMagnetStrength *st);

private:
    BDSBendBuilder();

    static BDSBendBuilder* _instance;

    G4double outerDiameter;

    /// Utility function to calculate the number of segments an sbend should be split into.
    /// Based on aperture error tolerance - default is 1mm.
    G4int CalculateNSBendSegments(GMAD::Element const* element,
                                  const G4double aperturePrecision = 1.0);

    BDSMagnet* NewSbendWedge(GMAD::Element* element,
                               G4bool fadeIn,
                               G4bool fadeOut,
                               G4int index,
                               G4int nSBends,
                               BDSMagnetStrength* st);
};
#endif
