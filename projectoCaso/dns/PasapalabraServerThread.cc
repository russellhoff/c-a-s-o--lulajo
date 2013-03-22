#include "PasapalabraServerThread.h"

namespace PracticaCaso
{
	PasapalabraServerThread::~PasapalabraServerThread() {
		delete this->client;
	}

	PasapalabraServerThread::PasapalabraServerThread(TcpClient* c) {
		this->client =c;

	}

	void PasapalabraServerThread::run() {

		string preguntas[];
		string respuestas[];
		bool respondidas[];

		//INICIALIZAMOS LOS ARRAYS
		preguntas[0]="EMPIEZA POR LA A ... Tipo de animal que es un gorri�n.";
		respuestas[0]="Ave";
		respondidas[0]=false;

		preguntas[1]="EMPIEZA POR LA B ... �Qu� navega por el mar?.";
		respuestas[1]="Barco";
		respondidas[1]=false;

		preguntas[2]="EMPIEZA POR LA C ... Articulaci�n situada entre el brazo y antebrazo.";
		respuestas[2]="Codo";
		respondidas[2]=false;


		preguntas[3]="EMPIEZA POR LA D ... Singular. Tenemos 5 en cada mano.";
		respuestas[3]="Dedo";
		respondidas[3]=false;


		preguntas[4]="EMPIEZA POR LA E ... Desviaci�n del alineamiento de un ojo respecto al otro.";
		respuestas[4]="Estrabismo";
		respondidas[4]=false;

		//ENVIAMOS LA PREGUNTA
		int var;
		string solucion="";
		for (var = 0; var <=4; ++var) {
			(this->client)->send(preguntas[var]);
			cout << "PREGUNTA ENVIADA" << var << endl;
			string respuesta=(this->client)->receive();
			cout <<"RESPUESTA RECIBIDA" << var << endl;

			if (respuesta.compare(respuestas[var].c_str())){
				solucion="HAS ACERTADO";
				(this->client)->send(solucion);
			}else{
				solucion="";
				solucion << "HAS FALLADO. La respuesta correcta era: '" << respuestas[var] << "'";
				(this->client)->send(solucion);
			}
		}

		(this->client)->close();

	}
}
