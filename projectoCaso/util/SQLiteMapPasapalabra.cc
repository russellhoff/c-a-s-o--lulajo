// SQLiteMapPasapalabra.cc
// author: dipina@eside.deusto.es
// compile: g++ SQLiteMapPasapalabra.cc -lsqlite3 -o SQLiteMapPasapalabra
#include "SQLiteMapPasapalabra.h"
#include <cstring>
#include <string.h>

namespace PracticaCaso {

	SQLiteMapPasapalabra::SQLiteMapPasapalabra(string fn): fileName(fn), dbh(0) {
		// Process the contents of the mapping file
		this->loadMappings(fn);
	}

	SQLiteMapPasapalabra::SQLiteMapPasapalabra(const SQLiteMapPasapalabra& rhs) {
		fileName = rhs.fileName;
		dbh = rhs.dbh;
		this->loadMappings(fileName);
	}

	SQLiteMapPasapalabra::~SQLiteMapPasapalabra() {
		cout << "SQLiteMap: destructor called " << endl;
		this->close();
	}

	void SQLiteMapPasapalabra::loadMappings(string mappingsDBFileName) {

		//TENEMOS QUE ABRIR LA BD ANTES DE COMPROBAR SI
		//EXISTE LA TABLA HAY K ABRIRLA EN 'dbh' -->
		cout << "Opening BD ..." << endl;
		if (sqlite3_open(mappingsDBFileName.c_str(), &dbh) != SQLITE_OK) {
			cerr << sqlite3_errmsg(dbh) << endl;
			sqlite3_close(dbh);
			exit(1);
		}

		char **result;
		int nrow;
		int ncol;
		char *errorMsg;

		//COMPROBAMOS SI LA TABLA EXISTE
		cout << "Checking if PreguntasPasapalabra table already exists ..." << endl;
		if (sqlite3_get_table(dbh, "select * from PreguntasPasapalabra", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
		  cerr << errorMsg << endl;
		  sqlite3_free(errorMsg);
		  //SI LA TABLA NO EXISTE LA CREAMOS
		  if (sqlite3_get_table(dbh, "create table PreguntasPasapalabra(id INTEGER NOT NULL PRIMARY KEY, suletra CHAR NOT NULL, comienza BOOLEAN NOT NULL, pregunta VARCHAR(150) NOT NULL, respuesta VARCHAR(150) NOT NULL)", &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
			  cerr << errorMsg << endl;
			  sqlite3_free(errorMsg);
			  this->close();
		  }else{
			  cout << "Tabla de preguntas de pasapalabra creada" << endl;
		  }
		  cout << "Table preguntas pasapalabra created" << endl;
		  sqlite3_free_table(result);
		  return;

		}

		//cerramos la BD
		sqlite3_free_table(result);
	}

	/**
	 * Esta funcion devolvera un par aleatorio de pregunta/respuesta dada una letra.
	 */
	void SQLiteMapPasapalabra::get(string letra, bool *comienza, string *pregunta, string *respuesta) {
		cout << "Opening BD to look for a question" << endl;
		if (sqlite3_open(fileName.c_str(), &dbh) != SQLITE_OK) {
			cerr << sqlite3_errmsg(dbh) << endl;
			sqlite3_close(dbh);
			exit(1);
		}

		char **result;
		int nrow = 0;
		int ncol = 0;
		char *errorMsg;

		//cogemos los datos de la tabla
		//cout << "Hacemos la consulta" << endl;
		string query = "SELECT * FROM PreguntasPasapalabra WHERE suletra='" + letra + "' ORDER BY RANDOM () LIMIT 1;";
		if (sqlite3_get_table(dbh, query.c_str(), &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
			cerr << errorMsg << endl;
			sqlite3_free(errorMsg);
			return;
		}

		string str = result[7];
		transform(str.begin(), str.end(), str.begin(), ::tolower);
		istringstream is(str);
		is >> std::boolalpha >> *comienza;

		//cout << "Esto contiene si comienza o no?" << result[7] << endl;
		*pregunta = result[8];
		*respuesta = result[9];

		//cerramos la BD
		sqlite3_free_table(result);

	}

	void SQLiteMapPasapalabra::set(string letra, bool comienza, string pregunta, string respuesta) {
		char **result;
		int nrow, ncol;
		char *errorMsg;

		string auxComienza = "";

		if(comienza)
			auxComienza = "true";
		else
			auxComienza = "false";

		string query = "INSERT INTO PreguntasPasapalabra(id, suletra, comienza, pregunta, respuesta) VALUES (NULL, "+letra+", "+auxComienza+", "+pregunta+", "+respuesta+")";

		if (sqlite3_get_table(dbh, query.c_str(), &result, &nrow, &ncol, &errorMsg) != SQLITE_OK) {
			cout << "fallo1" << endl;
			cerr << errorMsg << endl;
			sqlite3_free(errorMsg);
			sqlite3_close(dbh);
			exit(1);
		}else{
			cout << "Datos insertados. " << endl;

		}

		cout << "Datos insertados." << endl;
		sqlite3_free_table(result);//libera la memoria correctamente
		return;

	}

	void SQLiteMapPasapalabra::close() {
		sqlite3_close(dbh);
	}

	ostream & operator << (ostream & os, SQLiteMapPasapalabra &t) {
		os << "DB file name: " << t.fileName << endl;
		return os;
	}
}
