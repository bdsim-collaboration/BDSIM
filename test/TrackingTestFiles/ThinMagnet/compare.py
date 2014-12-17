import pymadx
import robdsim

def compare(column, madxFile='trackone', bdsimFile='output_0.root') : 
    m = pymadx.TfsArray(madxFile) 
    r = robdsim.robdsimOutput(bdsimFile) 
    
    md = ((m.GetSegment(5)).dataArray)[:,2:]
    rd = robdsim.d2darrayToNumpy(r.GetSamplerData(4))
    
    print shape(md)
    print shape(rd)
