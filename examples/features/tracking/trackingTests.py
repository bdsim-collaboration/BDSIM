import numpy as _np
from scipy import constants as _con

elements = ['drift',
            'rbend',
            'sbend',
            'vkick',
            'hkick',
            'quadrupole',
            'sextupole',
            'octopole',
            'decapole',
            'multipole',
            'thinmultipole',
            'rfcavity',
            'rcol',
            'ecol',
            'degrader',
            'muspoiler',
            'shield',
            'solenoid',
            'laser']


class params():
    def __init__(self,includeOptional=False):
        self._includeOptional = includeOptional
        self._setParams()
    
    def _setParams(self):
        #global params applicable to all elements
        #electron energies: ATF2,   DLS,    ILC, CLIC
        #proton energies:   PSI G2, J-PARC, SPS, LHC, FCC
        #Note, PSI G2 is the 250MeV k.e. proton medical gantry 2.s
        self.globalParams = {'particle'       : ['e-','proton'],
                             'electronEnergy' : [1.282,    3,  250, 1500],
                             'protonEnergy'   : [1.188272, 50, 400, 4000, 50000]}
        
        #use same magnet strengths for multipole and thinMultipole components
        self._magStrengths = {'k1'   : 0.025,
                             'k2'   : 5.0,
                             'k3'   : 1.5e3,
                             'k4'   : 6.0e5,
                             'k5'   : 3.0e8}
        
        #tuples for multipole and thinMultipole (all components for given element)
        k1l = [(-1.0 * self._magStrengths['k1']),             (0), (1.0 * self._magStrengths['k1'])]
        k2l = [(0, -1.0 * self._magStrengths['k1']),          (0), (0, 1.0 * self._magStrengths['k1'])]
        k3l = [(0, 0, -1.0 * self._magStrengths['k1']),       (0), (0, 0, 1.0 * self._magStrengths['k1'])]
        k4l = [(0, 0, 0, -1.0 * self._magStrengths['k1']),    (0), (0, 0, 0, 1.0 * self._magStrengths['k1'])]
        k5l = [(0, 0, 0, 0, -1.0 * self._magStrengths['k1']), (0), (0, 0, 0, 0, 1.0 * self._magStrengths['k1'])]
        
        self.elementParams = {'length'    : [0.2, 2.0, 20.0],
                              'angle'     : [-0.3, -0.003, 0, 0.003, 0.3],
                              'field'     : [0, 0, 0, 0, 0],
                              'e1'        : [-0.3491, -0.003491, 0, 0.003491, 0.3491],
                              'e2'        : [-0.3491, -0.003491, 0, 0.003491, 0.3491],
                              'k1'        : [-1.0 * self._magStrengths['k1'], 0, self._magStrengths['k1']],
                              'k2'        : [-1.0 * self._magStrengths['k2'], 0, self._magStrengths['k2']],
                              'k3'        : [-1.0 * self._magStrengths['k3'], 0, self._magStrengths['k3']],
                              'k4'        : [-1.0 * self._magStrengths['k4'], 0, self._magStrengths['k4']],
                              'knl'       : [k1l,k2l,k3l,k4l,k5l],
                              'ksl'       : [k1l,k2l,k3l,k4l,k5l],
                              'gradient'  : [-20.0, -0.002, 0, 0.002, 20.0],
                              'x(col)'    : [50],
                              'y(col)'    : [50],
                              'xOut(col)' : [50],
                              'yOut(col)' : [50],
                              'numWedges' : [1, 5, 10],
                              'thickness' : [0.01, 0.1, 1.0], #fraction of element length
                              'ks'        : [-0.3, -0.003, 0, 0.003, 0.3],
                              '**fint'    : [0, 1.0/6.0, 0.5],
                              '**fintx'   : [0, 1.0/6.0, 0.5],
                              '**hgap'    : [0.01, 0.1]}
        
        self.hasParams = {  'drift'         : ['length'],
                            'rbend'         : ['length','angle','field','e1','e2','**fint','**fintx','**hgap'],
                            'sbend'         : ['length','angle','field','e1','e2','**fint','**fintx','**hgap'],
                            'vkick'         : ['length','angle','field'],
                            'hkick'         : ['length','angle','field'],
                            'quadrupole'    : ['length','k1'],
                            'sextupole'     : ['length','k2'],
                            'octopole'      : ['length','k3'],
                            'decapole'      : ['length','k4'],
                            'multipole'     : ['length','knl','ksl'],
                            'thinmultipole' : ['knl','ksl'],
                            'rfcavity'      : ['length','gradient'],
                            'rcol'          : ['length','x(col)','y(col)', 'xOut(col)','yOut(col)'],
                            'ecol'          : ['length','x(col)','y(col)', 'xOut(col)','yOut(col)'],
                            'degrader'      : ['length','numWedges','thickness'],
                            'muspoiler'     : ['length','field'],
                            'shield'        : ['length'],
                            'solenoid'      : ['length','ks'],
                            'laser'         : ['length']}

        self.numFiles =  {}
        for element in elements:
            self.numFiles[element] = 1
        
        #calculate number of beam variations. Proton and electron calculated seperately.
        numBeamVariations = 0
        for particle in self.globalParams['particle']:
            if particle == 'e-':
                for elecEnergy in self.globalParams['electronEnergy']:
                    numBeamVariations += 1
            if particle == 'proton':
                for elecEnergy in self.globalParams['protonEnergy']:
                    numBeamVariations += 1
        self.numBeamVariations = numBeamVariations
        

        for key in self.hasParams.keys():
            numElementVariations = 1
            params = self.hasParams[key]
            for elementKey in params:
                #multiple by the number component strengths for multipole
                if elementKey == 'knl' or elementKey == 'ksl':
                    numElementVariations *= (len(k1l)*len(self.elementParams[elementKey]))
                elif elementKey == 'rcol' or elementKey == 'ecol':
                    pass
                elif elementKey == 'rbend' or elementKey == 'sbend':
                    pass
                else:
                    numElementVariations *= len(self.elementParams[elementKey])

            if key == 'rbend' or key == 'sbend':
                numBendTests = 0
                for length in self.elementParams['length']:
                    for ang in self.elementParams['angle']:
                        for e1 in self.elementParams['e1']:
                            for e2 in self.elementParams['e2']:
                                for fint in self.elementParams['**fint']:
                                    for fintx in self.elementParams['**fintx']:
                                        for hgap in self.elementParams['**hgap']:
                                            numBendTests += 1

                numElementVariations = numBendTests
                numElementVariations *= 2 #angle or field

            if key == 'hkick' or key == 'vkick':
                #don't vary angle AND field, only vary angle OR field.
                numElementVariations /= 5
                numElementVariations *= 2

            #set explicitly for collimator due to limited scenarios (eg can't have tapered with aperture 0 at just one end)
            if key == 'rcol' or key == 'ecol':
                numColVars = len(self.elementParams['length'])*5
                numElementVariations = numColVars

            numElementVariations *= self.numBeamVariations

            self.numFiles[key] *= numElementVariations
        
        self.totalFiles = 0
        for key in self.numFiles:
            self.totalFiles += self.numFiles[key]

