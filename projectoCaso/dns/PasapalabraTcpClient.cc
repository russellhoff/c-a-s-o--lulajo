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
	int var;
	for (var = 0; var <=4; ++var) {
		pregunta = client->receive();
		cout << endl;
		cout << pregunta << endl;
		cout << "Respuesta: ";
		cin >> respuesta;
		client->send(respuesta);
	}
	pregunta = client->receive();
	int final=pregunta.find("EL JUEGO HA ACABADO");
	cout << endl;
	cout << pregunta.substr(0,final)<< endl;
	cout << endl;
	cout <<"------------- EL JUEGO HA ACABADO --------------"<< endl;
	cout << endl;

	cout << pregunta.substr(final+19,pregunta.length()) << endl;

	client->close();
	delete client;
}
