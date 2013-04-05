// SqliteMapPasapalabra.h
// author: dipina@eside.deusto.es
#ifndef __SQLITEMAPPASAPALABRA_H
#define __SQLITEMAPPASAPALABRA_H

//lo escrito en viene dentro de extern.
extern "C" {
	#include <sqlite3.h>	
}

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;

namespace PracticaCaso
{
	class SQLiteMapPasapalabra {
		private:
			sqlite3 *dbh;
			string fileName;

			void close();
			void loadMappings(string mappinsFileName);
		public:
			SQLiteMapPasapalabra(string fn);
			SQLiteMapPasapalabra(const SQLiteMapPasapalabra&);
			~SQLiteMapPasapalabra();

			/**
			 * Obtiene una pregunta de una determinada letra (de todas las preguntas para esa letra,
			 * obtiene una pregunta ALEATORIAMENTE)
			 */
			void get(string letra, bool *comienza, string *pregunta, string *respuesta);

			/**
			 * Anyade una pregunta con letra, comienza, pregunta y respuesta
			 */
			void set(string letra, bool comienza, string pregunta, string respuesta);
			friend ostream & operator << (ostream &os, SQLiteMapPasapalabra &t);
	};
};
#endif
