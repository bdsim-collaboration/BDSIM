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
  typedef std::vector<TRKTrackingElement*>::const_iterator TRKLineIter;

public:
  TRKLine(std::string name);
  ~TRKLine();
  /// Append TRKElement to Line
  void AddElement(TRKTrackingElement *e);
  /// Find first element with name eName
  TRKTrackingElement* FindElement(std::string eName)const;
  /// Track all elements in line
  virtual void Track(const double vIn[], double vOut[]);
  virtual void Track(const double vIn[], double vOut[], double h);

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKLine &element);

protected:
  virtual void ThinTrack(const double vIn[], double vOut[], double h);
  virtual void HybridTrack(const double vIn[], double vOut[], double h);
  virtual void ThickTrack(const double vIn[], double vOut[], double h);

private:
  TRKLine(); ///< not implemented
};

#endif
