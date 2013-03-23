#include "PasapalabraServerThread.h"
#include <sstream>
#include <string.h>

namespace PracticaCaso
{
	PasapalabraServerThread::~PasapalabraServerThread() {
		delete this->client;
	}

	void PasapalabraServerThread::run() {
		string preguntas[5];
		string respuestas[5];

		//INICIALIZAMOS LOS ARRAYS
		preguntas[0]="EMPIEZA POR LA A ... Tipo de animal que es una gaviota.";
		respuestas[0]="AVE";

		preguntas[1]="EMPIEZA POR LA B ... Medio de transporte que navega por el mar.";
		respuestas[1]="BARCO";

		preguntas[2]="EMPIEZA POR LA C ... Articulacion situada entre el brazo y antebrazo.";
		respuestas[2]="CODO";

		preguntas[3]="EMPIEZA POR LA D ... Tenemos 5 en cada mano (singular).";
		respuestas[3]="DEDO";

		preguntas[4]="EMPIEZA POR LA E ... Desviacion del alineamiento de un ojo respecto al otro.";
		respuestas[4]="ESTRABISMO";

		int acertadas=0;
		int falladas=0;
		int var;
		string solucion="";
		for (var = 0; var <=4; ++var) {
			string pregunta =preguntas[var].c_str();
			(this->client)->send(solucion+"*"+pregunta);
			string respuesta=(this->client)->receive();
			if (!strcasecmp(respuestas[var].c_str(), respuesta.c_str())){
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
