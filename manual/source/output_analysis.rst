.. _output-analysis-section:

===============
Output Analysis
===============

ROOT Output
-----------

Example: Plot Optical Functions with pybdsim
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
::

   import pybdsim
   import pymadx
   import robdsim

   # load ROOT output file into robdsim
   r=robdsim.robdsimOutput("output.root")
   # calculate optics and write to output file "optics.dat"
   r.CalculateOpticalFunctions("optics.dat")
   # load optics file into pybdsim
   a=pybdsim.Data.Load("optics.dat")

   f = figure()
   # Plot sqrt Beta_x
   plot(a.S(),sqrt(a.Beta_x()),'.-',label='BDSIM')

   # compare with MadX file:
   b = pymadx.Tfs("madx.tfs")
   s = b.GetColumn("S")
   betx = b.GetColumn("BETX")
   plot(s,sqrt(betx),'.-',label='MadX')

   # labels and legend:
   xlabel('$s$ [m]')
   ylabel('$\sqrt{\\beta_x}$ [m]')
   legend(loc=0)

   # add machine lattice to figure (optional):
   pybdsim.Plot.AddMachineLatticeToFigure(f,b)
   
   # save figure
   f.savefig("sqrtbetax.png")

ASCII Output
------------
