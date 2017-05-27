/*
 * Database.cpp
 *
 *  Created on: May 9, 2017
 *      Author: sabbat
 */

#include "Database.h"

Database::Database(string db_path)
{
	try
	{
		auto db_ptr = db.get();
		auto result = sqlite3_open(db_path.c_str(), &db_ptr);
		db.reset(db_ptr, sqlite3_close );
		if (result != SQLITE_OK) throw 1; // TODO разобраться с этим
		clog << "Database successfully " << db_path << " opened\n";
	}
	catch(int)
	{
		cerr << "Can't open database " << db_path << endl;
	}
}

void Database::createTable()
{
	char *zErrMsg = 0;
	/* Create SQL statement */
	string sql = "CREATE TABLE COMPANY("  \
	      "ID INT PRIMARY KEY     NOT NULL," \
	      "NAME           TEXT    NOT NULL," \
	      "AGE            INT     NOT NULL," \
	      "ADDRESS        CHAR(50)," \
	      "SALARY         REAL );";

	/* Execute SQL statement */
	try
	{
		auto result = sqlite3_exec(db.get(), sql.c_str(), callback, 0, &zErrMsg);
		if (result != SQLITE_OK) throw 1;
	}
	catch(int)
	{
		cerr << "Can't create table\n";
	}
}

Database::~Database()
{
	try
	{
		auto result = sqlite3_close(db.get());
		if (result != SQLITE_OK) throw 1;
		clog << "Database successfully closed\n";
	}
	catch(int)
	{
		cerr << "Database closing error\n";
	}
}

int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}
