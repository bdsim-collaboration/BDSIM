import sys,os

def main(f1,f2,precision=1e-6):
    print "Precision required:",precision
    max_diff=0.0
    stream1=file(f1,'r')
    stream2=file(f2,'r')
    i=0
    for l1,l2 in zip(stream1,stream2):
        if l1.strip()[0]=='#':
            continue
        i+=1
        for a,b in zip(l1.split(),l2.split()):
            if (type(a) is str) or (type(b) is str):
                continue
            diff=abs(float(a)-float(b))
            if diff>precision:
                raise ValueError("Found error in line %i for files %s, %s" % (i,f1,f2))
            elif diff>max_diff:
              max_diff=diff
    return max_diff

if __name__=="__main__":
    if len(sys.argv)==4:
        diff=main(sys.argv[1],sys.argv[2],float(sys.argv[3]))
    elif len(sys.argv)==3:
        diff=main(sys.argv[1],sys.argv[2])
    else:
        raise TypeError("Wrong input...")

    print "*"*62
    print "*\t\tFiles are (sufficiently) equal!\t\t\t*"
    print "*\tMaximum difference between two values was %.2e\t*" % diff
    print "*"*62
