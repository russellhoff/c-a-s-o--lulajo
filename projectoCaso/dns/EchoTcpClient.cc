// EchoTcpClient.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"

int main() {
/*
	// MODIFICATION 2.3.3
		PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
		client->connect("127.0.0.1", 4321);
		string msg = "¡Hello CASO students!";
		client->send(msg);
		cout << "Message sent: " << msg << endl;
		msg = client->receive();
		cout << "Message received: " << msg << endl;
		client->close();	
		delete client;
*/
	// MODIFICATION 2.3.5
	for (int i=0; i<100; i++) {
		PracticaCaso::TcpClient * client = new PracticaCaso::TcpClient();
		cout << "Petición: " << i << endl;
		client->connect("127.0.0.1", 4321);
		string msg = "¡Hello CASO students!";
		client->send(msg);
		cout << "Message sent: " << msg << endl;
		msg = client->receive();
		cout << "Message received: " << msg << endl;
		client->close();	
		delete client;
	}
}

