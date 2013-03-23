#include "PasapalabraServerThread.h"
#include <stdio.h>
#include <stdlib.h>
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
		respuestas[0]="Ave";

		preguntas[1]="EMPIEZA POR LA B ... Medio de transporte que navega por el mar.";
		respuestas[1]="Barco";

		preguntas[2]="EMPIEZA POR LA C ... Articulacion situada entre el brazo y antebrazo.";
		respuestas[2]="Codo";

		preguntas[3]="EMPIEZA POR LA D ... Tenemos 5 en cada mano (singular).";
		respuestas[3]="Dedo";

		preguntas[4]="EMPIEZA POR LA E ... Desviacion del alineamiento de un ojo respecto al otro.";
		respuestas[4]="Estrabismo";

		int acertadas=0;
		int falladas=0;
		int var;
		string solucion="";

		for (var = 0; var <=4; ++var) {
			string pregunta =preguntas[var].c_str();
			(this->client)->send(pregunta);
			string respuesta=(this->client)->receive();

			if (!respuesta.compare(respuestas[var].c_str())){
				solucion="RESPUESTA CORRECTA!!";
				acertadas++;
				(this->client)->send(solucion);
			}else{
				solucion="";
				falladas++;
				string sol =respuestas[var].c_str();
				solucion = "HAS FALLADO. La respuesta correcta era: "+sol+ ".";
				(this->client)->send(solucion);
			}
		}
		(this->client)->send("EL JUEGO HA ACABADO");
		solucion="ACERTADAS: ** FALLADAS: ";
		(this->client)->send(solucion);
		(this->client)->close();
	}
}
