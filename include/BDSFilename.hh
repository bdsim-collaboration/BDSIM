#ifndef __BDS_FILENAME_HH_
#define __BDS_FILENAME_HH_

#include "G4String.hh"

class BDSFilename {
public:
  BDSFilename();
  ~BDSFilename();
  BDSFilename(G4String fileName);
  G4String fileName();
  G4String fullPath();
  void fileName(G4String);
  
private:
  G4String _fileName;
};

#endif
