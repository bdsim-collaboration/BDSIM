import pylab as pl 

class MachineRep(list) : 
    def __init__(self) : 
        list()
       
    def writeLattice() :
        pass
           
def Linac() : 
    pass

def Ring(ndipole = 20, ldipole = 2.0, clength = 10.0, cell = []) : 
    '''Build a random ring
    ndipole : number of dipole magnets
    ldipole : length of dipole 
    clength : length of other components
    cell    : components and specificiations of each dipole cell
    '''

    line = []
    
    for i in range(0,ndipole,1) : 
        name = 'dipole.'+str(i)
        type = 'sbend' 
        angl = 2*pl.pi/ndipole
        leng = ldipole         
        line.append([name,type,leng,angl])

        name = 'drift.'+str(i)
        type = 'drift' 
        leng = clength-ldipole
        line.append([name,type,leng])
        
#        name = 'quadrupole.'+str(i)
#        line.append([name,'quadrupole',0.5,0.2])

    totalbl = 0.0
    totall  = 0.0 
    for e in line : 
        # total bending length
        if e[1] == 'sbend' : 
            totalbl += e[2]
            
        # sum all component lengths
        totall += e[2]

        print e

    print 'RingBuilder> total bength length',totalbl
    print 'RingBuilder> total length',totall


    f = open("RandomRing.gmad","w")

    # write components
    for e in line : 
        wl = ''
        if e[1] == 'sbend' : 
            wl = '{0} : {1}, l={2}, angle={3};\n'.format(e[0],e[1],e[2],e[3])
        elif e[1] == 'drift' : 
            wl = '{0} : {1}, l={2};\n'.format(e[0],e[1],e[2])         
        elif e[1] == 'quadrupole' : 
            wl = '{0} : {1}, l={2}, k={3};\n'.format(e[0],e[1],e[2],e[3])
        f.write(wl)

    # write line  
    l = ''
    last = False 
    for e in line :
        if e == line[-1] : 
            last = True

        l += e[0]
        if not last : 
            l += ','

    l = 'lat : line = ('+l+');\n'
    
    f.write(l)
        
    f.close()
