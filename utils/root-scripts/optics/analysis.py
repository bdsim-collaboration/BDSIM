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

    plt.plot(data[:,s],abs(data[:,beta_x]),label=r'$\beta_{x}$')
    plt.plot(data[:,s],abs(data[:,beta_y]),label=r'$\beta_{y}$')
    
    plt.xlim(0,np.max(data[:,s]))
    plt.ylim(0,2000)

    plt.legend()

