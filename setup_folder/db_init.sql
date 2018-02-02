CREATE TABLE Olympic(
    id             SERIAL PRIMARY KEY,
    name           varchar(64) NOT NULL unique,
    description    text DEFAULT ('No description') NOT NULL,
    start_time     TIMESTAMP,
    end_time       TIMESTAMP
);

CREATE TABLE Exercise(
    id             SERIAL PRIMARY KEY,
    ol_id          INTEGER  NOT NULL  REFERENCES Olympic(id) ON DELETE CASCADE,
    title          varchar(256) NOT NULL,
    content        text         NOT NULL
);

CREATE TABLE PgLang(
    id             SERIAL PRIMARY KEY,
    name           varchar(64) UNIQUE,
    cmd            varchar(128),
    ext            varchar(20)
);

CREATE TABLE olympic_lang(
    ol_id          INTEGER REFERENCES Olympic(id) ON DELETE CASCADE,
    lang_id        INTEGER REFERENCES PgLang(id) ON DELETE CASCADE,
    CONSTRAINT olympic_lang_pkey PRIMARY KEY (ol_id, lang_id)
);

CREATE TABLE AppUsr(
    id             SERIAL PRIMARY KEY,
    username       varchar(24) NOT NULL unique,
    pass           CHAR(40) NOT NULL,
    realName       varchar(64),
    email          varchar(64),
    birth_date     DATE, --CHECK (birth_date > '1940-01-01'),
    university     varchar(128),
    usrRole        INTEGER NOT NULL   -- 0 - user, 1 - moderator, 2 - admin
);

CREATE TABLE Participant(
    usr_id         INTEGER REFERENCES AppUsr(id) ON DELETE CASCADE,
    ol_id          INTEGER REFERENCES Olympic(id) ON DELETE CASCADE,
    CONSTRAINT participant_pkey PRIMARY KEY (ol_id, usr_id)
);

CREATE TABLE Exercise_points(
    ex_id          INTEGER REFERENCES Exercise(id) ON DELETE CASCADE,
    usr_id         INTEGER REFERENCES AppUsr(id) ON DELETE CASCADE,
    points         INTEGER,
    submit_time    TIMESTAMP,
    CONSTRAINT ex_points_pkey PRIMARY KEY (ex_id, usr_id)
);

CREATE VIEW STALE_OLYMPICS AS
SELECT * from olympic where now() > end_time;

insert INTO APPUSR(username, pass, realName, email, birth_date, university, usrrole)
        values('admin', 'admin', 'Super admin', 'Joc@joc.com',DATE '1990-12-12', 'UAIC', 2);