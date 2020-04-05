DROP TABLE events;
DROP SEQUENCE event_id;

CREATE SEQUENCE "event_id" start 1 minvalue 1;
CREATE TABLE events
(
	event_id	smallint	DEFAULT nextval('event_id'::text)
					UNIQUE
					NOT NULL
					PRIMARY KEY,
	ownerid		int		NOT NULL,
	eventtime	timestamp without time zone
					NOT NULL
					DEFAULT "timestamp"('now'::text),
	alarmyn		smallint	NOT NULL
					DEFAULT 0,
	repeat		interval	NOT NULL
					DEFAULT "interval"(0),
	description	varchar(255)
);
