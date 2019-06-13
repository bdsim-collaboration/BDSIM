//#ifdef USE_HEPMC

#ifndef BDSIM_BDSHEPEVTINTERFACE_H
#define BDSIM_BDSHEPEVTINTERFACE_H

#include <fstream>
#include <vector>

#include "globals.hh"
#include "G4VPrimaryGenerator.hh"
#include "G4HEPEvtParticle.hh"

class G4PrimaryVertex;
class G4Event;

class G4HEPEvtInterface:public G4VPrimaryGenerator
{
  public: // with description
    G4HEPEvtInterface(const char* filename, G4int vl=0);
//    G4HEPEvtInterface(G4String evfile);
    // Constructors, "evfile" is the file name (with directory path).

  public:
    ~G4HEPEvtInterface();

    void GeneratePrimaryVertex(G4Event* evt);

  private:
    G4int vLevel;
    G4String filename;
    std::ifstream inputFile;
    std::vector<G4HEPEvtParticle*> HPlist;
};

#endif

//#endif