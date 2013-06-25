# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS QUADTEST;
CREATE DATABASE QUADTEST;
USE QUADTEST;

CREATE TABLE MYTEST_CONE (
    PARENTNAME    VARCHAR(32), #
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
    NAME          VARCHAR(32),   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
    INHERITSTYLE  VARCHAR(32)
);

INSERT INTO MYTEST_CONE VALUES ("", 0, 0, 500, 1, 0, 0, "S", 1000, 0, 0, 500, 500, 0., 0., 0., "IRON", "QD_OUTER","");

# Cut a hole in QD outer to place vacuum tube (so that vacuum tube does not have to be a daughter ot QD_OUTER
# note that it is made longer than qd_outer, but it will be clipped when it is removed
INSERT INTO MYTEST_CONE VALUES ("QD_OUTER", -400, 0, 0, 1, 1, 0, "I", 1100, 0, 0, 90, 90, 0., 0., 0., "VACUUM", "SUBTRACTING_TUBE","SUBTRACT");

INSERT INTO MYTEST_CONE VALUES ("", -400, 0, 500, 0, 0, 0, "S", 1000, 0, 0, 90, 90, 0., 0., 0., "VACUUM", "ZERO_FIELD_APERTURE","");



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
INSERT INTO BOXTEST_BOX VALUES ("QD_OUTER", 150., 0., 0, 0., 0., 0.0, "S", 500, 200, 1100, 0., 0.0, 0, 0.0, "", "VACUUM", "SUBTRACT_REGION","SUBTRACT");

INSERT INTO BOXTEST_BOX VALUES ("", 150., 0., 500, 0., 0., 1.0, "I", 500, 200, 1000, 0., 0.0, 0, 1.5, "QUAD", "VACUUM", "FIELD_REGION","");

