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
  /// tilt, alpha (rotation around z-axis) in [urad]
  double phi;
  /// beta (rotation around x'-axis)
  double theta;
  /// gamma (rotation around z''-axis)
  double psi;
  ///@}
};

#endif
