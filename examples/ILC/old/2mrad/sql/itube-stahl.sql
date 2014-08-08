# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS INVBEAMPIPE;
CREATE DATABASE INVBEAMPIPE;
USE INVBEAMPIPE;

CREATE TABLE INVTUBE_CONE (
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
INSERT INTO INVTUBE_CONE VALUES (30.5e-3, 0.0, 11964.5, 0.0, 1.0, 1.0, "S", 61, 14.0, 14.0, 14.5, 14.5, 0., 0., -1e-3, "BERYLLIUM", ""); 

# inner bulge near IP
INSERT INTO INVTUBE_CONE VALUES (116.5e-3, 0.0, 11878.5, 0.0, 1.0, 1.0, "S", 111, 22.0, 14.0, 23.0, 14.5, 0., 0., -1e-3, "BERYLLIUM", ""); 

# outer parallel part
INSERT INTO INVTUBE_CONE VALUES (218.05e-3, 0.0, 11776.95, 0.0, 1.0, 1.0, "S", 92.1, 22.0, 22.0, 23.0, 23.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# outer bulge in front of ECAL
INSERT INTO INVTUBE_CONE VALUES (1532.05e-3, 0.0, 10462.95, 0.0, 1.0, 1.0, "S", 2535.9, 239.0, 22.0, 240.0, 23.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# inside ECAL
INSERT INTO INVTUBE_CONE VALUES (2919.5e-3, 0.0, 9075.5, 0.0, 1.0, 1.0, "S", 239.0, 239.0, 239.0, 240.0, 240.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# face in front of LumCal
INSERT INTO INVTUBE_CONE VALUES (3039.5e-3, 0.0, 8955.5, 0.0, 1.0, 1.0, "S", 1.0, 69.0, 69.0, 240.0, 240.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# inside LumCal
INSERT INTO INVTUBE_CONE VALUES (3150e-3, 0.0, 8845.0, 0.0, 1.0, 1.0, "S", 220.0, 69.0, 69.0, 70.0, 70.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# bellow behind LumCal
INSERT INTO INVTUBE_CONE VALUES (3275e-3, 0.0, 8720.0, 0.0, 1.0, 1.0, "S", 30.0, 69.0, 69.0, 100.0, 100.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# in front of & behind pump
INSERT INTO INVTUBE_CONE VALUES (3444.5e-3, 0.0, 8550.5, 0.0, 1.0, 1.0, "S", 309.0, 99.0, 99.0, 100.0, 100.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# face in front of BeamCal
INSERT INTO INVTUBE_CONE VALUES (3599.5e-3, 0.0, 8395.5, 0.0, 1.0, 1.0, "S", 1.0, 15.0, 15.0, 100.0, 100.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# inside BeamCal
INSERT INTO INVTUBE_CONE VALUES (3729.5e-3, 0.0, 8265.5, 0.0, 1.0, 1.0, "S", 259.0, 15.0, 15.0, 16.0, 16.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

# face behind BeamCal
INSERT INTO INVTUBE_CONE VALUES (3859.5e-3, 0.0, 8135.5, 0.0, 1.0, 1.0, "S", 1.0, 15.0, 15.0, 20.0, 20.0, 0., 0., -1e-3, "BERYLLIUM", ""); 

 # face behind BeamCal
INSERT INTO INVTUBE_CONE VALUES (3910e-3, 0.0, 8085.0, 0.0, 1.0, 1.0, "S", 100.0, 19.0, 19.0, 20.0, 20.0, 0., 0., -1e-3, "BERYLLIUM", "");
