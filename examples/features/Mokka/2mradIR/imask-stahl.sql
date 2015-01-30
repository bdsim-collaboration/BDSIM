# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS MASKS;
CREATE DATABASE MASKS;
USE MASKS;

CREATE TABLE LUMCAL_CONE (
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
                      
INSERT INTO LUMCAL_CONE VALUES (3150e-3, 0.0, 8845.0, 0.8, 0.2, 0.2, "S", 200, 80.0, 80.0, 280.0, 280.0, 0., 0., 1e-3, "TUNGSTEN", ""); 

#############################################################

CREATE TABLE BEAMCAL_CONE (
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
                      
INSERT INTO BEAMCAL_CONE VALUES (3750e-3, 0.0, 8245.0, 0.2, 0.4, 1.0, "S", 200, 16.0, 16.0, 120.0, 120.0, 0., 0., 1e-3, "TUNGSTEN", ""); 

#############################################################

CREATE TABLE SUPPORT_CONE (
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

# between LumCal and pump                      
INSERT INTO SUPPORT_CONE VALUES (3295e-3, 0.0, 8700.0, 0.2, 0.6, 0.2, "S", 90, 110.0, 110.0, 160.0, 160.0, 0., 0., 1e-3, "TUNGSTEN", ""); 

# shield around pump
INSERT INTO SUPPORT_CONE VALUES (3425e-3, 0.0, 8570.0, 0.2, 0.6, 0.2, "S", 170, 210.0, 210.0, 250.0, 250.0, 0., 0., 1e-3, "TUNGSTEN", ""); 

# shield behind pump
INSERT INTO SUPPORT_CONE VALUES (3545e-3, 0.0, 8450.0, 0.2, 0.6, 0.2, "S", 70, 100.0, 100.0, 250.0, 250.0, 0., 0., 1e-3, "TUNGSTEN", ""); 

# main support tube
INSERT INTO SUPPORT_CONE VALUES (3770e-3, 0.0, 8225.0, 0.2, 0.6, 0.2, "S", 380, 180.0, 180.0, 250.0, 250.0, 0., 0., 1e-3, "TUNGSTEN", ""); 

#############################################################

CREATE TABLE ELECTRONICS_CONE (
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

# around LumCal
INSERT INTO ELECTRONICS_CONE VALUES (3150e-3, 0.0, 8845.0, 0.8, 0.8, 0.0, "S", 200, 280.0, 280.0, 300.0, 300.0, 0., 0., 1e-3, "SILICON", ""); 

# Connection
INSERT INTO ELECTRONICS_CONE VALUES (3275e-3, 0.0, 8720.0, 0.8, 0.8, 0.0, "S", 50, 250.0, 250.0, 300.0, 300.0, 0., 0., 1e-3, "SILICON", ""); 

# behind LumCal
INSERT INTO ELECTRONICS_CONE VALUES (3290e-3, 0.0, 8705.0, 0.8, 0.8, 0.0, "S", 80, 160.0, 160.0, 250.0, 250.0, 0., 0., 1e-3, "SILICON", ""); 

# around BeamCal
INSERT INTO ELECTRONICS_CONE VALUES (3750e-3, 0.0, 8245.0, 0.8, 0.8, 0.0, "S", 200, 120.0, 120.0, 180.0, 180.0, 0., 0., 1e-3, "SILICON", ""); 

#############################################################

CREATE TABLE VACPUMP_CONE (
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

# around LumCal
INSERT INTO VACPUMP_CONE VALUES (3350e-3, 0.0, 8645.0, 0.0, 0.0, 1.0, "S", 10, 100.0, 100.0, 205.0, 205.0, 0., 0., 1e-3, "IRON", ""); 

# Connection
INSERT INTO VACPUMP_CONE VALUES (3500e-3, 0.0, 8495.0, 0.0, 0.0, 1.0, "S", 10, 100.0, 100.0, 205.0, 205.0, 0., 0., 1e-3, "IRON", ""); 

# behind LumCal
INSERT INTO VACPUMP_CONE VALUES (3425e-3, 0.0, 8570.0, 0.0, 0.0, 1.0, "S", 140, 195.0, 195.0, 205.0, 205.0, 0., 0., 1e-3, "IRON", ""); 

# around BeamCal
INSERT INTO VACPUMP_CONE VALUES (3425e-3, 0.0, 8570.0, 0.0, 0.0, 1.0, "S", 140, 100.0, 100.0, 110.0, 110.0, 0., 0., 1e-3, "IRON", ""); 

# around BeamCal
INSERT INTO VACPUMP_CONE VALUES (3905e-3, 0.0, 8090.0, 0.0, 0.0, 1.0, "S", 70, 20.0, 20.0, 130.0, 130.0, 0., 0., 1e-3, "IRON", ""); 

#############################################################
