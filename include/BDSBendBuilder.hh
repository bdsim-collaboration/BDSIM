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
 * @brief A class for building rbends and sbends from individual BDSMagnet* components.
 *
 * This calculates and constructs a BDSLine* of BDSMagnet*.  This class inherits
 * BDSComponent factory so it can use many of the 
 *
 * For SBends, the bend is split into multiple wedges. If a small or zero poleface
 * angle is specified, the end wedges will have faces angled as appropriate, the
 * remaining wedges will re-use a single identical 'central' wedge several times.
 * For a stronger angled poleface, the faces of each wedge fade in/out from the
 * poleface to the cental wedge in the middle. Thin fringefield elements are placed
 * at the beginning and end of the beamline if required.
 *
 * For rbends, a line is returned with a single magnet as the main dipole, but can have
 * fringefield magnets placed either end if specified.
 *
 * @author William Shields
 */

class BDSBendBuilder : public BDSComponentFactory
{
public:

    virtual ~BDSBendBuilder();

    /// Construct beamline for an rbend
    BDSLine* RBendLine(GMAD::Element* element,
                       GMAD::Element* prevElement,
                       GMAD::Element* nextElement,
                       G4double angelIn,
                       G4double angleOut,
                       G4double rho,
                       BDSMagnetStrength* st);

    /// Construct beamline for an rbend
    BDSLine* SBendLine(GMAD::Element*  element,
                             G4double angleIn,
                             G4double angleOut,
                             BDSMagnetStrength* st);

    static BDSBendBuilder* Instance();

protected:
    void Build();

    /// Thin magnet for dipole fringe field.
    /// Is beampipe only, no outer magnet.
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

    /// Function to return a new magnet wedge for use in an sbend.
    /// The faces of each wedge are calculated as appropriate depending
    /// on the poleface angle(s).
    BDSMagnet* NewSbendWedge(GMAD::Element* element,
                               G4bool fadeIn,
                               G4bool fadeOut,
                               G4int index,
                               G4int nSBends,
                               BDSMagnetStrength* st);
};
#endif
