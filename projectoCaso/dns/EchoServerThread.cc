// EchoServerThread.cc
// author: dipina@eside.deusto.es

/*
 * para cualquier servidor multihilo: copiar, cambiar nombre y ya esta. Solo sobreescribir el metodo run()
 */
#include "EchoServerThread.h"

namespace PracticaCaso
{
	EchoServerThread::~EchoServerThread() {
		delete this->client;
	}

	void EchoServerThread::run() {
		// make the type casting and recuperate the parameters using "arg"
		string msg = (this->client)->receive();
		cout << "Message received: " << msg << endl;
		(this->client)->send(msg);
		cout << "Message sent: " << msg << endl;
		(this->client)->close();
	}
}
