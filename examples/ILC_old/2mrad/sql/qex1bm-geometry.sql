# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS QEX1B;
CREATE DATABASE QEX1B;
USE QEX1B;

CREATE TABLE QEX1B_CONE (
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
    SETSENSITIVE  INTEGER(16),  # TO ADD THE VOLUME TO THE LIST OF SENSITIVE COMPONENTS 
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32),   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
    INHERITSTYLE  VARCHAR(32)
);


#yoke
INSERT INTO QEX1B_CONE VALUES ("", 0, 0, 1500, 1, 0, 0, "S", 3000, 0, 0, 500, 500, 0., 0., 0.,1, "IRON", "QEX1B_OUTER","");



# Cut a hole in  outer to place vacuum tube (so that vacuum tube does not have to be a daughter ot QD_OUTER
# note that it is made longer than qd_outer, but it will be clipped when it is removed

# incoming
INSERT INTO QEX1B_CONE VALUES ("QEX1B_OUTER", -171.5,0., 0, 1, 1, 0, "I", 3100, 0, 0, 10.., 10.., 0., 0.,-0.0056 ,0, "VACUUM", "SUBTRACTING_TUBE","SUBTRACT");

INSERT INTO QEX1B_CONE VALUES ("", -171.5, 0, 1500, 0, 0, 0, "S", 3000, 0, 0, 10., 10., 0., 0.,-0.0056 ,0, "VACUUM", "ZERO_FIELD_APERTURE","");



CREATE TABLE QEX1B_BOX (
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

# Cut a hole in outer to place vacuum tube (so that field tube does not have to be a daughter ot QD_OUTER
# note that it is made longer than qd_outer, but it will be clipped when it is removed
INSERT INTO QEX1B_BOX VALUES ("QEX1B_OUTER", 0., 0., 0, 0., 0., 0.0, "S", 254, 254, 3100, 0., 0.0, 0, 0.0, "", "VACUUM", "SUBTRACT_REGION","SUBTRACT");

INSERT INTO QEX1B_BOX VALUES ("", 0., 0., 1500, 0., 0., 1.0, "I", 254, 254, 3000, 0., 0.0, 0,0.62727612684608661417e-2 , "QUAD", "VACUUM", "FIELD_REGION","");

