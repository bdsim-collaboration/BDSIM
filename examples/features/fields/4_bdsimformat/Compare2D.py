import pybdsim
import matplotlib.pyplot as plt
import numpy as np

def Compare2D(original, nearest, linear, cubic):
    a = pybdsim.Field.Load(original)
    n = pybdsim.Field.Load(nearest)
    l = pybdsim.Field.Load(linear)
    c = pybdsim.Field.Load(cubic)
    _Plot(a, 'original', 'Original')
    _Plot(n, 'nearest',  'Nearest')
    _Plot(l, 'linear',   'Linear')
    _Plot(c, 'cubic',    'Cubic')

def _Plot(a, filename, title):
    f   = plt.figure(figsize=(7.5,4))
    ax  = f.add_subplot(131)
    ax2 = f.add_subplot(132)
    ax3 = f.add_subplot(133)

    xmin = np.min(a[:,:,0])
    xmax = np.max(a[:,:,0])
    ymin = np.min(a[:,:,1])
    ymax = np.max(a[:,:,1])
    ax.imshow(a[:,:,2], interpolation='None',extent=(xmin,xmax,ymin,ymax), vmin=-4, vmax=4, origin='lower')
    ax.set_xlabel('X (cm)')
    ax.set_ylabel('Y (cm)')
    ax.set_title('X-Component',size='medium')
    
    im = ax2.imshow(a[:,:,3], interpolation='None',extent=(xmin,xmax,ymin,ymax), vmin=-4, vmax=4, origin='lower')
    ax2.set_xlabel('X (cm)')
    ax2.set_title('Y-Component',size='medium')
    ax2.get_yaxis().set_ticks([])

    im = ax3.imshow(a[:,:,4], interpolation='None',extent=(xmin,xmax,ymin,ymax), vmin=-4, vmax=4, origin='lower')
    ax3.set_xlabel('X (cm)')
    ax3.set_title('Z-Component',size='medium')
    ax3.get_yaxis().set_ticks([])

    f.subplots_adjust(left=0.09, right=0.85, top=0.86, bottom=0.12, wspace=0.02)
    cbar_ax = f.add_axes([0.88, 0.15, 0.05, 0.7])
    f.colorbar(im, cax=cbar_ax)

    plt.suptitle(title,size='x-large')

    #pdf seems to force interpolation
    plt.savefig(filename+'.png',dpi=400)
    
    plt.draw()
