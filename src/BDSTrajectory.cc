#include "BDSTrajectory.hh"
#include "BDSTrajectoryPoint.hh"
#include "G4AttDef.hh"
#include <map>
#include <iterator>

BDSTrajectory::BDSTrajectory(const G4Track* aTrack):G4RichTrajectory(aTrack){
}

BDSTrajectory::~BDSTrajectory(){
}

void BDSTrajectory::printRichData(){
  G4cout << "" << G4endl;
  G4cout << "Att Defs: " << G4endl;
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

void BDSTrajectory::printRichDataOfSteps(){
  BDSTrajectoryPoint* tj;
  for(int i = 0; i<GetPointEntries(); i++){
    tj = (BDSTrajectoryPoint*)GetPoint(i);
    G4cout << "BDSTrajectory: Rich data for trajectory point : " << i << G4endl;
    tj->printRichData();
  }
}
