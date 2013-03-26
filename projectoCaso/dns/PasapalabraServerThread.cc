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
		string letras[5] = {"a","b","c","d","e"};
		//string letras[25] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","ñ","o","p","q","r","s","t","u","v","w","x","y","z"};


		int acertadas=0;
		int falladas=0;
		int var;
		bool comienza;
		string pregunta;
		string solucion = "";
		string sol = "";
		string acertado = "";
		for (var = 0; var < cant_letras; ++var) {
			sqliteMapPasapalabra->get(letras[var], &comienza, &pregunta, &solucion);
			string envio = "";
			if(comienza){
				envio = "Comienza con la letra " + letras[var] + ": " + pregunta;
			}else{
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

		(this->client)->send(acertado);
		(this->client)->receive();

		stringstream stream,stream2;
		stream << falladas;
		stream2 << acertadas;
		solucion="PREGUNTAS ACERTADAS: "+stream2.str()+" * PREGUNTAS FALLADAS: "+stream.str();
		(this->client)->send("EL JUEGO HA ACABADO"+solucion);
		(this->client)->close();

	}
}
