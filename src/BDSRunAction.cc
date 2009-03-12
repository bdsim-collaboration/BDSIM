/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

//==========================================================
//==========================================================

#include "BDSGlobalConstants.hh" // must be first in include list

#include "BDSRunAction.hh"
#include "BDSRunManager.hh"
#include "BDSDump.hh"

#include "G4Run.hh"
#include "G4StateManager.hh"
#include "G4UImanager.hh"
#include "G4VVisManager.hh"
#include "G4ios.hh"

#include "BDSAcceleratorComponent.hh"
#include <list>
#include <fstream>

#include <string>

typedef list<BDSAcceleratorComponent*>  myBeamline;
extern myBeamline theBeamline;
extern G4int nptwiss;

const int DEBUG  = 0;
const int DEBUG2 = 0;



//==========================================================

BDSRunAction::BDSRunAction()
{
}

//==========================================================

BDSRunAction::~BDSRunAction()
{}

//==========================================================

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  //  BDSGlobals->GetTimer()->Start();
 
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;

 //  if(BDSGlobals->GetUseTimer())
//     {
//       //BDSGlobals->GetTimer()->Start();
//     }

  if (G4VVisManager::GetConcreteInstance())
    {
      G4UImanager* UI = G4UImanager::GetUIpointer(); 
      UI->ApplyCommand("/vis/scene/notifyHandlers");
    } 


}

