//
// Created by strangesyd on 3/18/21.
//

#ifndef BDSIM_BDSDICOMPHANTOMPARAMETERISATIONCOLOUR_HH
#define BDSIM_BDSDICOMPHANTOMPARAMETERISATIONCOLOUR_HH

#include <map>

#include "G4PhantomParameterisation.hh"
class G4VisAttributes;

class BDSDicomPhantomParameterisationColour : public G4PhantomParameterisation
{
public:
    typedef std::map<G4String,G4VisAttributes*> ColourMap_t;

    static G4String defaultColorFile;

public:  // with description
    BDSDicomPhantomParameterisationColour(G4String colorFile =
    defaultColorFile);
    ~BDSDicomPhantomParameterisationColour();

    virtual G4Material* ComputeMaterial(const G4int repNo,
                                        G4VPhysicalVolume *currentVol,
                                        const G4VTouchable *parentTouch=0);

    const ColourMap_t& GetColourMap() const { return fColours; }
    ColourMap_t& GetColourMap() { return fColours; }

private:
    void ReadColourData(G4String colourFile);

private:
    ColourMap_t fColours;
    std::map<G4int, G4VisAttributes*> mColours;
};

#endif //BDSIM_BDSDICOMPHANTOMPARAMETERISATIONCOLOUR_HH
