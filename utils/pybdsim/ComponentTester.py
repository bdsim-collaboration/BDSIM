import Tester as _Tester

class ComponentTester(_Tester.Tester) : 
    def __init__(self, particle = 'e-', energy = 1.5, component = 'quad') :
        print 'ComponentTester.ComponentTester.__init__>'
        _Tester.Tester()

        self.particle  = particle 
        self.energy    = energy 
        self.component = component
    
    def makeOptions(self) : 
        pass

    def makeBeam(self) : 
        pass 

    def makeLattice(self) : 
        pass 

    def run(self) :
        pass
    
    def plotOutput(self) :
        pass

