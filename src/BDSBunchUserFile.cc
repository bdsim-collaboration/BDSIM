#include "BDSBunchUserFile.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "BDSDebug.hh"

BDSBunchUserFile::BDSBunchUserFile(struct Options &opt){
  SetOptions(opt);
  ParseFileFormat();
  OpenBunchFile(); //
  SkipLines();
}

BDSBunchUserFile::~BDSBunchUserFile(){
  CloseBunchFile();
}

void BDSBunchUserFile::OpenBunchFile(){
  G4cout << __METHOD_NAME__ << " - opening " << distribFile << G4endl;
  InputBunchFile.open(distribFile);
  if(!InputBunchFile.good()){ 
    G4cerr<<"Cannot open bunch file "<< distribFile <<G4endl; 
    exit(1); 
  } 
}

void BDSBunchUserFile::SkipLines(){
  //Skip the a number of lines defined by the user option.
  G4cout << __METHOD_NAME__ << " - skipping " << nlinesIgnore << " lines" << G4endl;
  skip((G4int)(nlinesIgnore * fields.size()));
}

void BDSBunchUserFile::CloseBunchFile(){
  InputBunchFile.close();
}


void BDSBunchUserFile::ParseFileFormat(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4String unparsed_str = bunchFormat;
  G4int pos = unparsed_str.find(":");
  
  struct BDSBunchUserFile::Doublet sd;
  
  while(pos > 0){
    pos = unparsed_str.find(":");
    G4String token = unparsed_str.substr(0,pos);
    
    
    unparsed_str = unparsed_str.substr(pos+1);
#ifdef BDSDEBUG 
    G4cout<< "BDSBunch : " <<"token -> "<<token<<G4endl;
    G4cout<< "BDSBunch : token.substr(0,1) -> " << token.substr(0,1) << G4endl;
    G4cout<< "BDSBunch : " <<"unparsed_str -> "<<unparsed_str<<G4endl;
    G4cout<< "BDSBunch : " <<"pos -> "<<pos<<G4endl;
#endif
    if(token.substr(0,1)=="E") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"E!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	      sd.name = "E"; 
	      
	      if(fmt=="GeV") sd.unit=1;
	      if(fmt=="MeV") sd.unit=1.e-3;
	      if(fmt=="KeV") sd.unit=1.e-6;
	      if(fmt=="eV") sd.unit=1.e-9;
	      
	      fields.push_back(sd);
      }
    } else if(token.substr(0,1)=="P") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"P!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name = "P"; 
        
	if(fmt=="GeV") sd.unit=1;
	if(fmt=="MeV") sd.unit=1.e-3;
	if(fmt=="KeV") sd.unit=1.e-6;
	if(fmt=="eV") sd.unit=1.e-9;
	
	fields.push_back(sd);
      }
    } else if(token.substr(0,1)=="t") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"t!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name = "t"; 
        
	if(fmt=="s") sd.unit=1;
	if(fmt=="ms") sd.unit=1.e-3;
	if(fmt=="mus") sd.unit=1.e-6;
	if(fmt=="ns") sd.unit=1.e-9;
	if(fmt=="mm/c") sd.unit=(CLHEP::mm/CLHEP::c_light)/CLHEP::s;
	if(fmt=="nm/c") sd.unit=(CLHEP::nm/CLHEP::c_light)/CLHEP::s;
        
	fields.push_back(sd);
	
      }
    } else if( (token.substr(0,1)=="x") && (token.substr(1,1)!="p") ) {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"x!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name="x";
	
	if(fmt=="m") sd.unit=1;
	if(fmt=="cm") sd.unit=1.e-2;
	if(fmt=="mm") sd.unit=1.e-3;
	if(fmt=="mum") sd.unit=1.e-6;
	if(fmt=="nm") sd.unit=1.e-9;
	
	fields.push_back(sd);
	
      }
    }else if(token.substr(0,1)=="y" && token.substr(1,1)!="p" ) {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"y!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name="y";
	
	if(fmt=="m") sd.unit=1;
	if(fmt=="cm") sd.unit=1.e-2;
	if(fmt=="mm") sd.unit=1.e-3;
	if(fmt=="mum") sd.unit=1.e-6;
	if(fmt=="nm") sd.unit=1.e-9;
	
	fields.push_back(sd);
      }
    }else if(token.substr(0,1)=="z" && token.substr(1,1)!="p" ) {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"z!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name="z";
        
	if(fmt=="m") sd.unit=1;
	if(fmt=="cm") sd.unit=1.e-2;
	if(fmt=="mm") sd.unit=1.e-3;
	if(fmt=="mum") sd.unit=1.e-6;
	if(fmt=="nm") sd.unit=1.e-9;
	
	fields.push_back(sd);
      }
    } else if(token.substr(0,2)=="xp") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"xp!"<<G4endl;
