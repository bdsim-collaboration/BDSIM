
# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXIST YOKE;
CREATE DATABASE YOKE;
USE YOKE;

CREATE TABLE MYYOKE_BOX (
    PARENTNAME    VARCHAR(32),  #
    INHERITSTYLE  VARCHAR(32),  #
    RED           DOUBLE(10,3), #
    GREEN         DOUBLE(10,3), #
    BLUE          DOUBLE(10,3), #
    ALPHA         DOUBLE(10,3), #
    VISATT        VARCHAR(32),  # I = INVISIBLE, S = SOLID, W = WIREFRAME
    POSX          DOUBLE(10,3), #
    POSY          DOUBLE(10,3), #
    POSZ          DOUBLE(10,3), #
    LENGTHX       DOUBLE(10,3), #
    LENGTHY       DOUBLE(10,3), #
    LENGTHZ       DOUBLE(10,3), #
    MATERIAL      VARCHAR(32),  # MATERIAL, CGA LITERAL NAME
    NAME          VARCHAR(32),   # NAME OF SOLID, LOGICAL, AND PHYSICAL VOLUME
    SETSENSITIVE  INTEGER(11)
);

#Place iron yoke
INSERT INTO MYYOKE_BOX VALUES ("","",0.0,1.0,0.0,0.4,"W",0.0,0.0,858.0,1740.0,1160.0,1000,"Iron","YOKE_BOX",1);

#Subtract a cavity
INSERT INTO MYYOKE_BOX VALUES ("YOKE_BOX","SUBTRACT",0.0,1.0,0.0,0.4,"I",0.0,0.0,0.0,1130.0,520.0,1000,"air","YOKE_CAVITY",1);

