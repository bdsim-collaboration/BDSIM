#ifndef TRKLine_h
#define TRKLine_h

#include <vector>

#include "TRKTrackingElement.hh"

/**
 * @brief line tracking
 */
class TRKLine : public TRKTrackingElement {
private: 
  // in case we use FindElement a lot, probably good to make an index also (but something for later)
  std::vector<TRKTrackingElement*> elements;
  typedef std::vector<TRKTrackingElement*>::iterator TRKLineIter;

public:
  TRKLine(std::string name);
  ~TRKLine();
  /// Append TRKElement to Line
  void AddElement(TRKElement *e);
  /// Find first element with name eName
  TRKElement* FindElement(std::string eName)const;
  /// Track all elements in line
  virtual void Track(const double vIn[], double vOut[]);
  virtual void Track(const double vIn[], double vOut[], double h);

private:
  TRKLine(); ///< not implemented
};

#endif
