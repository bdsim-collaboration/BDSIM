# photon/electron beam splitting region
# 2mrad ILC extraction line


# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS QEX1;
CREATE DATABASE QEX1;
USE QEX1;

CREATE TABLE QEX1_CONE (
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

# Beampipe of QEX1 - 2mrad
INSERT INTO QEX1_CONE VALUES ("", 0., 0.0, 5750, 1.0, 0.0, 0.0, "S", 2500, 0, 0, 35, 35, 0.,  0.00, 0, 0.0, "QUAD", "ALUMINIUM", 1, "QEX1_BP"); 
INSERT INTO QEX1_CONE VALUES ("QEX1_BP", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "I", 2500, 0, 0, 34.0, 34.0, 0.,  0.00, 0., -0.095803008082, "QUAD", "VACUUM", 0, "QEX1_INNER");

# Outer Iron of Magnet - 2mrad
INSERT INTO QEX1_CONE VALUES ("", 0., 0.0, 5750, 1.0, 0.0, 0.0, "S", 2500, 35.0, 35.0, 450.0, 450.0, 0.,  0.00, 0., 0.0, "QUAD", "Iron", 1, "QEX1_OUTER"); 

CREATE TABLE BOXTEST_BOX (
    PARENTNAME    VARCHAR(32), #
    POSX          DOUBLE(10,3), #
    POSY          DOUBLE(10,3), #
    POSZ          DOUBLE(10,3), #
    RED           DOUBLE(10,3), #
    GREEN         DOUBLE(10,3), #
    BLUE          DOUBLE(10,3), #
    VISATT        VARCHAR(32),  # I = INVISIBLE, S = SOLID, W = WIREFRAME
    LENGTHX       DOUBLE(10,3), #
    LENGTHY       DOUBLE(10,3), #
    LENGTHZ       DOUBLE(10,3), #
    ROTPSI        DOUBLE(10,3), #
    ROTTHETA      DOUBLE(10,3), #
    ROTPHI        DOUBLE(10,3), #
    K1		  DOUBLE(10,3), #
    MAGTYPE	  VARCHAR(10,3), #
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32),   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
    INHERITSTYLE  VARCHAR(32)
);

# Cut a hole in QD outer to place vacuum tube (so that field tube does not have to be a daughter ot QD_OUTER
# note that it is made longer than qd_outer, but it will be clipped when it is removed
INSERT INTO BOXTEST_BOX VALUES ("QEX1_OUTER", 50., 0., 0, 0., 0., 0.0, "S", 500, 200, 2550, 0., 0.0, 0, 0.0, "", "VACUUM", "SUBTRACT_REGION","SUBTRACT");

INSERT INTO BOXTEST_BOX VALUES ("", 50., 0., 0, 0., 0., 1.0, "I", 500, 200, 2500, 0., 0.0, 0, 1.5, "QUAD", "VACUUM", "FIELD_REGION","");




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
  
# Drift from QEX1 to SEX1 - 2mrad
INSERT INTO D1A0_CONE VALUES (0., 0.0, 7597.5, 0.0, 1.0, 0.0, "S", 1195, 34, 87, 35, 88, 0.,  0.00, 0., "ALUMINIUM", ""); 

####################################################

CREATE TABLE SEX1_CONE (
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

# Beampipe of SEX1 - 2mrad
INSERT INTO SEX1_CONE VALUES ("", 0., 0.0, 10095, 1.0, 1.0, 0.0, "S", 3800, 0, 0, 88, 88, 0.,  0.00, 0., 0.0, "SEXT", "ALUMINIUM", 1, "SEX1_BP"); 
INSERT INTO SEX1_CONE VALUES ("SEX1_BP", 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "I", 3800, 0, 0, 87.0, 87.0, 0.,  0.00, 0.0, 0.625408483406, "SEXT", "VACUUM", 0, "SEX1_INNER");

# Outer Iron of Magnet - 2mrad
INSERT INTO SEX1_CONE VALUES ("", 0., 0.0, 10095, 1.0, 1.0, 0.0, "S", 3800, 88, 88, 250.0, 250.0, 0.,  0.00, 0., 0.0, "SEXT", "Iron", 0, "SEX1_OUTER"); 



