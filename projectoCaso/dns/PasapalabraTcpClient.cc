// PasapalabraTcpClient.cc

#include "TcpListener.h"

void usage() {
	cout << "Usage: PasapalabraTcpClient <puerto>" << endl;
	exit(1);
}

int main() {
	PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
	client->connect("127.0.0.1", 4321);
	cout << endl;
	cout << "***********************  BIENVENIDO A PASAPALABRA  ***********************" <<endl;
	cout << "--------------------------------------------------------------------------" <<endl;
	cout << endl;
	cout << "Se van a ofrecer varias definiciones y debes responder la palabra definida."<<endl;
	bool acabado=false;
	string mensaje;
	string pregunta;
	string solucion;
	string respuesta;
	int var;
	for (var = 0; var <=4; ++var) {
		mensaje = client->receive();
		pregunta = mensaje.substr(mensaje.find("*")+1, mensaje.length());
		solucion = mensaje.substr(0,mensaje.find("*"));
		cout << solucion <<endl;
		cout << endl;
		cout << pregunta << endl;
		cout << "Respuesta: ";
		cin >> respuesta;
		client->send(respuesta);
	}
	pregunta = client->receive();
	int final=pregunta.find("EL JUEGO HA ACABADO");
	cout << pregunta.substr(0,final)<< endl;
	cout << endl;
	cout << "**************************  EL JUEGO HA ACABADO ** ************************" <<endl;
	cout << "---------------------------------------------------------------------------" <<endl;	cout << endl;

	cout << pregunta.substr(final+19,pregunta.length()) << endl;

	client->close();
	delete client;
}
