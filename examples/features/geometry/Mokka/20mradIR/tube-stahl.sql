# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS BEAMPIPE;
CREATE DATABASE BEAMPIPE;
USE BEAMPIPE;

CREATE TABLE BP_POLYCONE (
    POSX          DOUBLE(10,3), #
    POSY          DOUBLE(10,3), #
    POSZ          DOUBLE(10,3), #
    RED           DOUBLE(10,3), #
    GREEN         DOUBLE(10,3), #
    BLUE          DOUBLE(10,3), #
    VISATT        VARCHAR(32),  # I = INVISIBLE, S = SOLID, W = WIREFRAME
    NZPLANES      INTEGER(16),  #
    PLANEPOS1     DOUBLE(10,3), #
    PLANEPOS2     DOUBLE(10,3), #
    PLANEPOS3     DOUBLE(10,3), #
    PLANEPOS4     DOUBLE(10,3), #
    PLANEPOS5     DOUBLE(10,3), #
    PLANEPOS6     DOUBLE(10,3), #
    PLANEPOS7     DOUBLE(10,3), #
    RINNER1       DOUBLE(10,3), #
    RINNER2       DOUBLE(10,3), #
    RINNER3       DOUBLE(10,3), #
    RINNER4       DOUBLE(10,3), #
    RINNER5       DOUBLE(10,3), #
    RINNER6       DOUBLE(10,3), #
    RINNER7       DOUBLE(10,3), #
    ROUTER1       DOUBLE(10,3), #
    ROUTER2       DOUBLE(10,3), #
    ROUTER3       DOUBLE(10,3), #
    ROUTER4       DOUBLE(10,3), #
    ROUTER5       DOUBLE(10,3), #
    ROUTER6       DOUBLE(10,3), #
    ROUTER7       DOUBLE(10,3), #
    STARTPHI      DOUBLE(10,3), #
    DELTAPHI      DOUBLE(10,3), #
    ROTPSI        DOUBLE(10,3), #
    ROTTHETA      DOUBLE(10,3), #
    ROTPHI        DOUBLE(10,3), #
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);

# Beampipe of BP - 2mrad
INSERT INTO BP_POLYCONE VALUES (0.0, 0.0, 1500.0, 1.0, 0.0, 0.0, "S", 7, -1500, -1455, -1450, -1300,-500, 0, 1500, 
       	    		      	     	   	     	       	    	 15,    15,    30,    30,  50, 50, 120,
       	    		      	     	   	     	       	    	 17,    17,    32,    32,  52, 52, 122, 3.14, 3.14,
									     0.0, 0.0, 0.0, "Aluminium", ""); 

