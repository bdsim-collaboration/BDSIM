#ifndef TRKLine_h
#define TRKLine_h

#include <vector>

#include "TRKTrackingElement.hh"

class TRKLine : public TRKTrackingElement {
private: 
  std::vector<TRKTrackingElement*> elements;

public:
  TRKLine();
  ~TRKLine();
  void AddElement(std::string eName, TRKElement *e);  
  TRKElement* FindElement(std::string eName);
  
};

#endif
