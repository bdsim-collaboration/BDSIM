#ifndef TRKLine_h
#define TRKLine_h

#include <vector>

#include "TRKElement.h"

class TRKLine : public TRKElement {
private: 
  std::vector<TRKTrackingInterface*> elements;

public:
  TRKLine();
  ~TRKLine();
  void AddElement(std::string eName, TRKElement *e);  
  TRKElement FindElement(std::string eName);
  
};

#endif
