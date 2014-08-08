# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS INVQD0;
CREATE DATABASE INVQD0;
USE INVQD0;

CREATE TABLE INVQD0_CONE (
    PARENTNAME    VARCHAR(32),  #
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
    K1            DOUBLE(10,3), # Magnet strength (e.g. K1 or K2 or K3...)
    MAGTYPE       VARCHAR(32),  # Magnet Type (e.g. QUAD, SEXT)
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);

# Beampipe of INVQD0 - 2mrad
INSERT INTO INVQD0_CONE VALUES ("", 5.750, 0.0, 6245, 1.0, 0.0, 0.0, "S", 2500, 0, 0, 35, 35, 0.,  0.00, 0., 0.0, "QUAD", "ALUMINIUM", "QD0_BP"); 
INSERT INTO INVQD0_CONE VALUES ("QD0_BP", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "I", 2500, 0, 0, 34.0, 34.0, 0.,  0.00, 0., -0.095803008082, "QUAD", "VACUUM", "QD0INNER");

# Outer Iron of Magnet - 2mrad
INSERT INTO INVQD0_CONE VALUES ("", 5.750, 0.0, 6245, 1.0, 0.0, 0.0, "S", 2500, 35.0, 35.0, 250.0, 250.0, 0.,  0.00, 0., 0.0, "QUAD", "Iron", "QD0_OUTER"); 

####################################################

CREATE TABLE INVD1A0_CONE (
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
  
# Drift from INVQD0 to INVSD0 - 2mrad
INSERT INTO INVD1A0_CONE VALUES (7.6025, 0.0, 4397.5, 0.0, 1.0, 0.0, "S", 1195, 87, 34, 88, 35, 0.,  0.00, 0., "ALUMINIUM", ""); 

####################################################

CREATE TABLE INVSD0_CONE (
    PARENTNAME    VARCHAR(32),  #
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
    K2            DOUBLE(10,3), # Magnet strength (e.g. K1 or K2 or K3...)
    MAGTYPE       VARCHAR(32),  # Magnet Type (e.g. QUAD, SEXT)
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    ALIGNIN       INTEGER(16),  # Component to Align the incoming beamline to
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);

# Beampipe of INVSD0 - 2mrad
INSERT INTO INVSD0_CONE VALUES ("",  10.099, 0.0, 1900, 1.0, 1.0, 0.0, "S", 3800, 0, 0, 88, 88, 0.,  0.00, 0., 0.0, "SEXT", "ALUMINIUM", 1, "SD0_BP"); 
INSERT INTO INVSD0_CONE VALUES ("SD0_BP", 0.0,   0.0, 0.0,  0.0, 0.0, 0.0, "I", 3800, 0,  0,  87, 87, 0.,  0.00, 0.0,   0.625408483406, "SEXT", "VACUUM", 0, "SD0_INNER");

# Outer Iron of Magnet - 2mrad
INSERT INTO INVSD0_CONE VALUES ("", 10.099, 0.0, 1900, 1.0, 1.0, 0.0, "S", 3800, 88, 88, 250.0, 250.0, 0.,  0.00, 0., 0.0, "SEXT", "Iron", 0, "SD0_OUTER"); 



