#include "BDSTrackingFIFO.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include "BDSDump.hh"
#include "BDSExecOptions.hh"

#include "G4StackManager.hh"
#include "G4EventManager.hh"

BDSTrackingFIFO::BDSTrackingFIFO(){
  _fifo=NULL;
  _filename=BDSGlobalConstants::Instance()->GetFifo();
  _tmpT=0.0;
}

BDSTrackingFIFO::~BDSTrackingFIFO(){
}

void BDSTrackingFIFO::doFifo(){
  if(BDSDump::nUsedDumps < BDSDump::GetNumberOfDumps()){
    //If there is no fifo filename specified, do nothing
    G4cout << __METHOD_NAME__ << " _filename = " << _filename << G4endl;
    if(_filename.length()==0){
      G4cout << "FIFO filename length is 0, not doing FIFO." << G4endl;
      return;
    }
    if(BDSGlobalConstants::Instance()->getWaitingForDump()) // synchronization with placet
      {
	G4cout<< __METHOD_NAME__ <<G4endl;
	writeToFifo();
	readFromFifo();
      }else{
      G4Exception("Read from fifo failed: bad file name\n", "-1", FatalException, "");
      exit(1);
    }
    BDSDump::nUsedDumps++;
  }
}

void BDSTrackingFIFO::writeToFifo(){
  initWrite();
  fprintf(_fifo,"# nparticles = %i\n",(int)BDSGlobalConstants::Instance()->transformedQueue.size());
#ifdef DEBUG
  G4cout << "reftime = " << _tmpT << G4endl;
#endif
  std::deque<tmpParticle>::iterator iter;
  for(iter=BDSGlobalConstants::Instance()->transformedQueue.begin();
      iter!=BDSGlobalConstants::Instance()->transformedQueue.end();iter++)
    {
      (*iter).x -= ((*iter).t-_tmpT)*(*iter).xp*c_light/micrometer;
      (*iter).y -= ((*iter).t-_tmpT)*(*iter).yp*c_light/micrometer;
      (*iter).z -= ((*iter).t-_tmpT)*c_light/micrometer;
      fprintf(_fifo,"%.15f %.15f %.15f %.15f %.15f %.15f %.15f\n",
	      (*iter).E,
	      (*iter).x,
	      (*iter).y,
	      (*iter).z,
	      (*iter).xp,
	      (*iter).yp,
	      (*iter).t);
    }
  finishWrite();
}

//Initialize for writing to the FIFO
void BDSTrackingFIFO::initWrite(){
  G4StackManager* SM = G4EventManager::GetEventManager()->GetStackManager();
  BDSGlobalConstants::Instance()->setWaitingForDump(false);
  BDSGlobalConstants::Instance()->setDumping(true);
  BDSGlobalConstants::Instance()->outputQueue.clear();
  BDSGlobalConstants::Instance()->transformedQueue.clear();
  SM->TransferStackedTracks(fPostpone, fUrgent);// so that they can be reclassified
  SM->ReClassify();
  _tmpT = 0;
  G4double* referenceTimes = BDSGlobalConstants::Instance()->referenceQueue.front();
  for(int i=0;i<BDSExecOptions::Instance()->GetNPTwiss();++i){
    _tmpT += referenceTimes[i];
  }
  _tmpT /= BDSExecOptions::Instance()->GetNPTwiss();
  openForWriting();
}

void BDSTrackingFIFO::finishWrite(){
  close();
  BDSGlobalConstants::Instance()->setDumping(false);
}

void BDSTrackingFIFO::readFromFifo(){
  initRead();
  // read in the stuff from placet
  int token;
  G4double x,y,z,t,xp,yp,zp,E;
  x = y = z = xp = yp = zp = t = E = 0;
  G4AffineTransform tf = BDSGlobalConstants::Instance()->GetDumpTransform();
  G4ThreeVector pos;
  G4ThreeVector momDir;
  G4ThreeVector LocalPosition;
  G4ThreeVector LocalDirection;	
  if(_fifo != NULL){
    fscanf(_fifo,"# nparticles = %i",&token);
    G4cout << "# nparticles read from fifo = " << token << G4endl;
    for(int i=0; i< token;i++){
      fscanf(_fifo,"%lf %lf %lf %lf %lf %lf %lf",
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
      G4double refTime = (_tmpT-t); // all t0=0 so remove /2
      
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
  }
  finishRead();
}

//initialize for reading to the FIFO
void BDSTrackingFIFO::initRead(){
  BDSGlobalConstants::Instance()->setReading(true);
  BDSGlobalConstants::Instance()->holdingQueue.clear();
  openForReading();
}

void BDSTrackingFIFO::finishRead(){
  close();
  BDSGlobalConstants::Instance()->setReading(false);
  BDSGlobalConstants::Instance()->setReadFromStack(false);
  //	BDSGlobalConstants::Instance()->referenceQueue.pop_front();
  delete[] BDSGlobalConstants::Instance()->referenceQueue.front();
  BDSGlobalConstants::Instance()->referenceQueue.pop_front();
#ifdef DEBUG 
  G4cout << "Number read in = " << BDSGlobalConstants::Instance()->holdingQueue.size() << G4endl;
#endif
}

void BDSTrackingFIFO::openForWriting(){
  open("w");
}

void BDSTrackingFIFO::openForReading(){
  open("r");
}

void BDSTrackingFIFO::open(const char* mode){
  _fifo = fopen(_filename.c_str(),mode);
  if(!_fifo){
    std::string errorMessage = __METHOD_NAME__ + " unable to open file: " + _filename + " in mode " + mode;
    G4Exception(errorMessage.c_str(), "-1", FatalException, "");
  }
}

void BDSTrackingFIFO::close(){
  if(_fifo) fclose(_fifo);
  _fifo=NULL;
}
