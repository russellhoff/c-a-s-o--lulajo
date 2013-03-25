// EchoTcpListener.cc
// author: dipina@eside.deusto.es

#include "TcpListener.h"
#include "PasapalabraServerThread.h"

extern "C" {
	#include <signal.h>
}

PracticaCaso::TcpListener * listener_pointer;

void usage(){
	cout << "./PasapalabraTcpListener pasapalabra_db.db" << endl;
}

void ctrl_c(int)
{
    cout << "CTRL-C was pressed..." << endl;
	listener_pointer->stop();
}


int main(int argc, char** argv) {

	if( argc < 1 || argc > 1 ){
		usage();
	}

	signal(SIGINT,ctrl_c);
	PracticaCaso::TcpListener listener(4321);
	listener_pointer = &listener;
	cout << "TcpListener created: " << listener << endl;
	listener.start();

	while (true) {
		PracticaCaso::TcpClient* client = listener.acceptTcpClient();
		PracticaCaso::PasapalabraServerThread* t = new PracticaCaso::PasapalabraServerThread(client,string(argv[0]));
		t->start();
	}

	cout << "Finishing server ..." << endl;
	listener.stop();
}
