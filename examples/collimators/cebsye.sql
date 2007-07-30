# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS

DROP DATABASE IF EXISTS CEBSYE;
CREATE DATABASE CEBSYE;
USE CEBSYE;

CREATE TABLE cebsye_box (
		PARENTNAME		VARCHAR(32),
    PosX          DOUBLE(10,3), #
    PosY          DOUBLE(10,3), #
    PosZ          DOUBLE(10,3), #
    Red           DOUBLE(10,3), #
    Blue          DOUBLE(10,3), #
    Green         DOUBLE(10,3), #
    VisAtt        VARCHAR(32),  # i = invis, s = solid, w = wireframe
    LengthX       DOUBLE(10,3), #
    LengthY       DOUBLE(10,3), #
    LengthZ       DOUBLE(10,3), #
    Material      VARCHAR(32),  # material, CGA literal name
    Name          VARCHAR(32),   # name of solid, logical, and physical volume
		SETSENSITIVE  INTEGER(11)      
);

INSERT INTO cebsye_box VALUES ("",0.0, 0.0, 1500, 0.7, 0.5, 0.0, "w", 1200.0, 
1200.0, 3000.0, "TitaniumAlloy","cebsye_outer",1);


INSERT INTO cebsye_box VALUES ("cebsye_outer",0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "w", 120.0,120.0, 3000.0, "Vacuum","cebsye_inner",0);




