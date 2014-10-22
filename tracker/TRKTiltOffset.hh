#ifndef TRKOffset_h
#define TRKOffset_h 1

/**
 * @brief class for Element offset and tilt
 */
class TRKTiltOffset {
public: 
  TRKTiltOffset(double x,double y, double phi, double theta, double psi);

private:
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
};

#endif
