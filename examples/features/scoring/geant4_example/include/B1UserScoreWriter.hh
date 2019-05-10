#ifndef B1UserScoreWriter_h
#define B1UserScoreWriter_h 1

#include "globals.hh"
#include "G4VScoreWriter.hh"

// class description:
//
//  This class represents storing the scored quantity into a file.
//

class B1UserScoreWriter : public G4VScoreWriter {

public:
    B1UserScoreWriter();
   ~B1UserScoreWriter() override;

public:
  // store a quantity into a file
  void DumpQuantityToFile(const G4String & psName,
          const G4String & fileName,
          const G4String & option) override;

};

#endif