#endif
      G4String rest = token.substr(2);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name="xp";
        
	if(fmt=="rad") sd.unit=1;
	if(fmt=="mrad") sd.unit=1.e-3;
	if(fmt=="murad") sd.unit=1.e-6;
	
	fields.push_back(sd);
	
      }
    }else if(token.substr(0,2)=="yp") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"yp!"<<G4endl;
#endif
      G4String rest = token.substr(2);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name="yp";
	
	if(fmt=="rad") sd.unit=1;
	if(fmt=="mrad") sd.unit=1.e-3;
	if(fmt=="murad") sd.unit=1.e-6;
	
	fields.push_back(sd);
      }
    } else if(token.substr(0,2)=="zp") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"zp!"<<G4endl;
#endif
      G4String rest = token.substr(2);
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name="zp";
	
	if(fmt=="rad") sd.unit=1;
	if(fmt=="mrad") sd.unit=1.e-3;
	if(fmt=="murad") sd.unit=1.e-3;
	
	fields.push_back(sd);
      }
    }else if(token.substr(0,2)=="pt") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"pt!"<<G4endl;
#endif
      sd.name="pt";
      sd.unit=1;
      fields.push_back(sd);
    } else if(token.substr(0,1)=="w") {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<"weight!"<<G4endl;
#endif
      sd.name="weight";
      sd.unit=1;
      fields.push_back(sd);
    } else if (token.substr(0,1)=="-") {
      // skip
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : skip token " <<G4endl;
#endif
      sd.name="skip";
      sd.unit=1;
      fields.push_back(sd);
    } else {
      G4cerr << "Cannot determine bunch data format" << G4endl; exit(1);
    }
  } 
  return;
}

void BDSBunchUserFile::skip(G4int nvalues){
  G4double dummy_val;
  for(G4int i=0;i<nvalues;i++) ReadValue(dummy_val);
}

void BDSBunchUserFile::SetOptions(struct Options &opt) {
  BDSBunchInterface::SetOptions(opt);
  SetDistribFile((G4String)opt.distribFile); 
  SetBunchFormat((G4String)opt.distribFileFormat); 
  SetNLinesIgnore(opt.nlinesIgnore);
  return; 
}

