import pybdsim

def Compare2D(original, nearest, linear, linearmag, cubic):
    kws = {'vmin':-4, 'vmax':4}
    pybdsim.Field.Plot2DXYFxFyFz(original, 'Original', **kws)
    pybdsim.Field.Plot2DXYFxFyFz(nearest, 'Nearest', **kws)
    pybdsim.Field.Plot2DXYFxFyFz(linear, 'Linear', **kws)
    pybdsim.Field.Plot2DXYFxFyFz(linearmag, 'Linear Mag', **kws)
    pybdsim.Field.Plot2DXYFxFyFz(cubic, 'Cubic', **kws)
    return

if __name__ == "__main__":
    Compare2D("2dexample.dat.gz",
              "2d_interpolated_nearest.dat",
              "2d_interpolated_linear.dat",
              "2d_interpolated_linear_mag.dat",
              "2d_interpolated_cubic.dat")
