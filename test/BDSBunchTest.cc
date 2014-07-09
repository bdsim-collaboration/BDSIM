#include "parser/options.h"
#include "BDSBunch.hh"

extern Options options;

int main(void) {
  BDSBunch bdsBunch;
  bdsBunch.SetOptions(options); 
}
