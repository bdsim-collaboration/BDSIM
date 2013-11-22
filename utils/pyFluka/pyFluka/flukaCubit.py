import pylab as pl 


class createJou:
    def __init__(self,fileName):
        self.f=open(fileName,'w')
        self.f.write('reset\n')

    def MakeModel(self,mod):
        self.f.write(mod)

    def export(self,exportName,reset=True):
        self.f.write('export stl ascii "'+exportName+'"\n')
        if reset==True:
            self.f.write('reset\n')

    def close(self):
        self.f.close()

class Primitives(object):
    def __init__(self):
        self.Inf=1e6

    def translation(self,array):
        x=array[0]
        y=array[1]
        z=array[2]
        s=''
        s+='move volume 1 x '+str(x)+' y '+str(y)+' z '+str(z)+' include_merged\n'
        return s

    def RPP(self,array):
        s=''
        xmin=array[0]
        xmax=array[1]
        ymin=array[2]
        ymax=array[3]
        zmin=array[4]
        zmax=array[5]
        s+='brick x '+str(xmax-xmin)+' y '+str(ymax-ymin)+' z '+str(zmax-zmin)+ '\n' 
        s+='move volume 1 x '+str((xmax-xmin)/2.0 + xmin)+' y '+str((ymax-ymin)/2.0 + ymin)+' z '+str((zmax-zmin)/2.0 + zmin)+' include_merged\n'
        return s

    def BOX(self,array):
        s=''
        vx=array[0]
        vy=array[1]
        vz=array[2]
        Hx1=array[3]
        Hy1=array[4]
        Hz1=array[5]
        Hx2=array[6]
        Hy2=array[7]
        Hz2=array[8]
        Hx3=array[9]
        Hy3=array[10]
        Hz3=array[11]
        xLength = pl.sqrt(Hx1**2 + Hy1**2 + Hz1**2)
        yLength = pl.sqrt(Hx2**2 + Hy2**2 + Hz2**2)
        zLength = pl.sqrt(Hx3**2 + Hy3**2 + Hz3**2)
        s+='create curve location '+str(vx) + ' ' +str(vy)+ ' '+str(vz)+'  direction '+str(Hx1) + ' ' +str(Hy1)+ ' '+str(Hz1)+' length '+str(xLength)+' \n'
        s+='create curve location '+str(vx) + ' ' +str(vy)+ ' '+str(vz)+'  direction '+str(Hx2) + ' ' +str(Hy2)+ ' '+str(Hz2)+' length '+str(yLength)+' \n'
        s+='create curve location '+str(vx) + ' ' +str(vy)+ ' '+str(vz)+'  direction '+str(Hx3) + ' ' +str(Hy3)+ ' '+str(Hz3)+' length '+str(zLength)+' \n'
        s+='sweep curve 1 along curve 2 \n'
        s+='sweep surface 1 along curve 3 \n'
        return s

    def RCC(self,array):
        s=''
        vx=array[0]
        vy=array[1]
        vz=array[2]
        Hx=array[3]
        Hy=array[4]
        Hz=array[5]
        R=array[6]
        Length = pl.sqrt(Hx**2 + Hy**2 + Hz**2)
        s+='create curve location '+str(vx)+' '+str(vy)+' '+str(vz)+' direction '+str(Hx)+' '+str(Hy)+' '+str(Hz)+' length '+ str(Length)+' \n'
        s+='create curve arc center vertex 1 radius '+str(R)+' normal '+str(Hx)+' '+str(Hy)+' '+str(Hz)+' full \n'
        s+='create surface curve 2 \n'
        s+='sweep surface 1 along curve 1 \n'
        return s

    def ZCC(self,array):
        s=''
        Ax = array[0]
        Ay = array[1]
        R = array[2]
        s+='create surface circle radius '+str(R)+' zplane\n'
        s+='move surface 1 x '+str(Ax)+' y '+str(Ay)+' include_merged\n'
        s+='sweep surface 1 perpendicular distance '+str(self.Inf)+'\n'
        s+='sweep surface 3 perpendicular distance '+str(self.Inf)+'\n'
        return s

    def XCC(self,array):
        s=''
        Ay = array[0]
        Az = array[1]
        R = array[2]
        s+='create surface circle radius '+str(R)+' xplane\n'
        s+='move surface 1 y '+str(Ay)+' z '+str(Az)+' include_merged\n'
        s+='sweep surface 1 perpendicular distance '+str(self.Inf)+'\n'
        s+='sweep surface 3 perpendicular distance '+str(self.Inf)+'\n'
        return s

    def YCC(self,array):
        s=''
        Az = array[0]
        Ax = array[1]
        R = array[2]
        s+='create surface circle radius '+str(R)+' yplane\n'
        s+='move surface 1 x '+str(Ax)+' z '+str(Az)+' include_merged\n'
        s+='sweep surface 1 perpendicular distance '+str(self.Inf)+'\n'
        s+='sweep surface 3 perpendicular distance '+str(self.Inf)+'\n'
        return s

    def YZP(self,array):
        s=''
        Vx = array[0]
        s+='brick x '+str(2*self.Inf)+' y '+str(2*self.Inf)+' z '+str(2*self.Inf)+'\n'
        s+='move Volume 1 x '+str(self.Inf+Vx)+ 'include_merged\n' 
        return s

    def XZP(self,array):
        s=''
        Vy = array[0]
        s+='brick x '+str(2*self.Inf)+' y '+str(2*self.Inf)+' z '+str(2*self.Inf)+'\n'
        s+='move Volume 1 y '+str(self.Inf+Vy)+ 'include_merged\n' 
        return s

    def XYP(self,array):
        s=''
        Vz = array[0]
        s+='brick x '+str(2*self.Inf)+' y '+str(2*self.Inf)+' z '+str(2*self.Inf)+'\n'
        s+='move Volume 1 z '+str(self.Inf+Vz)+ 'include_merged\n' 
        return s

    def PLA(self, array):
        s=''
        Hx = array[0]
        Hy = array[1]
        Hz = array[2]
        Vx = array[3]
        Vy = array[4]
        Vz = array[5]
        s+='create curve location '+str(Vx)+' '+str(Vy)+' '+str(Vz)+' direction '+str(Hx)+' '+str(Hy)+' '+str(Hz)+' length '+str(self.Inf)+'\n' 
        s+='create curve arc center vertex 1 radius '+str(self.Inf)+' normal '+str(Hx)+' '+str(Hy)+' '+str(Hz)+'full\n'
        s+='create surface curve 2\n'
        s+='sweep surface 1 along curve 1\n'
        return s
