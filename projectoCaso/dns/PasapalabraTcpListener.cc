// EchoTcpListener.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "PasapalabraServerThread.h"

extern "C" {
	#include <signal.h>
}

PracticaCaso::TcpListener * listener_pointer;

void usage(){
	cout << "./PasapalabraTcpListener pasapalabra_db" << endl;
}

void ctrl_c(int)
{
    cout << "CTRL-C was pressed..." << endl;
	listener_pointer->stop();
}


int main() {

	signal(SIGINT,ctrl_c);
	PracticaCaso::TcpListener listener(4321);
	listener_pointer = &listener;
	cout << "TcpListener created: " << listener << endl;
	listener.start();

	while (true) {
		PracticaCaso::TcpClient* client = listener.acceptTcpClient();
		PracticaCaso::PasapalabraServerThread* t = new PracticaCaso::PasapalabraServerThread(client,"pasapalabra_db.db");
		t->start();
	}

	cout << "Finishing server ..." << endl;
	listener.stop();

}
