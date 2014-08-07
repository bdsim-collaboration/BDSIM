import numpy as np
import matplotlib.pyplot as plt


def Load(filename):
    data = []
    keys = []
    with open(filename,'r') as f:
        for i,line in enumerate(f):
            if i == 0:
                keys.extend(list(line.strip().split()))
            else:
                data.append(list(map(float,line.strip().split())))
    data = np.array(data)
    return keys,data

def Plot(data,keys):
    plt.figure()
    s      = keys.index('s')
    beta_x = keys.index('beta_x')
    beta_y = keys.index('beta_y')
    max_bx = np.max(data[:,beta_x])
    max_by = np.max(data[:,beta_y])
    max_y  = np.max([max_bx,max_by])

    plt.plot(data[:,s],data[:,beta_x],label=r'$\beta_{x}$')
    plt.plot(data[:,s],data[:,beta_y],label=r'$\beta_{y}$')
    
    plt.xlim(np.min(data[:,s]),np.max(data[:,s]))
    plt.ylim(0,max_y*1.05)

    plt.legend()

