# ALL LENGTHS MUST BE GIVEN IN MILLIMETRES
# ALL ROTATIONS MUST BE GIVEN IN RADIANS



DROP DATABASE IF EXISTS MY_FIELD;
CREATE DATABASE MY_FIELD;
USE MY_FIELD;








CREATE TABLE field_box (
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
    FieldX          DOUBLE(10,3), #
    FieldY          DOUBLE(10,3), #
    FieldZ          DOUBLE(10,3) #
);

                         


INSERT INTO field_box VALUES ("",0.0, 0.0, 500.0, 0.0, 0.0, 1.0, "s", 500.0, 
500.0, 1000.0, "vacuum", "my_field",0.0, 1.0, 0.0);


