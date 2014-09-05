#include "TRKLine.hh"

#include <iostream>

TRKLine::TRKLine(std::string nameIn) :
  TRKTrackingElement(TRKTrackingElement::thin, TRK::DEFAULT_TRACKING_STEPS, nameIn, 0.0, 0.0, 0.0, NULL, NULL)
{
}

TRKLine::~TRKLine()
{
}

void TRKLine::Track(const double vIn[], double vOut[]) {
  double vTemp[6];
  /// vTemp = vIn;
  for (int i=0; i<6; i++) {
    vTemp[i]=vIn[i];
  }

  TRKLineIter elIter = elements.begin();
  for (;elIter!=elements.end(); ++elIter) {
    (*elIter)->Track(vTemp,vOut);
    /// vTemp = vOut;
    for (int i=0; i<6; i++) {
      vTemp[i]=vOut[i];
    }
  }
}

void TRKLine::Track(const double [], double [], double) {
  /// not sure how to do this
}

void TRKLine::AddElement(TRKTrackingElement* e) {
  elements.push_back(e);
}

TRKTrackingElement* TRKLine::FindElement(std::string eName)const {
 TRKLineIter elIter = elements.begin();
 TRKLineIter elIterEnd = elements.end();
  for (;elIter!=elIterEnd; ++elIter) {
    if ((*elIter)->GetName() == eName) {
      return (*elIter);
    }
  }
  /// if not found return NULL
  return NULL;
}

void TRKLine::ThinTrack(const double [], double [], double){std::cout << "should not be called" << std::endl;}
void TRKLine::HybridTrack(const double [], double [], double){std::cout << "should not be called" << std::endl;}
void TRKLine::ThickTrack(const double [], double [], double){std::cout << "should not be called" << std::endl;}

/// output stream
std::ostream& operator<< (std::ostream &out, const TRKLine &line) {
  TRKLine::TRKLineIter elIter = line.elements.begin();
  TRKLine::TRKLineIter elIterEnd = line.elements.end();
  for (;elIter!=elIterEnd; ++elIter) {
    out << **elIter << std::endl;
  }
  return out;
}
