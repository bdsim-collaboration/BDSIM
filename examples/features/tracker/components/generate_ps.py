import numpy as np

#  # while read p; do tracker --file=${p} --outfile="tracker-${p%.*}" --batch; done <files.txt
header = "x[m]:xp[rad]:y[m]:yp[rad]"

P = 6499999.9322804334
P0 = 6499999.9322804334

def main():
    xys = np.logspace(np.log10(1e-5), np.log10(0.015), num=10)
    xyps = np.logspace(np.log10(1e-6), np.log10(0.0007), num=10)

    with open("primaries.dat", "w") as f1, open("ptcstart.madx", "w") as f2:
        f1.write(f"# {header}\n")
        for xy, xyp in zip(xys, xyps):
            f1.write(f"{xy} {xyp} {xy} {xyp}\n")
            xyp *= P / P0
            f2.write(
                f"ptc_start, x={xy}, px={xyp}, y={xy}, py={xyp}, t=0, pt=0;\n"
            )


if __name__ == '__main__':
    main()
