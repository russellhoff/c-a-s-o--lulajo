// PasapalabraTcpClient.cc

#include "TcpListener.h"

void usage() {
	cout << "Usage: PasapalabraTcpClient <puerto>" << endl;
	exit(1);
}

int main() {
	PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
	client->connect("127.0.0.1", 4321);

	bool acabado=false;
	string pregunta;
	string respuesta;
	while (!acabado)	{
		pregunta = client->receive();
		if (!strcmp(pregunta.c_str(),"EL JUEGO HA ACABADO")) {
			acabado = true;
		} else {
			cout << endl;
			cout << pregunta << endl;
			cout << "Respuesta: ";
			cin >> respuesta;
			client->send(respuesta);
		}
	}
	client->close();
	delete client;
}