//==========================================================

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{

  //  if (G4VVisManager::GetConcreteInstance()) {
  //   G4UImanager::GetUIpointer()->ApplyCommand("/vis/viewer/update");

 //  if(BDSGlobals->GetUseTimer())
//     {
//       BDSGlobals->GetTimer()->Stop();
//       G4cout<<"RUN Elapsed time="<<*BDSGlobals->GetTimer()<<G4endl;
//     }


  /*  
  if(BDSGlobals->GetSynchRadOn())
    {
    ofstream synch_output("synch_factors_new.dat");
      myBeamline::const_iterator iBeam;
      for(iBeam=theBeamline.begin();iBeam!=theBeamline.end();iBeam++)
	{
	  synch_output<<(*iBeam)->GetSynchEnergyLoss()/ 
	    G4double(aRun->GetNumberOfEvent())/GeV<<G4endl;
	}
	}
  */
  if(BDSGlobals->getWaitingForDump()) // synchronization with placet
    {
      //G4cout<<"end of event : "<<event_number<<G4endl;
      G4cout<<"last event reached! dumping"<<G4endl;
      
      G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
      
      BDSGlobals->setWaitingForDump(false);
      BDSGlobals->setDumping(true);
      /*      
      BDSGlobals->fileDump.open(BDSGlobals->GetFifo()); //SPM
      if(!BDSGlobals->fileDump.good()){
	G4Exception("BDSGlobals->GetFifo(): fifo not found. Quitting.");
	exit(1);
      }
      BDSGlobals->outputQueue.clear();
      BDSGlobals->fileDump << "# nparticles = " << SM->GetNPostponedTrack() << "\n";
      */
      BDSGlobals->outputQueue.clear();
      BDSGlobals->transformedQueue.clear();
      SM->TransferStackedTracks(fPostpone, fUrgent);// so that they can be reclassified
      SM->ReClassify();
      //BDSGlobals->fileDump.close(); // SPM

      G4double tmpT = 0;
      /*
	G4int nPrimaries = 0;
	for(iter=BDSGlobals->transformedQueue.begin();
	iter!=BDSGlobals->transformedQueue.end();iter++)
	if((*iter).parentID == 0){ // only interested in primary particles
	tmpT += (*iter).t;
	++nPrimaries;
	}
	
	if(nPrimaries)
	tmpT /= nPrimaries; // average event time elapsed
      */

      G4double* referenceTimes = BDSGlobals->referenceQueue.front();
      for(int i=0;i<nptwiss;++i){
	tmpT += referenceTimes[i];
	//G4cout << referenceTimes[i] << G4endl;
      }
      tmpT /= nptwiss;

      FILE* fifo = fopen(BDSGlobals->GetFifo(),"w");
      fprintf(fifo,"# nparticles = %i\n",(int)BDSGlobals->transformedQueue.size());

      //change particle z from absolute to relative
      //      tmpZ = BDSGlobals->referenceQueue.front()*c_light;
      if(DEBUG) G4cout << "reftime = " << tmpT << G4endl;
      std::deque<tmpParticle>::iterator iter;
      for(iter=BDSGlobals->transformedQueue.begin();
	  iter!=BDSGlobals->transformedQueue.end();iter++)
	{
	  (*iter).x -= ((*iter).t-tmpT)*(*iter).xp*c_light/micrometer;
	  (*iter).y -= ((*iter).t-tmpT)*(*iter).yp*c_light/micrometer;
	  (*iter).z -= ((*iter).t-tmpT)*c_light/micrometer;
	  fprintf(fifo,"%.15f %.15f %.15f %.15f %.15f %.15f %.15f\n",
		  (*iter).E,
		  (*iter).x,
		  (*iter).y,
		  (*iter).z,
		  (*iter).xp,
		  (*iter).yp,
		  (*iter).t);
	}
      fclose(fifo);
      BDSGlobals->setDumping(false);
      BDSGlobals->setReading(true);
      
      // read in the stuff from placet
      
      int token;
      G4double x,y,z,t,xp,yp,zp,E;
      x = y = z = xp = yp = zp = t = E = 0;
      BDSGlobals->holdingQueue.clear();
      
      G4AffineTransform tf = BDSGlobals->GetDumpTransform();
      G4ThreeVector pos;
      G4ThreeVector momDir;
      G4ThreeVector LocalPosition;
      G4ThreeVector LocalDirection;
      
      fifo = fopen(BDSGlobals->GetFifo(),"r");

      if(fifo != NULL){
        fscanf(fifo,"# nparticles = %i",&token);
	G4cout << "# nparticles read from fifo = " << token << G4endl;
	for(int i=0; i< token;i++){
	  fscanf(fifo,"%lf %lf %lf %lf %lf %lf %lf",
		&E,&x,&y,&z,&xp,&yp,&t);
	  if(DEBUG2) printf("In : %.15f %.15f %.15f %.15f %.15f %.15f %.15f\n",E,x,y,z,xp,yp,t);
	  
	  xp *= 1e-6*radian;
	  yp *= 1e-6*radian;
	  zp = sqrt(1-xp*xp-yp*yp)*radian;

	  pos = G4ThreeVector(x,y,z)*micrometer;
	  momDir = G4ThreeVector(xp,yp,zp);
	  
	  LocalPosition = tf.TransformPoint(pos);
	  LocalDirection = tf.TransformAxis(momDir);
	  G4double refTime = (tmpT-t); // all t0=0 so remove /2

	  LocalPosition -= LocalDirection*c_light*refTime;
///	  t = -z/c_light;
	  LocalPosition += LocalDirection.unit()*1e-4*micrometer; // temp fix for recirculation in dump volume

	  if(DEBUG){	  	  
            G4cout << "Stacking: Pos = " << pos << G4endl;
            G4cout << "LocalPos: Pos = " << LocalPosition << G4endl;
            G4cout << "Stacking: mom = " << momDir << G4endl;
            G4cout << "LocalDir: mom = " << LocalDirection << G4endl;
	  }
	  tmpParticle holdingParticle;
	  holdingParticle.E = E*GeV - BDSGlobals->GetParticleDefinition()->GetPDGMass();
	  holdingParticle.t = t;
	  holdingParticle.xp = LocalDirection.x();
	  holdingParticle.yp = LocalDirection.y();
	  holdingParticle.zp = LocalDirection.z();
	  
	  holdingParticle.x = LocalPosition.x();
	  holdingParticle.y = LocalPosition.y();
	  holdingParticle.z = LocalPosition.z();
	  
	  BDSGlobals->holdingQueue.push_back(holdingParticle);
	  if(DEBUG) G4cout << "Read particle number " << i << G4endl;
        }
        sleep(1);
	fclose(fifo);
        BDSGlobals->setReading(false);
        BDSGlobals->setReadFromStack(false);
//	BDSGlobals->referenceQueue.pop_front();
	delete[] BDSGlobals->referenceQueue.front();
	BDSGlobals->referenceQueue.pop_front();

	if(DEBUG) G4cout << "Number read in = " << BDSGlobals->holdingQueue.size() << G4endl;
      }
      else{
	G4Exception("Read from fifo failed: bad file name\n");
	exit(1);
      }
      BDSDump::nUsedDumps++;
    }
}
//==========================================================
