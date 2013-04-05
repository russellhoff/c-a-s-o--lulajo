// Dsm.cc
// author: dipina@eside.deusto.es

#include "Dsm.h"
#include "Thread.h"
#include "TcpListener.h"
#define BUFFER_SIZE 1024

namespace PracticaCaso {
	DsmObserver::DsmObserver(DsmDriver *c): client(c), keepRunning(true) {
		struct ip_mreq mreq;
		
		u_int yes=1;           
		/* create what looks like an ordinary UDP socket */
		if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
			cerr << "socket" << endl;
			exit(1);
		}

		/* allow multiple sockets to use the same PORT number */
		if (setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
			cerr << "Reusing ADDR failed" << endl;
			exit(1);
		}


		/* set up destination address */
		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr.s_addr=htonl(INADDR_ANY); /* N.B.: differs from sender */
		addr.sin_port=htons(DSM_PORT);

		/* bind to receive address */
		if (bind(fd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
			cerr << "bind" << endl;
			exit(1);
		}

		/* use setsockopt() to request that the kernel join a multicast group */
		mreq.imr_multiaddr.s_addr=inet_addr(DSM_GROUP);
		mreq.imr_interface.s_addr=htonl(INADDR_ANY);
		if (setsockopt(fd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
			cerr << "setsockopt" << endl;
			exit(1);
		}
	}

	void DsmObserver::run() {
		int nbytes;
		socklen_t addrlen;
		char msgbuf[MSGBUFSIZE];
		memset(msgbuf,0,sizeof(msgbuf));
		/* now just enter a read-print loop */
		while (this->keepRunning) {
			addrlen=sizeof(addr);
			if ((nbytes=recvfrom(fd,msgbuf,MSGBUFSIZE,0,
					   (struct sockaddr *) &addr,&addrlen)) < 0) {
			   cerr << "recvfrom" << endl;
			   exit(1);
			}

			istringstream ins;
			ins.str(msgbuf);
			string command, blockId;
			int nid;
			ins >> command >> nid >> blockId;
			this->client->dsm_notify(command, blockId);
			memset(msgbuf,0,sizeof(msgbuf));
		}
	}

	void DsmObserver::stop() {
		this->keepRunning = false;
	}
	
	
	// MODIFICACI�N PR�CTICA DSM: descripci�n en 3.3.5 (punto 2):
	// Nueva signature constructor: DsmDriver(string ipAddressNameServer, int portNameServer, string dmsServerName2Lookup); 
	//DsmDriver::DsmDriver(string DSMServerIPaddress, int DSMServerPort) {
		//INITIALIZE NEW VARIABLES
		/* this->condition=NULL;
		 * This is the right way to initialize pthread_cond_t variable:
		 */
	/*	pthread_cond_init(&this->condition, NULL);
		/* this->mutex=NULL;
		 * The right way to pthread__mutex_t
		 */
	/*	pthread_mutex_init(&this->mutex, NULL);

		// Lookup pop.deusto.es in NameServer
		this->observer = new DsmObserver(this);
		this->observer->start();

		this->connect(DSMServerIPaddress, DSMServerPort);
		this->send("dsm_init");
		this->nid = atoi((this->receive()).c_str());

	}*/

	DsmDriver::DsmDriver(string ipAddressNameServer, int portNameServer, string dsmServerName2Lookup){

		int portDmsServer;
		string ipDmsServer, strPortDmsServer;



		this->observer = new DsmObserver(this);
		this->observer->start();

		/*
		 * Let's ask NameServer about Dms Server
		 */
		PracticaCaso::TcpClient *cli = new PracticaCaso::TcpClient(); //create TcpClient (we are a client of DnsServer)
		cli->connect(ipAddressNameServer, portNameServer); //Connect to the DnsServer
		cli->send(dsmServerName2Lookup); //Ask to DnsServer about Dms Server machine

		string answerFromDnsServer = cli->receive(); //store the answer from Dns Server
		cout << "***Respuesta al resolver " << ipAddressNameServer << " con el puerto " << portNameServer << ":::: " << answerFromDnsServer << endl;
		/*
		 * Let's test whether the Dns Server has sent us a valid address or,
		 * unfortunately, it has sent us an error message
		 */

		if( answerFromDnsServer.find("ERROR") == 0 ){
			cerr << "The address " << dsmServerName2Lookup << " could not be resolved by DNS Server." << answerFromDnsServer << endl;
			this->observer->stop();
			this->close();
		}else{
			cout << "Dns Server resolved the address " << dsmServerName2Lookup << "! It corresponds to " << answerFromDnsServer << endl;

			/*
			 * Let's divide the answerFromDnsServer, as it containt the IP and Port
			 * of Dms Server, we are gonna parse them: Ip as unique string and
			 * Port as int.
			 */
			answerFromDnsServer = answerFromDnsServer.replace( //replace character ":" to " "
					answerFromDnsServer.find(":", 0), //Index of first character (find first occurrence of ":")
															//to replace.
					1,								//Number of characters to be replaced.
					" "								//replacing ":" with " "
			);

			istringstream ins;
			ins.str( answerFromDnsServer );

			/*
			 * Let's split answerFromDnsServer's ip and port onto 2 strings:
			 */
			ins >> ipDmsServer >> strPortDmsServer;

			portDmsServer = atoi( strPortDmsServer.c_str() ); //Parse port as string into port as int.

			/*
			 * Initialize vars for 3.3.5 (3)
			 */
			pthread_mutex_init(&this->mutex, NULL);
			pthread_cond_init(&this->condition, NULL);
		}
		//Closing the connection to Dns Server
		cli->close();

		/*
		 * Let's connecto to Dsm Server and add nid to us.
		 */
		this->connect(ipDmsServer, portDmsServer);
		this->send("dsm_init"); //send the message to initialize our Dsm Client.

		string ans = this->receive(); //We will receive an answer from Dsm Server...
		//...which is the nid
		this->nid = atoi(ans.c_str());

		//ADDED FROM THE OTHER CONSTRUCTOR: INITIALIZE NEW VARIABLES
		/* this->condition=NULL;
		 * This is the right way to initialize pthread_cond_t variable:
		 */
		pthread_cond_init(&condition, NULL);
		/* this->mutex=NULL;
		 * The right way to pthread__mutex_t
		 */
		pthread_mutex_init(&mutex, NULL);

	}


	DsmDriver::~DsmDriver() {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_exit " << this->nid;
		this->send(outs.str());
		string exitOK = this->receive();
		this->observer->stop();
		this->close();
		pthread_cond_init(&condition, NULL);
		pthread_mutex_init(&mutex, NULL);
	}

	DsmNodeId DsmDriver::get_nid() {
		return this->nid;
	}

	void DsmDriver::dsm_malloc(string blockId, int size) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_malloc " << this->nid << " " << blockId << " " << size;
		this->send(outs.str());
		string response = this->receive();
		if (response.find("ERROR") == 0) {
			throw DsmException(response);
		} else {
			istringstream ins;
			ins.str(response);
			unsigned long addr;
			ins >> addr;
		}
	}

