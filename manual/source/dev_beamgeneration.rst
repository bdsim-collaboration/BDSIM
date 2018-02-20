.. _dev-beams:

Beam Generation
***************

BDSIM uses the CLHEP random number generator and associated classes to generate
psuedo-random particle distributions for tracking.


Gauss Twiss Covariance Matrix
=============================

The beam in an accelerator can be described as a 6D distribution of particles. The 6
dimensions are 3 canonical conjugate coordinate pairs in three planes and can be
represented by the vector - :math:`\boldsymbol{x} = (x,\,x^{\prime},\,y,y^{\prime},\,t,\,\delta)`.
The means of that distribution are the beam centroids and the 6x6 matrix of second
order moments of the dsitribution is referred to as sigma matrix and can be represented
compactly as :math:`\Sigma_{ij}=\langle\boldsymbol{x_{i}}\,\boldsymbol{x_{j}}\rangle`
with the angle brackets denoting a central moment.

The beam is typically parameterised in an accelerator using the Twiss functions
:math:`\beta,\alpha,\gamma` and dispersions :math:`\eta_x,\eta_x^{\prime},\eta_y,\eta_y^{\prime}`.
These are used in combination with beam parameters
:math:`\epsilon_x,\epsilon_y,\sigma_{\delta},\sigma_{t}`. The combination of
machine and beam parameters gives the physical observables such as beam size.

Please note that
:math:`\sigma_{\delta}` is used here for clarity, but its defintition is equivallent
to :math:`\sigma_{E}` defined in :ref:`model-description`. 

Having information about the machine optics and beam parameters at a given point
in the machine allows the generation of a pseudo-random distribution of particles
with appropriate coordinates at that point. In the Gauss Twiss matrix input the
set of parameters described above is used to generate the beam.

For linear optics, in the absence of dispersion or energy spread the 3 2x2 submatrices
along the diagonal of the sigma matrix fully represent each plane and can be treated
independently of each other, which greatly simplifies the calculation. The well-known
Courant-Snyder fomalisim is applied in this case. For example, the horisontal plane
is parametrised as,

.. math::
   \begin{pmatrix}
   \langle x^{2}\rangle & \langle x\,x^{\prime}\rangle \\
   \langle x\,x^{\prime}\rangle & \langle x^{\prime 2}\rangle
   \end{pmatrix}
   =
   \epsilon_{x}
   \begin{pmatrix}
   \beta_{x} & -\alpha_{x} \\
   -\alpha_{x} & \gamma_{x}
   \end{pmatrix}.

When dispersion and energy spread are present, numerous couplings are introduced
which appear as off-diagonal elements in the sigma matrix. In this case the energy
dependance of the coordinates results in both longitudinal-transverse and
transverse-transverse couplings. In order to sucessfully generate a beam at an
arbitrary :math:`S` in the machine, all those couplings need to be taken into
acccount. Simply ignoring them or approximating them does not work because, in
order to generate a real distribution, the sima matrix should be positive definite.
A positive definite sgima matrix fulfills the fundamental requirement that the the
beam distribution should have a non-negative 'size' is each dimension.

To arrive at expressions for these couplings in terms of machine and beam parameters
the starting point is the effect of dispersion on single coordinates. For example,
for an individual beam particle, the horizontal position is affected as follows,

.. math::
   x_{i} = x_{\beta i} + \eta_{s}\delta_{i},

where :math:`x_{i}` is the observed coodinate, :math:`x_{\beta}` is the betatron
component and :math:`\eta_{x}\delta_{i}` reflects the change due to the particle's
relative energy offset in a dispersive region.

Because :math:`x_{\beta}` and :math:`\delta_{i}` are random variables and are
independent by construction, the second order moment in :math:`x_{\beta}` can be
factorised using the properties of the variance,

.. math::
   \langle x^{2}\rangle = \mathrm{var}[x_{\beta}+\eta_{x}\delta] = \langle x_{\beta}^{2}\rangle + \eta_{x}^{2}\langle\delta_{i}^{2}\rangle = \epsilon_{x}\beta_{x} + \eta_{x}^{2}\sigma_{\delta}^{2},

where the last form is obtained by applying the Courant-Snyder equations and rewriting
the variance in terms of standard deviation.

The covariance of the horizontal position and the energy offset can also be obtained as,

.. math::
   \langle x\delta\rangle = \textrm{cov}[x_{\beta}+\eta_{x}\delta, \delta] = \langle x_{\beta}\delta\rangle + \eta_{x}\langle \delta^{2}\rangle = \eta_{x}\langle\delta^{2}\rangle,

as :math:`\langle x_{\beta}\delta\rangle = 0` by construction as explained above.

Finally, correlations that involve two different variables affected by dispersion
have the same form. For example,

.. math::
   \langle xy\rangle &= \mathrm{cov}[x_{\beta}+\eta_{x}\delta, xp_{\beta}+\eta_{xp}\delta] =\\
                     &= \langle x_{\beta}xp_{\beta}\rangle + \eta_{x}\langle xp_{\beta}\delta\rangle + \eta_{xp}\langle x_{\beta}\delta\rangle + \eta_{x}\eta_{xp}\langle\delta^{2}\rangle= \\
                     &= \langle x_{\beta}xp_{\beta}\rangle + \eta_{x}\eta_{xp}\langle\delta^{2}\rangle =\\
                     &=-\epsilon_{x}\alpha_{x} + \eta_{x}\eta_{xp}\sigma_{\delta}^{2}.

All other couplings are done by analogy.

Some useful relations:

.. math::
   &\langle XY\rangle\equiv\mathrm{cov}[X,Y], \\
   &\langle X^2\rangle\equiv\mathrm{cov}[X,X]\equiv\mathrm{var}[X]\equiv\sigma_{X}^{2}, \\
   &\mathrm{var}[X+Y]=\mathrm{var}[X]+\mathrm{var}[Y]\,\,\mathrm{if  X,Y independent}, \\
   &\mathrm{cov}[aX+bY,cW+dV]=ac\mathrm{cov}[X,W]+ad\mathrm{cov}[X,V]+bc\mathrm{cov}[Y,W]+bd\mathrm{cov}[Y,V].

