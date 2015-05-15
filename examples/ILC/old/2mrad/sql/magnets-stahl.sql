# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS QD0;
CREATE DATABASE QD0;
USE QD0;

CREATE TABLE QD0_CONE (
    PARENTNAME    VARCHAR(32),  # 
    POSX          DOUBLE(10,3), # mm
    POSY          DOUBLE(10,3), # mm
    POSZ          DOUBLE(10,3), # mm
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
    SETSENSITIVE  INTEGER(16),  # TO ADD THE VOLUME TO THE LIST OF SENSITIVE COMPONENTS 
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);

# Beampipe of QD0 - 2mrad
INSERT INTO QD0_CONE VALUES ("", 0., 0.0, 5750, 1.0, 0.0, 0.0, "S", 2500, 0, 0, 35, 35, 0.,  0.00, 0, 0.0, "QUAD", "ALUMINIUM", 1, "QD0_BP"); 
INSERT INTO QD0_CONE VALUES ("QD0_BP", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "I", 2500, 0, 0, 34.0, 34.0, 0.,  0.00, 0., -0.096027184064, "QUAD", "VACUUM", 0, "QD0_INNER");

# Outer Iron of Magnet - 2mrad
INSERT INTO QD0_CONE VALUES ("", 0., 0.0, 5750, 1.0, 0.0, 0.0, "S", 2500, 35.0, 35.0, 250.0, 250.0, 0.,  0.00, 0., 0.0, "QUAD", "Iron", 1, "QD0_OUTER"); 

####################################################

CREATE TABLE D1A0_CONE (
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
  
# Drift from QD0 to SD0 - 2mrad
INSERT INTO D1A0_CONE VALUES (0., 0.0, 7597.5, 0.0, 1.0, 0.0, "S", 1195, 34, 87, 35, 88, 0.,  0.00, 0., "ALUMINIUM", ""); 

####################################################

CREATE TABLE SD0_CONE (
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
    ALIGNOUT      INTEGER(16),  # Component to Align the outgoing beamline to
    NAME          VARCHAR(32)   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
);

# Beampipe of SD0 - 2mrad
INSERT INTO SD0_CONE VALUES ("", 0., 0.0, 10095, 1.0, 1.0, 0.0, "S", 3800, 0, 0, 88, 88, 0.,  0.00, 0., 0.0, "SEXT", "ALUMINIUM", 1, "SD0_BP"); 
INSERT INTO SD0_CONE VALUES ("SD0_BP", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "I", 3800, 0, 0, 87.0, 87.0, 0.,  0.00, 0.0, 0.613667397116, "SEXT", "VACUUM", 0, "SD0_INNER");

# Outer Iron of Magnet - 2mrad
INSERT INTO SD0_CONE VALUES ("", 0., 0.0, 10095, 1.0, 1.0, 0.0, "S", 3800, 88, 88, 250.0, 250.0, 0.,  0.00, 0., 0.0, "SEXT", "Iron", 0, "SD0_OUTER"); 



