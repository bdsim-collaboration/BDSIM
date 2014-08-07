#ifndef TRKTrackingInterface_h
#define TRKTrackingInterface_h

class TRKTrackingInterface {
public : 
  enum type {thin, thick, hybrid}; 
  TRKTrackingInterface();
  virtual ~TRKTrackingInterface(); 
  virtual void Track(const double vIn[], double vOut[], double h) = 0;
  virtual void Track(const double vIn[], double vOut[]) = 0;
  
private: 
  // type of tracking 

};


#endif
