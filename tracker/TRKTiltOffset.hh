#ifndef TRKOffset_h
#define TRKOffset_h 1

/**
 * @brief class for Element offset and tilt
 */
class TRKTiltOffset {
public:
  // make sure TRKTiltOffset is always created on the heap
  /// constructor for offsets and angles
  static TRKTiltOffset* Create(double x,double y, double phi, double theta, double psi);
  /// constructor for offset
  static TRKTiltOffset* TRKOffset(double x,double y);
  /// constructor for tilts
  static TRKTiltOffset* TRKTilt(double phi, double theta, double psi);

private:
  /// constructor for offset and angles
  TRKTiltOffset(double x,double y, double phi, double theta, double psi);
  /// horizontal offset in [m]
  double offsetX;
  /// vertical offset in [m]
  double offsetY;
  /// Euler angles in [urad]
  ///@{
  /// tilt, phi/alpha (rotation around z-axis) in [urad]
  double phi;
  /// theta/beta (rotation around x'-axis)
  double theta;
  /// psi/gamma (rotation around z''-axis)
  double psi;
  ///@}

public:
  double GetOffsetX() const {return offsetX;}
  double GetOffsetY() const {return offsetY;}
  double GetPhi()     const {return phi;}
  double GetTheta()   const {return theta;}
  double GetPsi()     const {return psi;}

  void   AddOffset(double x, double y);
  void   AddTilt(double phi, double theta, double psi);
  void   SetOffset(double x, double y);
  void   SetTilt(double phi, double theta, double psi);
};

#endif
