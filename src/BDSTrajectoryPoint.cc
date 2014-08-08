#include "BDSTrajectoryPoint.hh"
#include "G4AttDef.hh"
#include <map>
#include <iterator>

BDSTrajectoryPoint::BDSTrajectoryPoint():G4RichTrajectoryPoint(){
}

BDSTrajectoryPoint::~BDSTrajectoryPoint(){
}

void BDSTrajectoryPoint::printRichData(){
  G4cout << "" << G4endl;
  G4cout << "BDSTrajectoryPoint - Att Defs: " << G4endl;
  G4cout << "MapString :: Name :: Description :: Category :: Extra :: ValueType :: Value" << G4endl;
  const std::map<G4String, G4AttDef>*  m = GetAttDefs();
  for(std::map<G4String, G4AttDef>::const_iterator it = m->begin(); it != m->end(); it++) {
    G4cout << it->first << " || " 
	   << it->second.GetName() << " || "
	   << it->second.GetDesc() << " || "	   
	   << it->second.GetCategory() << " || "
	   << it->second.GetExtra() << " || "
	   << it->second.GetValueType()  
	   << G4endl;
  }
}
