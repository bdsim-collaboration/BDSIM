#include "TRKLine.hh"

TRKLine::TRKLine(std::string nameIn) :
  TRKTrackingElement(TRKTrackingElement::thin, TRK::DEFAULT_TRACKING_STEPS, nameIn, 0.0, 0.0, 0.0, NULL, NULL)
{
}

TRKLine::~TRKLine()
{
}

void Track(const double vIn[], double vOut[]) {
  double vTemp[6];
  /// vTemp = vIn;
  for (int i=0; i<6; i++) {
    vTemp[i]=vIn[i];
  }

  TRKLineIter el_iter = elements.begin();
  for (;el_start<elements.end(); ++el_start) {
    (*el_iter)->Track(vTemp,vOut);
    /// vTemp = vOut;
    for (int i=0; i<6; i++) {
      vTemp[i]=vOut[i];
    }
  }
}

void Track(const double vIn[], double vOut[], double h) {
  /// not sure how to do this
}

void TRKLine::AddElement(TRKElement* e) {
  elements.push_back(e);
}

TRKElement* TRKLine::FindElement(std::string eName)const {
 TRKLineIter elIter = elements.begin();
 TRKLineIter elIterEnd = elements.end();
  for (;el_start<elIterEnd(); ++el_start) {
    if (*(elIter)->name == eName) {
      return (*elIter);
    }
  }
  /// if not found return NULL
  return NULL;
}
