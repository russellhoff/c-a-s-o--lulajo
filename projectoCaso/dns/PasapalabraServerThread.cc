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
		/*int cont = 0;
		letras[0] = "A";
		letras[1] = "B";
		letras[2] = "C";
		letras[3] = "D";
		letras[4] = "E";
		letras[5] = "F";
		letras[6] = "G";
		letras[7] = "H";
		letras[8] = "I";
		letras[9] = "J";
		letras[10] = "K";
		letras[11] = "L";
		letras[12] = "M";
		letras[13] = "N";
		letras[14] = "Ñ";
		letras[15] = "O";
		letras[16] = "P";
		letras[17] = "Q";
		letras[18] = "R";
		letras[19] = "S";
		letras[20] = "T";
		letras[21] = "U";
		letras[22] = "V";
		letras[23] = "W";
		letras[24] = "X";
		letras[25] = "Y";
		letras[26] = "Z";*/


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
