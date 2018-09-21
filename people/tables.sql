DROP SEQUENCE person_ID;
DROP TABLE persons;

CREATE SEQUENCE "person_id" start 1 minvalue 1;
CREATE TABLE persons
(
   person_id	int			DEFAULT nextval('person_id'::text)
					UNIQUE
					NOT NULL
					PRIMARY KEY,
   firstname	varchar(35),
   lastname	varchar(35),
   title	varchar(32),
   company	varchar(50),
   work_phone	varchar(20),
   home_phone	varchar(20),
   fax		varchar(20),
   other	varchar(20),
   email	varchar(32),
   address	varchar(70),
   city		varchar(32),
   state	varchar(5),
   zip		varchar(10),
   country	varchar(32),
   birthday	varchar(32),
   custom2	varchar(255),   
   custom3	varchar(255),   
   custom4	varchar(255),   
   note		text,
   privateyn	smallint		NOT NULL DEFAULT 0,
   category	varchar(32)
) WITH OIDs;
