import flukaCubit as fc 

class reader :
    def __init__(self, fileName) : 
        # need to relabel this body Dict 
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
        # fluka cmd dict 
        self.cmdDict = {'*':[]} 

        self.bodyDict = {}
        self.regiDict = {}
        self.rotnDict = {}

#        self.readFlukaGeometry(fileName)
        f = open(fileName) 
        self.readFile = f.read()
        self.readFlukaGeometryToken()
        self.readFlukaRegion()
        self.readFlukaRotnToken()

        f.close()

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

    def readFlukaGeometryToken(self) :        
        rfs = self.readFile.split()

        translat = [0., 0., 0.] 
        transform = '' 

        for i in range(0,len(rfs),1) : 
            # ########################
            # Get the transform associated with the geometric object 
            # ########################
            if rfs[i] == '$start_translat' : 
                translat = [float(rfs[i+1]), float(rfs[i+2]), float(rfs[i+3])]
            if rfs[i] == '$start_transform' : 
                transform = rfs[i+1]
            if rfs[i] == '$end_translat' :
                translat = [0., 0., 0.] 
            if rfs[i] == '$end_transform' : 
                transform = ''
            try : 
                geoDictInd = self.geoDict.keys().index(rfs[i])
                geoType    = self.geoDict.keys()[geoDictInd]
#                print geoType, len(self.geoDict[geoType])
                data = []
            
                name = rfs[i+1]
                data.append(geoType)
                for j in range(i+2,i+len(self.geoDict[geoType])+1,1) : 
                    data.append(float(rfs[j]))
                self.bodyDict[name] = [data,translat,transform]
            except ValueError :
                continue

    def readFlukaRotnToken(self) : 
        rfs = self.readFile.split() 

        for i in range(0,len(rfs),1) : 
            if rfs[i] == 'ROT-DEFI' :
                rdata = []
                for j in range(0,6,1) : 
                    rdata.append(float(rfs[i+j+1]))
                self.rotnDict[rfs[i+7]] = rdata
            
    def readFlukaRegion(self) :
        '''Does a seach for a leading 'END' token to 'GEOEND' to find regions''' 

        rfs = self.readFile.split('\n')

        geoBegin = False
        iRegion  = -1
        regionDef = '' 
        regionName = '' 

        for l in rfs : 
            t = l.split() 
            if len(t) ==  0 : 
                continue 
            if l[0] == '*' : 
                continue
            if l[0] == '#' : 
                continue 
            if l[0] == '$' : 
                continue
            if t[0] == 'LATTICE' : 
                continue
            if t[0] == 'END' :
                geoBegin = True
                continue
            if t[0] == 'GEOEND' : 
                geoBegin = False



            if geoBegin : 
                try : 
                    self.geoDict.keys().index(t[0])
                    continue 
                except ValueError :
                    if l[0:3] != '   ' : 
                        iRegion = iRegion+1 

                        # remove the init case 
                        if regionName != '' : 
                            self.regiDict[regionName] = regionDef

                        regionName = t[0] 
                        regionDef  = l 
#                        print regionName+':'+l
                    else : 
#                        print regionName+':'+l
                        regionDef = regionDef+l

    def makeCubitFile(self, region) : 
        geo = self.regiDict[region] 
        t = geo.split() 


        for i in range(2,len(t),1) : 
            print t[i]
            name = t[i][1:]
            data = self.bodyDict[name] 
            v1 = fc.createJou(t[i][1:]+'.jou')
            if data[0][0] == 'RPP' :                 
                v1.MakeModel(fc.RPP(data[0][1:]))                
            elif data[0][0] == 'RCC' : 
                v1.MakeModel(fc.RCC(data[0][1:]))                
                
            v1.export(name+'.stl')
            
#            os./Applications/Cubit.app/Contents/MacOS/cubitcl -input TAS_BODY.jou -nographics -nojournal -batch
