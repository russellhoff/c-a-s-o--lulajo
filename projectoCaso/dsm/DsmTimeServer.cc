//MOD (6)
//SERVER IS GOING TO SEND EACH SECOND A TIME FROM SERVER

#include "TcpListener.h"
#include "Dsm.h"

#define BUFFER_SIZE 1024

extern "C" {
	#include <sys/time.h>
}

void usage() { 
	cout << "Usage: DsmTimeServer <name-server-port>" << endl;
	exit(1);
}

int main(int argc, char** argv) {
	if (argc > 2) {
		usage(); 
	}
	int port =atoi(argv[1]);
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver("127.0.0.1", port, "dsm.deusto.es");

	char buffer[100];
	struct timeval tv;
	time_t curtime;
	
    try {
        driver->dsm_malloc("GLOBAL_TIMESTAMP", sizeof(buffer));
    } catch (DsmException & dsme) {
        cerr << "ERROR in dsm_malloc(\"GLOBAL_TIMESTAMP\", sizeof(" << sizeof(buffer) << ")): " << dsme << endl;
        exit(1);
    }

    while (true) {
		gettimeofday(&tv, NULL);
		curtime=tv.tv_sec;
	    strftime(buffer,100,"%d-%m-%Y, %H:%M:%S",localtime(&curtime));
		cout << "new system time: " << buffer << endl;
        try { 
        	//PUTTING TIME INTO VARIABLE GLOBAL_TIMESTAMP
            driver->dsm_put("GLOBAL_TIMESTAMP", (void *)buffer, sizeof(buffer));
        } catch (DsmException & dsme) {
            cerr << "ERROR: dsm_put(\"GLOBAL_TIMESTAMP\", buffer, " << sizeof(buffer) << ")): " << dsme << endl;
            driver->dsm_free("GLOBAL_TIMESTAMP");
            exit(1); 
        } 
        //WAIT FOR 1 SECOND TO SEND ANOTHER ONE
        sleep(1); 
	}
	driver->dsm_free("GLOBAL_TIMESTAMP");
}
