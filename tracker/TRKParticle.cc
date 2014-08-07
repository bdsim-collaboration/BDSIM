#include "TRKParticle.hh"

TRKParticle::TRKParticle() : name(""), mass(0.0), charge(0.0)
{}

TRKParticle::TRKParticle(std::string nameIn, double massIn, double charge) : name(nameIn), mass(massIn), charge(chargeIn)
{}

TRKParticle::~TRKParticle() {};
