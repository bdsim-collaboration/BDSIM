#ifndef TRKTrackingInterface_h
#define TRKTrackingInterface_h

class TRKTrackingInterface {
public : 
  enum type {thin, thick, hybrid}; 
  TRKTrackingInterface();
  ~TRKTrackingInterface(); 
  virtual void Track() = 0;
  
private: 
  // type of tracking 

};


#endif
