#ifndef _BDSLogicalVolumeInfo_hh
#define _BDSLogicalVolumeInfo_hh 1

#include "G4String.hh"
#include "globals.hh" //G4 basic types

class BDSLogicalVolumeInfo
{
public:
  BDSLogicalVolumeInfo(G4double SPosIn);
  BDSLogicalVolumeInfo(G4String NameIn, G4double SPosIn);
  ~BDSLogicalVolumeInfo();

  /*
  void     SetName(G4String NameIn);
  void     SetSPos(G4double SPosIn);
  G4double GetSPos();
  */

  G4String GetName()
  {return name;}
  
  void SetName(G4String NameIn)
  {name = NameIn;}
  
  void SetSPos(G4double SPosIn)
  {spos = SPosIn;}

  G4double GetSPos()
  {return spos;}

private:
  BDSLogicalVolumeInfo();

  G4String name;
  G4double spos;

};


#endif
