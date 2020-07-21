#ifndef MAPS_H
#define MAPS_H

#include "TRKParticle.hh"

namespace trk {
namespace maps {

void drift(TRKParticle &, double length) noexcept;
void nldrift(TRKParticle &, double length) noexcept;
void drift(TRKBunch &, double length) noexcept;
void nldrift(TRKBunch &, double length) noexcept;
void sbend(TRKBunch &, double length, double angle, double k1 = 0.0) noexcept;
void sbend(TRKParticle &, double length, double angle, double k1 = 0.0) noexcept;
void dipole_fringe(TRKParticle &, double k0, double angle) noexcept;
void dipole_fringe(TRKBunch &, double k0, double angle) noexcept;
void quadrupole(TRKParticle &, double length, double k1) noexcept;
void quadrupole(TRKBunch &, double length, double k1) noexcept;
void sextupole(TRKParticle &, double length, double k1) noexcept;
void kicker(TRKParticle &, double hkick, double vkick) noexcept;

} // namespace maps

} // namespace trk
#endif /* MAPS_H */
