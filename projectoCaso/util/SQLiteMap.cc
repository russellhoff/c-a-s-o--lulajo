// SQLiteMap.cc
// author: dipina@eside.deusto.es
// compile: g++ SQLiteMap.cc -lsqlite3 -o SQLiteMap
#include "SQLiteMap.h"
#include <cstring>
#include <string>

namespace PracticaCaso {
	sqlite3 *dbh;
	string fileName;
	map<string, string> dns2IpPortMap;

	SQLiteMap::SQLiteMap(string fn): fileName(fn), dbh(0) {
		// Process the contents of the mapping file
		this->loadMappings(fn);
	}

	SQLiteMap::SQLiteMap(const SQLiteMap& rhs) {
		fileName = rhs.fileName;
		dbh = rhs.dbh;
		this->loadMappings(fileName);
	}

	SQLiteMap::~SQLiteMap() {
		cout << "SQLiteMap: destructor called " << endl;
		this->close();
	}

	void SQLiteMap::loadMappings(string mappingsDBFileName) {
		// Loads the mappings stored at SQLite DB into the map loadMappings
		// In the case that the DB does not exist, create it, its structure is given by file KeyValueDB.sql
		// If a select * from KeyValuePair executed through a sqlite3_get_table does not return SQLITE_OK, it means that the table does not exist, and needs being created
		// If there are unexpected error exit the program with exit(1)
		fileName=mappingsDBFileName;
		char **result;
		int nrow;
		int ncol;
		char *errorMsg;
		cout << "Checking if KeyValuePair table already exists ..." << endl;
		if (sqlite3_get_table(dbh, "select * from KeyValuePair", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
		  cerr << errorMsg << endl;
		  sqlite3_free(errorMsg);
		  if (sqlite3_get_table(dbh, "create table KeyValuePair(key_element BLOB NOT NULL PRIMARY KEY, value_element BLOB)", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
			  cerr << errorMsg << endl;
			  sqlite3_free(errorMsg);
			  sqlite3_close(dbh);
			  exit(1);
		  } else {
			  cout << "Table KeyValuePair created" << endl;
			  sqlite3_free_table(result);
		  }
		}
		for(int i =0; i < (nrow+1) * (ncol); i++) {
			string key =result[i][0];
			string value =result[i][1];
			this->set(key,value);
		}
	}

	map<string, string> SQLiteMap::getMap() {
		return dns2IpPortMap;
	}


	string SQLiteMap::get(string key) {
		return dns2IpPortMap [key];
	}

	void SQLiteMap::set(string mapKey, string mapValue) {
		dns2IpPortMap[mapKey]=mapValue;
	}


	void SQLiteMap::close() {
		 sqlite3_close(db);
	}

	ostream & operator << (ostream & os, SQLiteMap &t) {
		os << "DB file name: " << t.fileName << endl;
		os << "DNS mappings:" << endl;
		typedef map<string, string>::const_iterator CI;
		for (CI p = t.dns2IpPortMap.begin(); p != t.dns2IpPortMap.end(); ++p) {
			os <<  p->first << " : " << p->second << endl;
		}
		return os;
	}
}


/*
// global variable
PracticaCaso::SQLiteMap * SQLiteMap_pointer;

// function called when CTRL-C is pressed
void ctrl_c(int)
{
    printf("\nCTRL-C was pressed...\n");
	delete SQLiteMap_pointer;
}


void usage() {
	cout << "Usage: SQLiteMap <name-mappings-db-file>" << endl;
	exit(1);
}

int main(int argc, char** argv) {
	signal(SIGINT,ctrl_c);

	if (argc != 2) {
		usage();
	}

	PracticaCaso::SQLiteMap * SQLiteMap = new PracticaCaso::SQLiteMap((string)argv[1]);
	cout << "SQLiteMap instance: " << endl << SQLiteMap << endl;
	//SQLiteMap_pointer = &SQLiteMap;
	SQLiteMap_pointer = SQLiteMap;

	//SQLiteMap.set("saludo", "hola");
	//SQLiteMap.set("despedida", "adios");
	SQLiteMap->set("saludo", "hola");
	SQLiteMap->set("despedida", "adios");

	//cout << "SQLiteMap[\"saludo\"] = " << SQLiteMap.get("saludo") << endl;
	//cout << "SQLiteMap[\"despedida\"] = " << SQLiteMap.get("despedida") << endl;
	cout << "SQLiteMap[\"saludo\"] = " << SQLiteMap->get("saludo") << endl;
	cout << "SQLiteMap[\"despedida\"] = " << SQLiteMap->get("despedida") << endl;

	cout << "SQLiteMap instance after 2 sets: " << endl << *SQLiteMap << endl;

	//map<string, string> savedSQLiteMap = SQLiteMap.getMap();
	map<string, string> savedSQLiteMap = SQLiteMap->getMap();
	cout << "Recuperado el mapa" << endl;
	typedef map<string, string>::const_iterator CI;
	for (CI p = savedSQLiteMap.begin(); p != savedSQLiteMap.end(); ++p) {
		cout << "dns2IpPortMap[" << p->first << "] = " << p->second << endl;
	}
	
	delete SQLiteMap_pointer;
}
*/

