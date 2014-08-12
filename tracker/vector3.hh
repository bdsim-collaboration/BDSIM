class vector3 { 
 public: 
  vector3(); 
  vector3(double xIn, double yIn, double zIn) { 
    double x;
    double y;
    double z; 
  }
  ~vector3();
  double& X() {return x;}  
  double& Y() {return y;}
  double& Z() {return z;} 

  // copy constructor 
  // operator+ 
  // operator- 
  // operator*
  // operator/
  // unit vector 
  
 private: 
    double x;
    double y;
    double z;   
};
