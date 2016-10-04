# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS OUTERMASKS;
CREATE DATABASE OUTERMASKS;
USE OUTERMASKS;

CREATE TABLE OUTER_POLYCONE (
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
    RINNER1       DOUBLE(10,3), #
    RINNER2       DOUBLE(10,3), #
    RINNER3       DOUBLE(10,3), #
    ROUTER1       DOUBLE(10,3), #
    ROUTER2       DOUBLE(10,3), #
    ROUTER3       DOUBLE(10,3), #
    STARTPHI      DOUBLE(10,3), #
    DELTAPHI      DOUBLE(10,3), #
    ROTPSI        DOUBLE(10,3), #
    ROTTHETA      DOUBLE(10,3), #
    ROTPHI        DOUBLE(10,3), #
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);

# Rough approximation of the outer masking elements
INSERT INTO OUTER_POLYCONE VALUES (0.0, 0.0, 4850.0, 0.0, 0.0, 1.0, "S", 3, -3000, -1350, 3000,
       	    		      	     	   	     	       	    	       80,   160,  160,
       	    		      	     	   	     	       	    	      500,   500,  500, 3.14, 3.14,
									     0.0, 0.0, 0.0, "Tungsten", ""); 

#############################################################
