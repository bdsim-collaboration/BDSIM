/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSBunchUserFile.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSParticleDefinition.hh"
#include "BDSUtilities.hh"

#include "parser/beam.h"

#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

#ifdef USE_GZSTREAM
#include "gzstream.h"
#endif

template <class T>
BDSBunchUserFile<T>::BDSBunchUserFile():
  nlinesIgnore(0)
{
  ffact = BDSGlobalConstants::Instance()->FFact();
}

template<class T>
BDSBunchUserFile<T>::~BDSBunchUserFile()
{
  CloseBunchFile();
}

template<class T>
void BDSBunchUserFile<T>::OpenBunchFile()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  InputBunchFile.open(distrFile);
  if(!InputBunchFile.good())
    { 
      G4cerr<<"Cannot open bunch file "<< distrFile <<G4endl; 
      exit(1); 
    }
}

template<class T>
void BDSBunchUserFile<T>::CloseBunchFile()
{
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  InputBunchFile.close();
}

template<class T>
void BDSBunchUserFile<T>::ParseFileFormat()
{
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
    G4cout<< __METHOD_NAME__ <<"token -> "<<token<<G4endl;
    G4cout<< __METHOD_NAME__ <<"token.substr(0,1) -> " << token.substr(0,1) << G4endl;
    G4cout<< __METHOD_NAME__ <<"unparsed_str -> "<<unparsed_str<<G4endl;
    G4cout<< __METHOD_NAME__ <<"pos -> "<<pos<<G4endl;
#endif
    if(token.substr(0,1)=="E" || token.substr(0,1)=="P") {
      G4String name,rest;
      if(token.substr(0,2)=="Ek") {//Kinetic energy (longer name).
	name = token.substr(0,2);
	rest = token.substr(2);
      } else {
	name = token.substr(0,1);
	rest = token.substr(1);
      }
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << name << "!"<<G4endl;
#endif
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name = name;
	sd.unit = ParseEnergyUnit(fmt);
	fields.push_back(sd);
      }
    } else if(token.substr(0,1)=="t") {
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"t!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name = "t"; 
        sd.unit = ParseTimeUnit(fmt);
	fields.push_back(sd);
      }
    } else if( ( token.substr(0,1)=="x" && token.substr(1,1)!="p" ) ||
	       ( token.substr(0,1)=="y" && token.substr(1,1)!="p" ) ||
	       ( token.substr(0,1)=="z" && token.substr(1,1)!="p" ) ) {
      G4String name = token.substr(0,1);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << name << "!"<<G4endl;
#endif
      G4String rest = token.substr(1);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 

	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name=name;
	sd.unit=ParseLengthUnit(fmt);	
	fields.push_back(sd);	
      }
    } else if ( (token.substr(0,2)=="xp") ||
		(token.substr(0,2)=="yp") ||
		(token.substr(0,2)=="zp") ) {
      G4String name = token.substr(0,2);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << name << "!"<<G4endl;
#endif
      G4String rest = token.substr(2);
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"rest ->"<<rest<<G4endl;
#endif
      G4int pos1 = rest.find("[");
      G4int pos2 = rest.find("]");
      if(pos1 < 0 || pos2 < 0) {
	G4cerr<<"unit format wrong!!!"<<G4endl;
      } else {
	G4String fmt = rest.substr(pos1+1,pos2-1);
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ <<"fmt ->"<<fmt<<G4endl;
#endif
	sd.name=name;
	sd.unit=ParseAngleUnit(fmt);
	fields.push_back(sd);
      }
    }else if(token.substr(0,2)=="pt") {
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"pt!"<<G4endl;
#endif
      sd.name="pt";
      sd.unit=1;
      fields.push_back(sd);
    } else if(token.substr(0,1)=="w") {
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<"weight!"<<G4endl;
#endif
      sd.name="weight";
      sd.unit=1;
      fields.push_back(sd);
    } else if (token.substr(0,1)=="-") {
      // skip
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ << "skip token " <<G4endl;
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

template<class T>
void BDSBunchUserFile<T>::skip(G4int nvalues){
  G4double dummy_val;
  for(G4int i=0;i<nvalues;i++) ReadValue(dummy_val);
}

template<class T>
void BDSBunchUserFile<T>::SetDistrFile(G4String filename)
{
  distrFile = BDS::GetFullPath(filename);
}

template<class T>
void BDSBunchUserFile<T>::SkipLines()
{
  //Skip the a number of lines defined by the user option.
  G4cout << "BDSBunchUserFile> skipping " << nlinesIgnore << " lines" << G4endl;
  skip((G4int)(nlinesIgnore * fields.size()));
}

template<class T>
void BDSBunchUserFile<T>::SetOptions(const GMAD::Beam& beam,
				     G4Transform3D beamlineTransformIn)
{
  BDSBunch::SetOptions(beam, beamlineTransformIn);
  SetDistrFile((G4String)beam.distrFile); 
  SetBunchFormat((G4String)beam.distrFileFormat);
  // Note this will be automatically advanced to the right nlinesIgnore
  // if we're recreating.
  SetNLinesIgnore(beam.nlinesIgnore);
  ParseFileFormat();
  OpenBunchFile(); 
  SkipLines();
  return; 
}

template<class T>
G4double BDSBunchUserFile<T>::ParseEnergyUnit(G4String &fmt)
{
  G4double unit=1.;
  if(fmt=="GeV") unit=1;
  else if(fmt=="MeV") unit=1.e-3;
  else if(fmt=="KeV") unit=1.e-6;
  else if(fmt=="eV") unit=1.e-9;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised energy unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}

template<class T>
G4double BDSBunchUserFile<T>::ParseLengthUnit(G4String &fmt)
{
  G4double unit=1.;
  if(fmt=="m") unit=1;
  else if(fmt=="cm") unit=1.e-2;
  else if(fmt=="mm") unit=1.e-3;
  else if(fmt=="mum" || fmt=="um") unit=1.e-6;
  else if(fmt=="nm") unit=1.e-9;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised length unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}

template<class T>
G4double BDSBunchUserFile<T>::ParseAngleUnit(G4String &fmt)
{
  G4double unit=1.;
  if(fmt=="rad") unit=1;
  else if(fmt=="mrad") unit=1.e-3;
  else if(fmt=="murad" || fmt=="urad") unit=1.e-6;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised angle unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}
template<class T>
G4double BDSBunchUserFile<T>::ParseTimeUnit(G4String &fmt)
{
  G4double unit=1.;
  if(fmt=="s") unit=1;
  else if(fmt=="ms") unit=1.e-3;
  else if(fmt=="mus" || fmt=="us") unit=1.e-6;
  else if(fmt=="ns") unit=1.e-9;
  else if(fmt=="mm/c") unit=(CLHEP::mm/CLHEP::c_light)/CLHEP::s;
  else if(fmt=="nm/c") unit=(CLHEP::nm/CLHEP::c_light)/CLHEP::s;
  else {
    G4cout << __METHOD_NAME__ << "Unrecognised time unit! " << fmt << G4endl;
    exit(1);
  }
  return unit;
}

template<class T>
void BDSBunchUserFile<T>::GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
		     G4double& xp, G4double& yp, G4double& zp,
		     G4double& t , G4double&  E, G4double& weight)
{

  E = x0 = y0 = z0 = xp = yp = zp = t = 0;
  weight = 1;
  
  bool zpdef = false; //keeps record whether zp has been read from file
  bool tdef = false; //keeps record whether t has been read from file
  
  G4int type;

  if (particleMass < 0)
    {
      auto particleDef = BDSGlobalConstants::Instance()->BeamParticleDefinition()->ParticleDefinition();
      particleMass = particleDef->GetPDGMass(); // should always exist at this point
    }
  
  for(auto it=fields.begin();it!=fields.end();it++)
    {
#ifdef BDSDEBUG 
      G4cout<< __METHOD_NAME__ <<it->name<<"  ->  "<<it->unit<<G4endl;
#endif
      if(it->name=="Ek") { 
	ReadValue(E); E *= ( CLHEP::GeV * it->unit ); 
#ifdef BDSDEBUG 
	G4cout << "******** Particle Kinetic Energy = " << E << G4endl;
#endif
	E += particleMass;
#ifdef BDSDEBUG 
	G4cout << "******** Particle Mass = " << particleMass << G4endl;
	G4cout << "******** Particle Total Energy = " << E << G4endl;
	G4cout<< __METHOD_NAME__ << E <<G4endl;
#endif
      }
      else if(it->name=="E") { 
	ReadValue(E); E *= ( CLHEP::GeV * it->unit ); 
#ifdef BDSDEBUG 

#endif
#ifdef BDSDEBUG 
	G4cout << "******** Particle Total Energy = " << E << G4endl;
	G4cout<< __METHOD_NAME__ << E <<G4endl;
#endif
      }
      else if(it->name=="P") { 
	G4double P=0;
	ReadValue(P); P *= ( CLHEP::GeV * it->unit ); //Paticle momentum
	G4double totalEnergy  = std::hypot(P,particleMass);
	E = totalEnergy - particleMass;
#ifdef BDSDEBUG 
	G4cout << "******** Particle Mass = " << particleMass << G4endl;
	G4cout << "******** Particle Total Energy = " << totalEnergy << G4endl;
	G4cout << "******** Particle Kinetic Energy = " << E << G4endl;
	G4cout<< __METHOD_NAME__ << E <<G4endl;
#endif
      }
      else if(it->name=="t") { ReadValue(t); t *= ( CLHEP::s * it->unit ); tdef = true; }
      else if(it->name=="x") { ReadValue(x0); x0 *= ( CLHEP::m * it->unit ); 
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ << x0 <<G4endl;
#endif
      }
      else if(it->name=="y") { 
	ReadValue(y0); y0 *= ( CLHEP::m * it->unit ); 
#ifdef BDSDEBUG 
	G4cout<< __METHOD_NAME__ << y0 <<G4endl;
#endif
      }
      else if(it->name=="z") { 
	ReadValue(z0); z0 *= ( CLHEP::m * it->unit ); 
#ifdef BDSDEBUG 

	G4cout<< __METHOD_NAME__ << z0 <<G4endl;
#endif
      }
      else if(it->name=="xp") { ReadValue(xp); xp *= ( CLHEP::radian * it->unit ); }
      else if(it->name=="yp") { ReadValue(yp); yp *= ( CLHEP::radian * it->unit ); }
      else if(it->name=="zp") { ReadValue(zp); zp *= ( CLHEP::radian * it->unit ); zpdef = true;}
      else if(it->name=="pt")
	{// particle type
	  // update base class flag - user file can specify different particles
	  if (!particleCanBeDifferent)
	    {particleCanBeDifferent = true;}
	  ReadValue(type);
	  G4String particleName = type;
	  if (InputBunchFile.good())
	    {
	      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	      G4ParticleDefinition* particleDef = particleTable->FindParticle(particleName);
	      if (!particleDef)
		{
		  G4cerr << "Particle \"" << particleName << "\"not found: quitting!" << G4endl;
		  exit(1);
		}
	      
	      // Wrap in our class that calculates momentum and kinetic energy.
	      // Requires that total energy 'E' already be set.
	      delete particleDefinition;
	      particleDefinition = new BDSParticleDefinition(particleDef, E, ffact); // update member
	    }
	}
      else if(it->name=="weight")
	{ReadValue(weight);}
      
      else if(it->name=="skip") {double dummy; ReadValue(dummy);}

      // If energy isn't specified, use the central beam energy (kinetic for Geant4)
      if (!BDS::IsFinite(E))
	{E = E0*CLHEP::GeV;}
      
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

  ApplyTransform(x0,y0,z0,xp,yp,zp);
}

template <class T>
template <typename Type>
G4bool BDSBunchUserFile<T>::ReadValue(Type &value)
{
  InputBunchFile>>value; 
  if (InputBunchFile.eof()){ //If the end of the file is reached go back to the beginning of the file.
    //this re reads the same file again to avoid crash - must always print warning
    G4cout << __METHOD_NAME__ << "End of file reached. Returning to beginning of file." << G4endl;
    CloseBunchFile();
    OpenBunchFile();
    InputBunchFile>>value; 
  } 
  return !InputBunchFile.eof();
}

template class BDSBunchUserFile<std::ifstream>;

#ifdef USE_GZSTREAM
template class BDSBunchUserFile<igzstream>;
#endif
