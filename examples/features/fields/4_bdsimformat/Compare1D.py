import pybdsim
import matplotlib.pyplot as plt

def Compare1D(original, nearest, linear, cubic):
    a = pybdsim.Field.Load(original)
    n = pybdsim.Field.Load(nearest)
    l = pybdsim.Field.Load(linear)
    c = pybdsim.Field.Load(cubic)

    plt.figure()
    plt.plot(a[:,0], a[:,1], 'bo', label='Original', mec='b', alpha=0.8)
    plt.plot(n[:,0], n[:,1], 'b:', label='Nearest')
    plt.plot(l[:,0], l[:,1], 'r--',label='Linear')
    plt.plot(c[:,0], c[:,1], '-',  label='Cubic', color='grey')
    plt.xlabel('X (cm)')
    plt.ylabel('Field (T)')
    plt.title('X-Component of B Field')
    plt.legend(numpoints=1)
    plt.xlim(-65,65)
    plt.ylim(-3.5,4.5)
    plt.tight_layout()
    plt.savefig('1d_interpolated_x.pdf')

    plt.figure()
    plt.plot(a[:,0], a[:,2], 'bo', label='Original', mec='b', alpha=0.8)
    plt.plot(n[:,0], n[:,2], 'b:', label='Nearest')
    plt.plot(l[:,0], l[:,2], 'r--',label='Linear')
    plt.plot(c[:,0], c[:,2], '-',  label='Cubic', color='grey')
    plt.xlabel('Y (cm)')
    plt.ylabel('Field (T)')
    plt.title('Y-Component of B Field')
    plt.legend(numpoints=1)
    plt.xlim(-65,65)
    plt.ylim(-2.5,2.5)
    plt.tight_layout()
    plt.savefig('1d_interpolated_y.pdf')

    plt.draw()
