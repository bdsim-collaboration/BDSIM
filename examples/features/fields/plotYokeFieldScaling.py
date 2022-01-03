import pybdsim

names = ['out_sfo_bend',
         'out_sfo_quad',
         'out_sfo_bend_lhc',
         'out_sfo_quad_lhc']

titles = ["Dipole",
          "Quadrupole",
          "LHC Dipole",
          "LHC Quadrupole"]


for name,title in zip(names, titles):
    dn = pybdsim.Field.Load(name+"_normal.dat")
    ds = pybdsim.Field.Load(name+"_scaled.dat")
    pybdsim.Field.Plot2DXY(dn, title=title+" Normal")
    pybdsim.Field.Plot2DXY(ds, title=title+" Scaled")
         
