.. _dev-tracking:

Tracking Algorithms
*******************

Dipoles
=======

RMatrix - from Particle Accelerator Physics (3rd Edition) by Wiedemann, chapter 5.
The z terms are not calculated via the matrix method, rather the z position
is simply the addition of the step length, and the  z momentum is calculated
from the x and y momentum to ensure momentum conservation.
Note that this matrix is incomplete, there are terms for the calculation of the
l parameter which are not needed in this stepper.

.. math::

   \begin{pmatrix}
   x_1    \\
   x'_1   \\
   y_1    \\
   y'_1   \\
   l_1    \\
   \delta \\
   \end{pmatrix} =

    \begin{pmatrix}
    \cos{\Theta}            & \frac{\sin{\Theta}}{\sqrt{K}} & 0                     & 0                              & 0 & \frac{1 - \cos{\Theta}}{\sqrt{K}}  \\
    -\sqrt{K}\sin{\Theta}   & \cos{\Theta}                  & 0                     & 0                              & 0 & \sin{\Theta}                       \\
    0                       & 0                             & \cosh{\Theta}         & \frac{\sinh{\Theta}}{\sqrt{K}} & 0 & 0                                  \\
    0                       & 0                             & \sqrt{K}\sinh{\Theta} & \cosh{\Theta}                  & 0 & 0                                  \\
    0                       & 0                             & 0                     & 0                              & 1 & 0                                  \\
    0                       & 0                             & 0                     & 0                              & 0 & 1                                  \\
   \end{pmatrix}
   \begin{pmatrix}
   x_0    \\
   x'_0   \\
   y_0    \\
   y'_0   \\
   l_0    \\
   \delta \\
   \end{pmatrix}

