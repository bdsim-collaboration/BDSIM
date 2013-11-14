class reader :
    def __init__(self, fileName) : 
        self.geoDict  = {'RPP':['name','xmin','xmax','ymin','ymax','zmin','zmax'],
                         'BOX':['name','vx','vy','vz','hx1','hy1','hz1','hx2','hy2','hz2','hx3','hy3','hz3'],
                         'SPH':['name','vx','vy','vz','r'],
                         'RCC':['name','vx','vy','vz','hx','hy','hz','r'],
                         'REC':['name','vx','vy','vz','hx','hy','hz','rx1','ry1','rz1','rx2','ry2','rz2'],
                         'TRC':['name','vx','vy','vz','hx','hy','hz','r1','r2'],
                         'ELL':['name','fx1','fy1','fz1','fx2','fy2','fz2','l'],
                         'WED':['name','vx','vy','vz','hx1','hy1','hz1','hx2','hy2','hz2','hx3','hy3','hz3'],
                         'RAW':['name','vx','vy','vz','hx1','hy1','hz1','hx2','hy2','hz2','hx3','hy3','hz3'],
                         'ARB':['name',
                                'vx1','vy1','vz1','vx2','vy2','vz2','vx3','vy3','vz3',
                                'vx4','vy4','vz4','vx5','vy5','vz5','vx6','vy6','vz6',
                                'vx7','vy7','vz7','vx8','vy8','vz8'],
                         'YZP':['name','x'],
                         'XZP':['name','y'],
                         'XYP':['name','z'],
                         'PLA':['name','hx','hy','hz','vx','vy','vz'],
                         'XCC':['name','ay','az','r'],
                         'YCC':['name','ax','az','r'],
                         'ZCC':['name','ax','ay','r'],
                         'XEC':['name','ay','az','ly','lz'],
                         'YEC':['name','ax','az','lx','lz'],
                         'ZEC':['name','ax','ay','lx','ly'],
                         'QUA':['name','axx','ayy','azz','axy','axz','ayz','ax','ay','az','a0']}
        self.bodyDict = {}
        self.regiDict = {}

        self.readFlukaGeometry(fileName)
        
    def readTranformations(self,fileName) :
        f = open(fileName) 
        
        for l in f : 
            self.lineTransCheck(l) 

        f.close()

    def readFlukaGeometry(self,fileName) : 
        f = open(fileName) 
        
        # loop over lines in files 
        for l in f : 
            self.lineTypeCheck(l) 

        f.close()

    def lineTransCheck(self, line) :
        pass

    def lineTypeCheck(self,line) : 
        # split line 
        t = line.split() 
        if len(t) <= 0 : 
            return 
        try : 
            geoDictInd = self.geoDict.keys().index(t[0]) 
            geoType    = self.geoDict.keys()[geoDictInd]
            print geoType, len(t), len(self.geoDict[geoType])
            data = []
            
            name = t[1]
            data.append(geoType)
            for i in range(2,len(self.geoDict[geoType]),1) : 
                data.append(float(t[i]))
            self.bodyDict[name] = data
        except ValueError :
            return

    
    
    

            

