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
		string pregunta = "";
		string respuestaJugador = "";
		string envioAJugador = "";
		time_t starting_point;
		time_t now;
		for (var = 0; var <=4; ++var) {

			pregunta =preguntas[var].c_str();
			//envioAJugador = "Tienes 15 segundos para responder a la siguiente pregunta: ";
			(this->client)->send(envioAJugador + "*" + pregunta);

			/*
			 * Esto es un tiempo límite para el cliente: 15 segundos máximo para responder:
			 */
			//Pillamos el timestamp de ahora:
			time(&starting_point);//lo mismo que hacer now = time(NULL);
			now = starting_point; //inicializamos el tiempo de la vuelta.
			while( difftime(starting_point, now) <= 15 ){
				//Cada vez que pasemos una vuelta pillamos el tiempo y lo comparamos para
				//saber si han pasado 15 segundos
				respuestaJugador=(this->client)->receive();
				time(&now);
			}

			//Almacenamos la solución a la pregunta
			string sol =respuestas[var].c_str();

			//Si se ha pasado el tiempo... OOOOH!
			if( difftime(starting_point, now) > 15 ){
				envioAJugador="";
				falladas++;
				envioAJugador = "SE TE HA AGOTADO EL TIEMPO. La respuesta correcta era: " + sol + ".";
			}else{
				if (!respuestaJugador.compare(respuestas[var].c_str())){
					envioAJugador="RESPUESTA CORRECTA!!";
					acertadas++;
				}else{
					envioAJugador="";
					falladas++;
					envioAJugador = "HAS FALLADO. La respuesta correcta era: " + sol + ".";
				}
			}
			(this->client)->send(envioAJugador);
		}
		stringstream stream,stream2;
		stream << falladas;
		stream2 << acertadas;
		envioAJugador = "PREGUNTAS ACERTADAS: " + stream2.str() + " * PREGUNTAS FALLADAS: " + stream.str();
		(this->client)->send("EL JUEGO HA ACABADO" + envioAJugador);
		(this->client)->close();
	}
}