void BDSBunchUserFile::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		     G4double& xp, G4double& yp, G4double& zp,
		     G4double& t , G4double&  E, G4double& weight) {

  E = x0 = y0 = z0 = xp = yp = zp = t = 0;
  weight = 1;
  
  bool zpdef = false; //keeps record whether zp has been read from file
  bool tdef = false; //keeps record whether t has been read from file
  
  G4int type;
  
  std::list<struct BDSBunchUserFile::Doublet>::iterator it;
  for(it=fields.begin();it!=fields.end();it++)
    {
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " <<it->name<<"  ->  "<<it->unit<<G4endl;
#endif
      if(it->name=="E") { 
	ReadValue(E); E *= ( CLHEP::GeV * it->unit ); 
#ifdef BDSDEBUG 
	G4cout << "******** Particle Mass = " << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass() << G4endl;
	G4cout << "******** Particle Total Energy = " << E << G4endl;
#endif
	E-=BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
#ifdef BDSDEBUG 
	G4cout << "******** Particle Kinetic Energy = " << E << G4endl;
	G4cout<< "BDSBunch : " << E <<G4endl;
#endif
      }
      else if(it->name=="P") { 
	G4double P=0;
	ReadValue(P); P *= ( CLHEP::GeV * it->unit ); //Paticle momentum
	G4double particleMass = BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
	G4double totalEnergy = sqrt(P*P + particleMass*particleMass);
	E = totalEnergy - particleMass;
#ifdef BDSDEBUG 
	G4cout << "******** Particle Mass = " << particleMass << G4endl;
	G4cout << "******** Particle Total Energy = " << totalEnergy << G4endl;
	G4cout << "******** Particle Kinetic Energy = " << E << G4endl;
	G4cout<< "BDSBunch : " << E <<G4endl;
#endif
      }
      else if(it->name=="t") { ReadValue(t); t *= ( CLHEP::s * it->unit ); tdef = true; }
      else if(it->name=="x") { ReadValue(x0); x0 *= ( CLHEP::m * it->unit ); 
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " << x0 <<G4endl;
#endif
      }
      else if(it->name=="y") { 
	ReadValue(y0); y0 *= ( CLHEP::m * it->unit ); 
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " << y0 <<G4endl;
#endif
      }
      else if(it->name=="z") { 
	ReadValue(z0); z0 *= ( CLHEP::m * it->unit ); 
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " << z0 <<G4endl;
#endif
      }
      else if(it->name=="xp") { ReadValue(xp); xp *= ( CLHEP::radian * it->unit ); 
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " << xp <<G4endl;
#endif
      }
      else if(it->name=="yp") { ReadValue(yp); yp *= ( CLHEP::radian * it->unit ); 
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " << yp <<G4endl;
#endif
      }
      else if(it->name=="zp") { ReadValue(zp); zp *= ( CLHEP::radian * it->unit ); zpdef = true;
#ifdef BDSDEBUG 
      G4cout<< "BDSBunch : " << zp <<G4endl;
#endif
      }
      else if(it->name=="pt") {
	ReadValue(type);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : particle type = " << type <<G4endl;
#endif
	BDSGlobalConstants::Instance()->SetParticleName(G4ParticleTable::GetParticleTable()->FindParticle(type)->GetParticleName());
	BDSGlobalConstants::Instance()->SetParticleDefinition(G4ParticleTable::
							      GetParticleTable()
							      ->FindParticle(type));
	if(!BDSGlobalConstants::Instance()->GetParticleDefinition()) 
	  {
	    G4Exception("Particle not found, quitting!", "-1", FatalErrorInArgument, "");
	    exit(1);
	  }
      }
      else if(it->name=="weight"){ ReadValue(weight);
#ifdef BDSDEBUG 
	G4cout<< "BDSBunch : " << weight <<G4endl;
#endif
      }
      else if(it->name=="skip") {double dummy; ReadValue(dummy);}
      
      // compute zp from xp and yp if it hasn't been read from file
      if (!zpdef) zp = CalculateZp(xp,yp,1);
      // compute t from z0 if it hasn't been read from file
      if (!tdef) t=0; 
      // use the Kinetic energy:
      //          if(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGEncoding() != 22){
      //}
    }
  //Add the global offset Z
  z0=z0+Z0*CLHEP::m;
}

template <typename Type> G4bool  BDSBunchUserFile::ReadValue(Type &value){
  InputBunchFile>>value; 
  if (InputBunchFile.eof()){ //If the end of the file is reached go back to the beginning of the file.
    //this re reads the same file again to avoid crash - must always print warning
    G4cout << "BDSBunchUserFile.cc> End of file reached. Returning to beginning of file." << G4endl;
    CloseBunchFile();
    OpenBunchFile();
    InputBunchFile>>value; 
  } 
  return !InputBunchFile.eof();
}
