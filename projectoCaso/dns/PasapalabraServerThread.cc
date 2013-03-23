#include "PasapalabraServerThread.h"

namespace PracticaCaso
{
	PasapalabraServerThread::~PasapalabraServerThread() {
		delete this->client;
	}

	void PasapalabraServerThread::run() {
		string preguntas[4];
		string respuestas[4];
		bool respondidas[4];

		//INICIALIZAMOS LOS ARRAYS
		preguntas[0]="EMPIEZA POR LA A ... Tipo de animal que es un gorrión.";
		respuestas[0]="Ave";
		respondidas[0]=false;

		preguntas[1]="EMPIEZA POR LA B ... ¿Qué navega por el mar?.";
		respuestas[1]="Barco";
		respondidas[1]=false;

		preguntas[2]="EMPIEZA POR LA C ... Articulación situada entre el brazo y antebrazo.";
		respuestas[2]="Codo";
		respondidas[2]=false;


		preguntas[3]="EMPIEZA POR LA D ... Singular. Tenemos 5 en cada mano.";
		respuestas[3]="Dedo";
		respondidas[3]=false;

		preguntas[4]="EMPIEZA POR LA E ... Desviación del alineamiento de un ojo respecto al otro.";
		respuestas[4]="Estrabismo";
		respondidas[4]=false;

		//ENVIAMOS LA PREGUNTA
		int acertadas=0;
		int falladas=0;
		int var;
		string solucion="";
		cout << "FUNCIONA" <<endl;


		for (var = 0; var <=4; ++var) {
			(this->client)->send(preguntas[var]);
			cout << "PREGUNTA ENVIADA" << var << endl;
			string respuesta=(this->client)->receive();
			cout <<"RESPUESTA RECIBIDA" << var << endl;

			if (respuesta.compare(respuestas[var].c_str())){
				solucion="HAS ACERTADO";
				acertadas++;
				(this->client)->send(solucion);
			}else{
				solucion="";
				falladas++;
				solucion = "HAS FALLADO. La respuesta correcta era: " +respuestas[var];
				(this->client)->send(solucion);
			}
		}

		(this->client)->close();

	}
}
