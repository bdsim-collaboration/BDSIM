! standard options for runing bdsim

option, beampipeRadius = 10 * cm,
        outerDiameter = 1.9 * m,
        beampipeThickness = 0.5 * mm,
	defaultRangeCut=1000*m,
	prodCutPhotons=1000*m,
	prodCutElectrons=1000*m,
	prodCutPositrons=1000*m,
	elossHistoBinWidth = 0.01;

option,physicsList="__PHYSICSLISTS__";
option,storeTrajectories=1;
option,storeTrajectoryEnergyThreshold=-1;
option,stopTracks=1;
! option,stopSecondaries=1;