/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

//==========================================================
//==========================================================

#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 

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

#if defined(__unix__) || defined(__linux__) || defined(__APPLE__)
#include <unistd.h> // for sleep, will not work on windows?
#endif

typedef std::list<BDSAcceleratorComponent*>  myBeamline;
extern myBeamline theBeamline;

//==========================================================

BDSRunAction::BDSRunAction()
{
  nptwiss = BDSExecOptions::Instance()->GetNPTwiss();
}

//==========================================================

BDSRunAction::~BDSRunAction()
{}

//==========================================================

void BDSRunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "BDSRunAction::BeginOfRunAction> Run number " << aRun->GetRunID() << " start." << G4endl;
  

  //  if (G4VVisManager::GetConcreteInstance())
  //    {
      //      G4UImanager* UI = G4UImanager::GetUIpointer(); 
      //  UI->ApplyCommand("/vis/scene/notifyHandlers");
  //    } 


}

//==========================================================

void BDSRunAction::EndOfRunAction(const G4Run* aRun)
{

  if(BDSGlobalConstants::Instance()->getWaitingForDump()) // synchronization with placet
    {
      G4cout<<"BDSRunAction::EnfOfRunAction> last event reached! dumping"<<G4endl;
      
      G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
      
      BDSGlobalConstants::Instance()->setWaitingForDump(false);
      BDSGlobalConstants::Instance()->setDumping(true);
      BDSGlobalConstants::Instance()->outputQueue.clear();
      BDSGlobalConstants::Instance()->transformedQueue.clear();
      SM->TransferStackedTracks(fPostpone, fUrgent);// so that they can be reclassified
      SM->ReClassify();

      G4double tmpT = 0;

      G4double* referenceTimes = BDSGlobalConstants::Instance()->referenceQueue.front();
      for(int i=0;i<nptwiss;++i){
	tmpT += referenceTimes[i];
      }
      tmpT /= nptwiss;

      FILE* fifo = fopen(BDSGlobalConstants::Instance()->GetFifo(),"w");
      fprintf(fifo,"# nparticles = %i\n",(int)BDSGlobalConstants::Instance()->transformedQueue.size());


#ifdef DEBUG
      G4cout << "reftime = " << tmpT << G4endl;
#endif
      std::deque<tmpParticle>::iterator iter;
      for(iter=BDSGlobalConstants::Instance()->transformedQueue.begin();
	  iter!=BDSGlobalConstants::Instance()->transformedQueue.end();iter++)
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
      BDSGlobalConstants::Instance()->setDumping(false);
      BDSGlobalConstants::Instance()->setReading(true);
      
      // read in the stuff from placet
      
      int token;
      G4double x,y,z,t,xp,yp,zp,E;
      x = y = z = xp = yp = zp = t = E = 0;
      BDSGlobalConstants::Instance()->holdingQueue.clear();
      
      G4AffineTransform tf = BDSGlobalConstants::Instance()->GetDumpTransform();
      G4ThreeVector pos;
      G4ThreeVector momDir;
      G4ThreeVector LocalPosition;
      G4ThreeVector LocalDirection;
      
      fifo = fopen(BDSGlobalConstants::Instance()->GetFifo(),"r");

      if(fifo != NULL){
        fscanf(fifo,"# nparticles = %i",&token);
	G4cout << "# nparticles read from fifo = " << token << G4endl;
	for(int i=0; i< token;i++){
	  fscanf(fifo,"%lf %lf %lf %lf %lf %lf %lf",
		&E,&x,&y,&z,&xp,&yp,&t);
#ifdef DEBUG
          printf("In : %.15f %.15f %.15f %.15f %.15f %.15f %.15f\n",E,x,y,z,xp,yp,t);
#endif
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

#ifdef DEBUG
          G4cout << "Stacking: Pos = " << pos << G4endl;
          G4cout << "LocalPos: Pos = " << LocalPosition << G4endl;
          G4cout << "Stacking: mom = " << momDir << G4endl;
          G4cout << "LocalDir: mom = " << LocalDirection << G4endl;
#endif
	  tmpParticle holdingParticle;
	  holdingParticle.E = E*GeV - BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass();
	  holdingParticle.t = t;
	  holdingParticle.xp = LocalDirection.x();
	  holdingParticle.yp = LocalDirection.y();
	  holdingParticle.zp = LocalDirection.z();
	  
	  holdingParticle.x = LocalPosition.x();
	  holdingParticle.y = LocalPosition.y();
	  holdingParticle.z = LocalPosition.z();
	  
	  BDSGlobalConstants::Instance()->holdingQueue.push_back(holdingParticle);
#ifdef DEBUG 
          G4cout << "Read particle number " << i << G4endl;
#endif
        }
        sleep(1);
	fclose(fifo);
        BDSGlobalConstants::Instance()->setReading(false);
        BDSGlobalConstants::Instance()->setReadFromStack(false);
//	BDSGlobalConstants::Instance()->referenceQueue.pop_front();
	delete[] BDSGlobalConstants::Instance()->referenceQueue.front();
	BDSGlobalConstants::Instance()->referenceQueue.pop_front();

#ifdef DEBUG 
        G4cout << "Number read in = " << BDSGlobalConstants::Instance()->holdingQueue.size() << G4endl;
#endif
      }
      else{
	G4Exception("Read from fifo failed: bad file name\n", "-1", FatalException, "");
	exit(1);
      }
      BDSDump::nUsedDumps++;
    }
  G4cout << "### Run " << aRun->GetRunID() << " end." << G4endl;
}
//==========================================================
