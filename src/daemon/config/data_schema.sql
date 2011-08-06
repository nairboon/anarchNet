CREATE TABLE aplist (
id INTEGER PRIMARY KEY AUTOINCREMENT ,
name TEXT );

CREATE TABLE data (
key TEXT UNIQUE,
value TEXT ,
timestamp INTEGER ,
lastcheck INTEGER ,
app INTEGER ,
protocol INTEGER , public INTEGER DEFAULT '0');
