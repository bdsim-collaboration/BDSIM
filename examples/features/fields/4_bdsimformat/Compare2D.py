import pybdsim
import matplotlib.pyplot as plt
import numpy as np

def Compare2D(original):#, nearest, linear, cubic):
    a = pybdsim.Field.Load(original)
    #n = pybdsim.Field.Load(nearest)
    #l = pybdsim.Field.Load(linear)
    #c = pybdsim.Field.Load(cubic)
    _Plot(a, 'original', 'Original')

def _Plot(a, filename, title):
    f   = plt.figure(figsize=(7.5,4))
    ax  = f.add_subplot(121)
    ax2 = f.add_subplot(122)


    xmin = np.min(a[:,:,0])
    xmax = np.max(a[:,:,0])
    ymin = np.min(a[:,:,1])
    ymax = np.max(a[:,:,1])
    ax.imshow(a[:,:,2], interpolation='None',extent=(xmin,xmax,ymin,ymax), vmin=-4, vmax=4)
    ax.set_xlabel('X (cm)')
    ax.set_ylabel('Y (cm)')
    ax.set_title('X-Component',size='medium')
    
    im = ax2.imshow(a[:,:,3], interpolation='None',extent=(xmin,xmax,ymin,ymax), vmin=-4, vmax=4)
    ax2.set_xlabel('X (cm)')
    ax2.set_title('Y-Component',size='medium')

    f.subplots_adjust(left=0.09, right=0.85, top=0.86, bottom=0.12, wspace=0.2)
    cbar_ax = f.add_axes([0.88, 0.15, 0.05, 0.7])
    f.colorbar(im, cax=cbar_ax)

    plt.suptitle(title,size='x-large')

    #pdf seems to force interpolation
    plt.savefig(filename+'.png',dpi=400)
    
    plt.draw()
