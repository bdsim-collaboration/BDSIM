#ifndef MAPS_H
#define MAPS_H

#include "TRKParticle.hh"

namespace trk {
namespace maps {

void drift(TRKParticle &, double length) noexcept;
void drift(TRKBunch &, double length) noexcept;

// void quadrupole(particle, length, k1l);
// void drift(particle, length, k1l);
// void drift(particle, length, k1l);
// void sbend(particle, length, angle);

} // namespace maps

} // namespace trk
#endif /* MAPS_H */
