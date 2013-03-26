#include "PasapalabraServerThread.h"
#include <sstream>
#include <string.h>

namespace PracticaCaso
{

	PasapalabraServerThread::PasapalabraServerThread(TcpClient * c, string f){
		client = c;
		sqliteMapPasapalabra = new SQLiteMapPasapalabra(f);
	}

	PasapalabraServerThread::PasapalabraServerThread(const PasapalabraServerThread& ns): client(ns.client) {
		sqliteMapPasapalabra = ns.sqliteMapPasapalabra;
	}

	PasapalabraServerThread::~PasapalabraServerThread() {
		delete this->client;
	}

	PasapalabraServerThread& PasapalabraServerThread::operator = (const PasapalabraServerThread& rhs) {
		// check for identity
		if (this == &rhs)
			return *this;
		client = rhs.client;
		sqliteMapPasapalabra = rhs.sqliteMapPasapalabra;
		return *this;
	}

	void PasapalabraServerThread::run() {

		int cant_letras = 5;

		string preguntas[cant_letras];
		string respuestas[cant_letras];
		string letras[cant_letras];
		letras[0] = "A";
		letras[1] = "B";
		letras[2] = "C";
		letras[3] = "D";
		letras[4] = "E";


		int acertadas=0;
		int falladas=0;
		int var;
		string aux = "";
		bool comienza;
		string pregunta;
		string solucion;
		for (var = 0; var < cant_letras; ++var) {

			sqliteMapPasapalabra->get(letras[var], &comienza, &pregunta, &solucion);

			if(comienza){
				pregunta = "Comienza con la letra " + letras[var] + ": " + pregunta;
			}else{
				pregunta = "Contiene la letra " + letras[var] + ": " + pregunta;
			}

			(this->client)->send(solucion+"*"+pregunta);
			string respuesta=(this->client)->receive();

			if (!strcasecmp(solucion.c_str(), respuesta.c_str())){

				solucion="RESPUESTA CORRECTA!!";
				acertadas++;

			}else{

				solucion="";
				falladas++;
				string sol =respuestas[var].c_str();
				solucion = "HAS FALLADO. La respuesta correcta era: "+sol+ ".";

			}

		}

		stringstream stream,stream2;
		stream << falladas;
		stream2 << acertadas;
		solucion="PREGUNTAS ACERTADAS: "+stream2.str()+" * PREGUNTAS FALLADAS: "+stream.str();
		(this->client)->send("EL JUEGO HA ACABADO"+solucion);
		(this->client)->close();

	}
}
