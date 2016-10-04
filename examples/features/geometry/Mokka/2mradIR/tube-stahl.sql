# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS BEAMPIPE;
CREATE DATABASE BEAMPIPE;
USE BEAMPIPE;

CREATE TABLE TUBE_CONE (
    POSX          DOUBLE(10,3), #
    POSY          DOUBLE(10,3), #
    POSZ          DOUBLE(10,3), #
    RED           DOUBLE(10,3), #
    GREEN         DOUBLE(10,3), #
    BLUE          DOUBLE(10,3), #
    VISATT        VARCHAR(32),  # I = INVISIBLE, S = SOLID, W = WIREFRAME
    LENGTH        DOUBLE(10,3), #
    RINNERSTART   DOUBLE(10,3), #
    RINNEREND     DOUBLE(10,3), #
    ROUTERSTART   DOUBLE(10,3), #
    ROUTEREND     DOUBLE(10,3), #
    ROTPSI        DOUBLE(10,3), #
    ROTTHETA      DOUBLE(10,3), #
    ROTPHI        DOUBLE(10,3), #
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);
                      
# inner parallel part, next to IP
INSERT INTO TUBE_CONE VALUES (0, 0.0, 30.5, 0.0, 1.0, 1.0, "S", 61, 14.0, 14.0, 14.5, 14.5, 0., 0., 0.0, "Beryllium", ""); 

# inner bulge near IP
INSERT INTO TUBE_CONE VALUES (0, 0.0, 116.5, 0.0, 1.0, 1.0, "S", 111, 14.0, 22.0, 14.5, 23.0, 0., 0., 0.0, "Beryllium", ""); 

# outer parallel part
INSERT INTO TUBE_CONE VALUES (0, 0.0, 218.05, 0.0, 1.0, 1.0, "S", 92.1, 22.0, 22.0, 23.0, 23.0, 0., 0., 0.0, "Beryllium", ""); 

# outer bulge in front of ECAL
INSERT INTO TUBE_CONE VALUES (0, 0.0, 1532.05, 0.0, 1.0, 1.0, "S", 2535.9, 22.0, 239.0, 23.0, 240.0, 0., 0., 0.0, "Beryllium", ""); 

# inside ECAL
INSERT INTO TUBE_CONE VALUES (0, 0.0, 2919.5, 0.0, 1.0, 1.0, "S", 239.0, 239.0, 239.0, 240.0, 240.0, 0., 0., 0.0, "Beryllium", ""); 

# face in front of LumCal
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3039.5, 0.0, 1.0, 1.0, "S", 1.0, 69.0, 69.0, 240.0, 240.0, 0., 0., 0.0, "Beryllium", ""); 

# inside LumCal
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3150.0, 0.0, 1.0, 1.0, "S", 220.0, 69.0, 69.0, 70.0, 70.0, 0., 0., 0.0, "Beryllium", ""); 

# bellow behind LumCal
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3275.0, 0.0, 1.0, 1.0, "S", 30.0, 69.0, 69.0, 100.0, 100.0, 0., 0., 0.0, "Beryllium", ""); 

# in front of & behind pump
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3444.5, 0.0, 1.0, 1.0, "S", 309.0, 99.0, 99.0, 100.0, 100.0, 0., 0., 0.0, "Beryllium", ""); 

# face in front of BeamCal
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3599.5, 0.0, 1.0, 1.0, "S", 1.0, 15.0, 15.0, 100.0, 100.0, 0., 0., 0.0, "Beryllium", ""); 

# inside BeamCal
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3729.5, 0.0, 1.0, 1.0, "S", 259.0, 15.0, 15.0, 16.0, 16.0, 0., 0., 0.0, "Beryllium", ""); 

# face behind BeamCal
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3859.5, 0.0, 1.0, 1.0, "S", 1.0, 15.0, 15.0, 20.0, 20.0, 0., 0., 0.0, "Beryllium", ""); 

 # face behind BeamCal
INSERT INTO TUBE_CONE VALUES (0, 0.0, 3910.0, 0.0, 1.0, 1.0, "S", 100.0, 19.0, 19.0, 20.0, 20.0, 0., 0., 0.0, "Beryllium", "");
