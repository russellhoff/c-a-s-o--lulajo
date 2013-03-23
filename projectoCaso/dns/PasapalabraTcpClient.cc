// EchoTcpClient.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"

void usage() {
	cout << "Usage: PasapalabraTcpClient <puerto>" << endl;
	exit(1);
}

int main(int argc, char** argv) {

	if (argc != 2) {
		usage();
	}

	PracticaCaso::TcpClient client;
	client.connect("127.0.0.1", atoi(argv[1]));

	bool acabado=false;
	string pregunta;
	string respuesta;
	while (!acabado){
		cout << "LLEGO" <<endl;
		pregunta = client.receive();
		cout << pregunta <<endl;
		if(strcmp(pregunta.c_str(),"EL JUEGO HA ACABADO")){
			acabado=true;
		}else{
			cout << pregunta << endl;
			cin >> respuesta;
			client.send(respuesta);
		}
	}

	client.close();
}
