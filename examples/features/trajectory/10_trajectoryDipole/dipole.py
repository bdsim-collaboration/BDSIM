import pybdsim
import matplotlib.pyplot as _plt
import numpy as _np

# dipole.dipole(["./model_dipole.root","./model_dipole_step0.50.root","./model_dipole_step0.25.root","./model_dipole_step0.10.root"],0.75,6,[0.02708269529],["none","0.50","0.25","0.10"])

# dipole.dipole(["./model_dipole_nosplit.root","./model_dipole_nosplit_step1.00.root","./model_dipole_nosplit_step0.50.root","./model_dipole_nosplit_step0.25.root","./model_dipole_nosplit_step0.10.root"],0.75,6,[0.02708269529],["no-split","1.00","0.50","0.25","0.10"])

# dipole.dipole(["./model_dipole.root","./model_dipole_halfangle.root","./model_dipole_doubleangle.root","./model_dipole_quarterangle.root"],0.75,6,[0.02708269529,0.02708269529/2,0.02708269529*2,0.02708269529/4.0],["none","half","double","quarter"])

def dipole(rootFile, d1, length, angle,labels) : 

    td = []

    for file in rootFile : 
        print file
        d = pybdsim.Data.Load(file)
        td.append(pybdsim.Data.TrajectoryData(d,0))
        


    x = td[0].trajectories[0]['x']      
    z = td[0].trajectories[0]['z']  

    r = length/angle[0]
    zc = d1
    xc = -r
    xn = xc+_np.sqrt(r**2-(z-zc)**2)
    xd = x - xn 

    _plt.figure(1)
    _plt.clf()
    _plt.plot(x,z)
    _plt.plot(xn,z,"+")
    _plt.xlabel("$x$")
    _plt.ylabel("$z$")

    _plt.figure(2)
    _plt.clf()

    for i in range(0,len(td),1) : 
        r = length/angle[i]
        zc = d1
        xc = -r
        x = td[i].trajectories[0]['x']      
        z = td[i].trajectories[0]['z']  
    
        xn = xc+_np.sqrt(r**2-(z-zc)**2)
        xd = x - xn 
        _plt.plot(z,xd,".--",label=labels[i])
    
    _plt.xlim(d1-0.01,d1+length+0.01)
    _plt.ylim(-0.75e-9,0.75e-9)
    _plt.xlabel("$z$")
    _plt.ylabel("$\delta x$")
    _plt.legend()
    _plt.grid(True)
