# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS POLYTEST;
CREATE DATABASE POLYTEST;
USE POLYTEST;

CREATE TABLE POLYTEST_POLYCONE (
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
    RINNER1       DOUBLE(10,3), #
    RINNER2       DOUBLE(10,3), #
    RINNER3       DOUBLE(10,3), #
    RINNER4       DOUBLE(10,3), #
    RINNER5       DOUBLE(10,3), #
    ROUTER1       DOUBLE(10,3), #
    ROUTER2       DOUBLE(10,3), #
    ROUTER3       DOUBLE(10,3), #
    ROUTER4       DOUBLE(10,3), #
    ROUTER5       DOUBLE(10,3), #
    ROTPSI        DOUBLE(10,3), #
    ROTTHETA      DOUBLE(10,3), #
    ROTPHI        DOUBLE(10,3), #
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);

# Beampipe of POLYTEST - 2mrad
INSERT INTO POLYTEST_POLYCONE VALUES (0.0, 0.0, 200, 1.0, 0.0, 0.0, "S", 5, -100, -50, 0, 200, 300, 
       	    		      	     	   	     	       	    	     20, 40, 100, 30, 50,
       	    		      	     	   	     	       	    	     25, 45, 105, 35, 55,									    
									     0.0, 0.0, 0.1, "ALUMINIUM", ""); 

