/*
 * Database.h
 *
 *  Created on: May 9, 2017
 *      Author: sabbat
 */

#ifndef DATABASE_H_
#define DATABASE_H_

#include <iostream>
#include <string>
#include <memory>
#include <sqlite3.h>

using namespace std;

class Database {
public:
	Database(string db_path);
	void createTable();
	virtual ~Database();
private:
	shared_ptr<sqlite3> db;
};

int callback(void *NotUsed, int argc, char **argv, char **azColName);

#endif /* DATABASE_H_ */
