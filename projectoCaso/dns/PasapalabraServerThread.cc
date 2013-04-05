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

		int cant_letras = 27;

		string letras[27] = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "Ñ", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};


		int acertadas=0;
		int falladas=0;
		int var;
		bool comienza;
		string pregunta;
		string solucion = "";
		string sol = "";
		string acertado = "";
		for (var = 0; var < cant_letras; var++) {
			sqliteMapPasapalabra->get(letras[var], &comienza, &pregunta, &solucion);
			string envio = "";

			if(comienza){
				cout << "****comienza"<< endl;
				envio = "Comienza con la letra " + letras[var] + ": " + pregunta;
			}else{
				cout << "****contiene"<< endl;
				envio = "Contiene la letra " + letras[var] + ": " + pregunta;
			}

			(this->client)->send(acertado+"*"+envio);
			string respuesta=(this->client)->receive();

			if (!strcasecmp(solucion.c_str(), respuesta.c_str())){

				acertado="RESPUESTA CORRECTA!!";
				acertadas++;

			}else{

				falladas++;
				string sol =solucion.c_str();
				acertado = "HAS FALLADO. La respuesta correcta era: "+sol+ ".";

			}

		}


		stringstream stream,stream2;
		stream << falladas;
		stream2 << acertadas;
		solucion="PREGUNTAS ACERTADAS: "+stream2.str()+" * PREGUNTAS FALLADAS: "+stream.str();
		(this->client)->send(acertado+"EL JUEGO HA ACABADO"+solucion);
		(this->client)->close();

	}
}