def calcBField(length,angle,energy,particle):
    #Calculate the magnetic field for a dipole
    if angle == 0:
        return 0
    else:
        energies = calcEnergy(energy,particle)
        rho = length / angle
        b = energies['brho'] / rho
        return b

def calcEnergy(total_energy,particle='e-'):
    #Calculate the energy & momentum of a proton and electron at a given total energy.
    eMass = _con.electron_mass * _con.c**2 / _con.e / 1e9
    pMass = _con.proton_mass * _con.c**2 / _con.e / 1e9

    if particle == 'proton' or particle == 'p':
        protonEnergy = total_energy
        protonKinetic = total_energy - pMass
        protonMomentum = _np.sqrt(total_energy**2 - pMass**2)
        brho = 3.335640951981521 * protonMomentum
        electronMomentum = protonMomentum
        electronEnergy = _np.sqrt(electronMomentum**2 + eMass**2)
        electronKinetic = electronEnergy - eMass
    elif particle == 'e' or particle == 'e-':
        electronEnergy = total_energy
        electronKinetic = total_energy - eMass
        electronMomentum = _np.sqrt(total_energy**2 - eMass**2)
        brho = 3.335640951981521 * electronMomentum
        protonMomentum = electronMomentum
        protonEnergy = _np.sqrt(protonMomentum**2 + pMass**2)
        protonKinetic = protonEnergy - pMass

    res={'e-'     : {'KE' : electronKinetic,
                     'TE' : electronEnergy,
                     'P'  : electronMomentum},
         'proton' : {'KE' : electronKinetic,
                     'TE' : electronEnergy,
                     'P'  : electronMomentum},
         'brho'   : brho}
    return res

