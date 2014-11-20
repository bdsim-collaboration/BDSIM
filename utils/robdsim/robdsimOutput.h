#ifndef __RoBdsimOutput_h

class robdsimOutput {

 public : 
  robdsimOutput();
  robdsimOutput(char *path);   
  ~robdsimOutput(); 

  void Chain();
  
  void ElossLoop(); 
  void PlossLoop();
  void PrecisionElossLoop();

 private: 
  char *path; 

    
};

#endif
