//MOD (6)
//CLIENT IS GOING TO RECEIVE EACH SECOND A TIME FROM SERVER
#include "../util/TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

extern "C" {
	#include <sys/time.h>
}

void usage() {
	cout << "Usage: DsmTimeClient <name-server-port> <dsm-domain> " << endl;
	exit(1);
}

int main(int argc, char** argv) {
	if (argc > 3) {
		usage();
	}
	int port=atoi(argv[1]);
	char * domain =argv[2];

	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver("127.0.0.1", port, domain);
	PracticaCaso::DsmData time;

	while (true) {
        bool waitTime = false;
        while (!waitTime) {
            try {
            	//WE GET FROM GLOBAL_TIMESTAMP VARIBLE
            	time = driver->dsm_get("GLOBAL_TIMESTAMP");
                waitTime = true;
            } catch (DsmException & dsme) {
                cerr << "ERROR: dsm_get(\"GLOBAL_TIMESTAMP\"): " << dsme << endl;
                driver->dsm_wait("GLOBAL_TIMESTAMP");
            }
        }
        char* time2;
        time2 = ((char *)time.addr);
        cout << "New time received: " <<time2 << endl;
        //NOW WE WAIT 1 SECOND TO NEXT READING
        sleep(1);
    }

}
