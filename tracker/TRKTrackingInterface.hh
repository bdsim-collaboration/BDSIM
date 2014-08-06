#ifndef TRKTrackingInterface_h
#define TRKTrackingInterface_h

class TRKTrackingInterface {
public : 
  TRKTrackingInterface();
  ~TRKTrackingInterface(); 
  virtual void Track() = 0;
  
private: 
  // type of tracking 

};


#endif
