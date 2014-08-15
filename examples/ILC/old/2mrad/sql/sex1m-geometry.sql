# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS SEX1;
CREATE DATABASE SEX1;
USE SEX1;

CREATE TABLE SEX1_CONE (
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
INSERT INTO SEX1_CONE VALUES ("", 0, 0, 0., 0.4, 0.6, 0.2, "S", 2000, 0, 0, 500, 500, 0., 0., 0.,1, "IRON", "SEX1_OUTER","");



# Cut a hole in outer to place vacuum tube (so that vacuum tube does not have to be a daughter ot 
# note that it is made longer than qd_outer, but it will be clipped when it is removed

# photon beampipe
INSERT INTO SEX1_CONE VALUES ("SEX1_OUTER", -181., 0,0, 1, 1, 0, "I", 2001, 0, 0, 10.., 10.., 0., 0.,-0.0052 ,0, "VACUUM", "SUBTRACTING_TUBE","SUBTRACT");

INSERT INTO SEX1_CONE VALUES ("", -181., 0, 1000, 0, 0, 0, "S", 2000, 0, 0, 10., 10., 0., 0.,-0.0052 , 0,"VACUUM", "ZERO_FIELD_APERTURE","");



CREATE TABLE SEX1_BOX (
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
    K2		  DOUBLE(10,3), #
    MAGTYPE	  VARCHAR(10,3), #
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32),   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
    INHERITSTYLE  VARCHAR(32)
);

# Cut a hole in outer to place vacuum tube (so that field tube does not have to be a daughter ot 
# note that it is made longer than qd_outer, but it will be clipped when it is removed
INSERT INTO SEX1_BOX VALUES ("SEX1_OUTER", 0., 0., 0, 0., 0., 0.0, "S", 270, 270, 2010, 0., 0.0, 0, 0.0, "", "VACUUM", "SUBTRACT_REGION","SUBTRACT");

#INSERT INTO SEX1_BOX VALUES ("", 0., 0., 1000, 0., 0., 1.0, "I", 270, 270, 2000, 0., 0.0, 0,0.62727612684608661417e-2 , "SEXT", "VACUUM", "FIELD_REGION","");

# sextupole disabled
INSERT INTO SEX1_BOX VALUES ("", 0., 0., 1000, 0., 0., 1.0, "I", 270, 270, 2000, 0., 0.0, 0,0. , "SEXT", "VACUUM", "FIELD_REGION","");