	void DsmDriver::dsm_put(string blockId, void * content, int size) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_put " << this->nid << " " << blockId << " " << size << " ";
		for (int i=0; i<size; i++) {
			outs << ((char *)content)[i];
		}
		this->send(outs.str());
		string response = this->receive();
		if (response.find("ERROR") == 0) {
			throw DsmException(response);
		} 
	}

	DsmData DsmDriver::dsm_get(string blockId) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_get " << this->nid << " " << blockId;
		this->send(outs.str());
				
		char * content = NULL;
		long bytesRead = this->receive(content);
		DsmData data;
		data.size = bytesRead;
		data.addr = malloc(bytesRead);
		memcpy(data.addr, content, bytesRead);
		delete [] content;
		if (data.size >= 5) {
			char msg[6];
			memcpy(msg, data.addr, 5);
			msg[5] = '\0';
			if (strcmp(msg, "ERROR") == 0) {
				string response((char*)data.addr);
				delete [] (char*)data.addr;
				throw DsmException(response);
			}
		}
		return data;
	}


	void DsmDriver::dsm_free(string blockId) throw (DsmException) {
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_free " << this->nid << " " << blockId;
		this->send(outs.str());
		string response = this->receive();
		if (response.find("ERROR") == 0) {
			throw DsmException(string(response));
		}
	}


	void DsmDriver::dsm_notify(string cmd, string blockId) {
		// MODIFICACI�N PR�CTICA DSM: seguir indicaciones de 3.3.5 (punto 3)
		cout << "***NOTIFICATION: " << cmd << " " << blockId << endl;
		if (cmd == "dsm_put") {
			// Add the new DsmEvent received
			DsmEvent dsmEvent;
			dsmEvent.cmd = cmd;
			dsmEvent.blockId = blockId;
			this->putEvents.push_back(dsmEvent);
		} else if (cmd == "dsm_free") {
			for (vector<DsmEvent>::iterator it = this->putEvents.begin(); it!=this->putEvents.end(); ++it) {
				if ((it->cmd == "dsm_put") && (it->blockId == blockId)) {
					// We should wipe out all the dsm_puts received for a block (problem when removing break)
					this->putEvents.erase(it);
					// DONE: not remove the break
					break;
				}
			}
		}
		pthread_cond_signal(&condition);
	}

	void DsmDriver::dsm_wait(string blockId) {

		pthread_mutex_lock(&mutex);

		bool blockPutEventReceived = false;
		while (!blockPutEventReceived) {
			for (vector<DsmEvent>::iterator it = this->putEvents.begin(); it != this->putEvents.end(); ++it) {
				if ((it->cmd == "dsm_put") && (it->blockId == blockId)) {
					blockPutEventReceived = true;
					this->putEvents.erase(it);
					break;
				}
			}

			if (!blockPutEventReceived) {
				// DONE: use binary semaphore initialized to 0 for conditional synchronisation
				// MODIFICACI�N PR�CTICA DSM: Seguir instrucciones de modificaci�n 3.3.5.3
				//pthread_mutex_lock(&this->mutex);
				pthread_cond_wait(&this->condition,&this->mutex);
				//pthread_mutex_unlock(&this->mutex);
			}

		}
	}

	
	ostream & operator << (ostream &os, DsmDriver &n) {
		os << "nid: " << n.nid << endl;
		return os;
	}
}

