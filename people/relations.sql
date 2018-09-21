DROP SEQUENCE relid;
DROP TABLE relations;

CREATE SEQUENCE "relid" start 1 minvalue 1;
CREATE TABLE relations
(
   rel_id	int	DEFAULT nextval('relid'::text)
			UNIQUE
			NOT NULL
			PRIMARY KEY,
   rel_name	varchar(20) NOT NULL
);

INSERT INTO relations VALUES( 1, 'husband');
INSERT INTO relations VALUES( 2, 'wife');
INSERT INTO relations VALUES( 3, 'small brother');
INSERT INTO relations VALUES( 4, 'old brother');
INSERT INTO relations VALUES( 5, 'small sister');
INSERT INTO relations VALUES( 6, 'old sister');
INSERT INTO relations VALUES( 7, 'small cousin');
INSERT INTO relations VALUES( 8, 'old cousin');
INSERT INTO relations VALUES( 9, 'uncle');
INSERT INTO relations VALUES(10, 'aunt');
INSERT INTO relations VALUES(11, 'father');
INSERT INTO relations VALUES(12, 'mother');
INSERT INTO relations VALUES(13, 'grandmother');
INSERT INTO relations VALUES(14, 'grandfather');
INSERT INTO relations VALUES(15, 'son');
INSERT INTO relations VALUES(16, 'daughter');
INSERT INTO relations VALUES(17, 'grandson');
INSERT INTO relations VALUES(18, 'granddaughter');

SELECT SETVAL('relid', 19);


DROP TABLE person_relation;
CREATE TABLE person_relation
(
   person_1	int	NOT NULL,
   person_2	int	NOT NULL,
   relation	int	NOT NULL
);

CREATE INDEX pel_idx ON person_relation (person_1);
