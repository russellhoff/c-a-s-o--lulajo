// DsmServer.cc
// author: dipina@eside.deusto.es

#include "DsmServer.h"

extern "C" {
	#include </usr/include/signal.h>
}

#define SPACES " \t\r\n"

inline string trim_right (const string & s, const string & t = SPACES) { 
	string d (s); 
	string::size_type i (d.find_last_not_of (t));
	if (i == string::npos)
		return "";
	else
		return d.erase (d.find_last_not_of (t) + 1) ; 
}  // end of trim_right


inline string trim_left (const string & s, const string & t = SPACES) { 
	string d (s); 
	return d.erase (0, s.find_first_not_of (t)) ; 
}  // end of trim_left


inline string trim (const string & s, const string & t = SPACES) { 
	string d (s); 
	return trim_left (trim_right (d, t), t) ; 
}  // end of trim


inline string str(int num) {
	char buffer[32];
	sprintf(buffer, "%d", num);
	return string(buffer);
}

inline string str(unsigned long num) {
	char buffer[32];
	sprintf(buffer, "%d", num);
	return string(buffer);
}

namespace PracticaCaso {
	DsmServer::DsmServer(int p): nidCounter(-1), nodeCounter(0), TcpListener(p) {
		//inicializamos la variable rwlock que hemos creado para el evaluable 3.3.5 (4)
		pthread_rwlock_init(&this->rwlock, NULL);
	}


	DsmServer::~DsmServer() {
		this->stop();
		// TODO: destory lock
		//destroy 3.3.5 (4)
		pthread_rwlock_destroy(&this->rwlock);
	}
			
	DsmNodeId DsmServer::dsm_init(TcpClient * dmsClient) {
		DsmNodeMetadata metadata;
		metadata.nid = ++nidCounter;
		metadata.client = dmsClient;
		dsmNodeMap[metadata.nid] = metadata;
		nodeCounter++;
	
		return metadata.nid;
	}

	//PARA LA MODIFICACION 3.3.5 (4) BASICAMENTE LO QUE HAY QUE HACER ES QUE DIFERENTES NODOS/PROCESOS PUEDAN ACTUALIZAR BLOQUES COMPARTIDOS
	//ADEMAS HAY QUE PERMITIR QUE TANTOS CLIENTES PUEDAN LEER LOS CONTENIDOS DE ESTOS BLOQUES
	//PARA ELLO UTILIZAMOS UN READ/WRITE LOCK. EL HILO QUE ABRE EL LOCK ES EL QUE HA DE CERRARLO, ESTO NOS PROTEGE DE ESCRITURAS NO-CONCURRENTES
	//ASIQUE IMPLEMENTAMOS ESTAS PROTECCIONES EN LOS METODOS QUE REALIZAN ESTE TIPO DE TAREAS
	void DsmServer::dsm_exit(DsmNodeId nodeId) {
		// Remove all the data structures created by this node
		//abrimos el lock de lectura 3.3.5(4)
		pthread_rwlock_rdlock(&this->rwlock);
		if (dsmNodeMap.find(nodeId) != dsmNodeMap.end()) {
			//liberamos el lock de lectura y abrimos el de escritura 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);
			pthread_rwlock_wrlock(&this->rwlock);
			--nodeCounter;
			if (nodeCounter == 0) {
				for (int i=0; i<dsmNodeMap[nodeId].dsmBlocksRequested.size(); i++) {
					(this->blockMetadataMap).erase(dsmNodeMap[nodeId].dsmBlocksRequested[i].blockId);
					free(dsmNodeMap[nodeId].dsmBlocksRequested[i].addr);
				}
				
			}
			dsmNodeMap.erase(nodeId);
			//liberamos el lock de escritura 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);
		}else{
			//necesitamos un else para liberar el lock de lectura en cualquier caso(si el condicional no entra en el if)
			//liberamos el lock(de lectura) 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);
		}
	}
	

	void * DsmServer::dsm_malloc(DsmNodeId nid, string blockId, int size) {
		//Lock de lectura 3.3.5 (4)

		pthread_rwlock_rdlock(&this->rwlock);
		if (this->dsmNodeMap.find(nid) != this->dsmNodeMap.end()) {
			if (this->blockMetadataMap.find(blockId) == this->blockMetadataMap.end()) {
				//liberar lock de lecutra 3.3.5 (4)
				pthread_rwlock_unlock(&this->rwlock);
				DsmBlock block;
				//Lock de escritura 3.3.5 (4)
				pthread_rwlock_wrlock(&this->rwlock);
				block.addr = malloc(size);
				if (block.addr != NULL) {
					block.blockSize = size;
					block.size = 0;
					block.creatorNode = nid;
					block.lastAccessNode = nid;
					this->blockMetadataMap[blockId] = block;
					DsmNodeMetadata metadata = this->dsmNodeMap[nid];
					metadata.dsmBlocksRequested.push_back(block);
					this->dsmNodeMap[nid] = metadata;

				//liberar lock de escritura 3.3.5(4)
				pthread_rwlock_unlock(&this->rwlock);

					return block.addr;

				} else {
					cerr << "ERROR: DMS Server ran out of memory!!!" << endl;
					return 0;
				}



			} else {
				cerr << "WARNING: attempt to create block " << blockId << " already existing by " << nid << "!!!" << endl;
				DsmBlock tempBlock = this->blockMetadataMap[blockId];
				//liberar lock de lectura 3.3.5(4)
				pthread_rwlock_unlock(&this->rwlock);
				if (tempBlock.size < size) {
					cerr << "ERROR: impossible to reuse block " << blockId << " of size " << tempBlock.size << " < " << size << endl;
					return 0;
				} else {
					return tempBlock.addr;
				}
			}
		} else {
			//liberar lock de lectura 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);
			cerr << "ERROR: attempt to create block " << blockId << " by non-registered node " << nid << "!!!" << endl;
			return 0;
		}
	}


	bool DsmServer::dsm_put(DsmNodeId nid, string blockId, void * content, int size) {

		//Lock de lectura 3.3.5 (4)
		pthread_rwlock_rdlock(&this->rwlock);
		if (this->blockMetadataMap.find(blockId) != this->blockMetadataMap.end()) {
			bool dsmPutResult = false;
			DsmBlock blockMetadata = this->blockMetadataMap[blockId];
			// We allow anybody to write over the blocks
			if ( size <= blockMetadata.blockSize ) {

				//liberar lock de lectura 3.3.5 (4)
				pthread_rwlock_unlock(&this->rwlock);

				//lock de escritura 3.3.5 (4)
				pthread_rwlock_wrlock(&this->rwlock);

				bzero(blockMetadata.addr, blockMetadata.blockSize);
				memcpy(blockMetadata.addr, content, size);
				blockMetadata.size = size;
				blockMetadata.lastAccessNode = nid;
				this->blockMetadataMap[blockId] = blockMetadata;

				//liberar lock de escritura 3.3.5 (4)
				pthread_rwlock_unlock(&this->rwlock);

				dsmPutResult = true;
			} else {

				//liberar lock de lectura 3.3.5 (4)
				pthread_rwlock_unlock(&this->rwlock);

				cerr << "ERROR: The node " << nid << " does not have write access!!!" << endl;
			}
			return dsmPutResult;
		} else {

			//liberar lock de lectura 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);

			cerr << "ERROR: blockId " + blockId + " does not exist" << endl;
			return false;
		}
	}

	void DsmServer::dsm_notify(string message) {
		DsmNotifierThread* t = new DsmNotifierThread(message);
		t->start();
	}

	void DsmServer::dsm_notify_free(int nid, string blockId) {
		/* now just sendto() our destination! */
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_free " << nid << " " << blockId;
		dsm_notify(outs.str());
	}

	void DsmServer::dsm_notify_put(int nid, string blockId) {
		/* now just sendto() our destination! */
		ostringstream outs;  // Declare an output string stream.
		outs << "dsm_put " << nid << " " << blockId;
		dsm_notify(outs.str());
	}

	DsmBlock DsmServer::dsm_get(DsmNodeId nid, string blockId) {

		//Lock de lectura 3.3.5 (4)
		pthread_rwlock_rdlock(&this->rwlock);
		if (this->blockMetadataMap.find(blockId) != this->blockMetadataMap.end()) {
			DsmBlock temp = this->blockMetadataMap[blockId];

			//liberar lock de lectura 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);

			return temp;
		} else {
			DsmBlock block;
			block.blockId = "ERROR";

			//liberar lock de lectura 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);

			return block;
		}
	}

	//UNA DE LAS COSAS QUE SE NOS PIDEN EN EL 3.3.4 (4) ES QUE SOLO EL ULTIMO NODO QUE ACTUALIZO EL BLOQUE PUEDE LIBERARLO
	//ESO LO IMPLEMENTAMOS AQUI, EN EL METODO FREE
	bool DsmServer::dsm_free(DsmNodeId nid, string blockId) {

		//Lock de lectura 3.3.5 (4)
		pthread_rwlock_rdlock(&this->rwlock);

		if (this->dsmNodeMap.find(nid) != this->dsmNodeMap.end()) {
			DsmNodeMetadata nodeMetadata = this->dsmNodeMap[nid];
			if (this->blockMetadataMap.find(blockId) != this->blockMetadataMap.end()) {
				DsmBlock blockMetadata = this->blockMetadataMap[blockId];

				//el id del nodo que intenta liberarlo ha de coincidir con el del ultimo nodo que accedio al bloque
				if (blockMetadata.lastAccessNode == nid) {

					//liberar lock de lectura 3.3.5 (4)
					pthread_rwlock_unlock(&this->rwlock);

					//lock de escritura 3.3.5 (4)
					pthread_rwlock_wrlock(&this->rwlock);

					(this->blockMetadataMap).erase(blockId);
					vector<DsmBlock> blocksRequested = (this->dsmNodeMap[nid]).dsmBlocksRequested;

					for (vector<DsmBlock>::iterator it = blocksRequested.begin(); it!=blocksRequested.end(); ++it) {
						if ( (blockMetadata.lastAccessNode == it->lastAccessNode) && 
							 (blockMetadata.blockSize == it->blockSize) && 
							 (blockMetadata.addr == it->addr)
							) {
							blocksRequested.erase(it, ++it);
							(this->dsmNodeMap[nid]).dsmBlocksRequested = blocksRequested;
							//liberar lock de escritura 3.3.5 (4) Write Unlock
							pthread_rwlock_unlock(&this->rwlock);
							return true;
						}
					}

					//liberar lock de escritura 3.3.5 (4) Write Unlock
					pthread_rwlock_unlock(&this->rwlock);

					return false;
				} else {

					//liberar lock de lectura 3.3.5 (4)
					pthread_rwlock_unlock(&this->rwlock);

					return false;
				}
			}
			else {

				//liberar lock de lectura 3.3.5 (4)
				pthread_rwlock_unlock(&this->rwlock);
			}
		} else {

			//liberar lock de lectura 3.3.5 (4)
			pthread_rwlock_unlock(&this->rwlock);

			return false;
		}
	}

	ostream & operator << (ostream &os, DsmServer &n) {
		os << "lastNid: " << n.lastNid << " - nodeCounter: " << n.nodeCounter << endl;
		// << " - dsmNodeMap: " << n.dsmNodeMap << " - blockMetadataMap " << n.blockMetadataMap << endl;
		return os;
	}

	DsmServerThread::~DsmServerThread() {
		delete this->dsmClient;
	}

	void DsmServerThread::run() {
		char* dsmCmd;
		int bytesRead;
		bool dsm_exit = false;
		while (!dsm_exit) {
			bytesRead = (this->dsmClient)->receive(dsmCmd);
			if (strncmp(dsmCmd, "dsm_init", strlen("dsm_init")) == 0) {
				// DsmNodeId dsm_init(TcpClient *);
				DsmNodeId nid = (this->dsmServer).dsm_init(this->dsmClient);
				(this->dsmClient)->send(str(nid));
				cout << "dsm_init " << nid << endl;
			} else if (strncmp(dsmCmd, "dsm_exit", strlen("dsm_exit")) == 0) {
				//void dsm_exit(DsmNodeId);
				string dsmCmd2(dsmCmd, 0, bytesRead);
				DsmNodeId nid = atoi((trim(dsmCmd2.substr(strlen("dsm_exit")))).c_str());
				(this->dsmServer).dsm_exit(nid);
				(this->dsmClient)->send(str(nid));
				dsm_exit = true;
				cout << "dsm_exit " << nid << endl;
			} else if (strncmp(dsmCmd, "dsm_malloc", strlen("dsm_malloc")) == 0) {
				// void * dsm_malloc(DsmNodeId nid, string blockId, int size);
				// dsm_malloc nid size
				string dsmCmd2(dsmCmd, 0, bytesRead);
				char blockId[256];
				DsmNodeId nid;
				int size;
				sscanf (dsmCmd2.c_str(),"%*s %d %s %d", &nid, &blockId, &size);
				void *p = (this->dsmServer).dsm_malloc(nid, blockId, size);
				if (p) {
					(this->dsmClient)->send(str((unsigned long)p));
					cout << "dsm_malloc " << nid << " " << blockId << " " << size << endl;
				} else {
					(this->dsmClient)->send("ERROR in dsm_malloc: nid " + str(nid) + " has not registered with DSM server or blockId " + blockId + " already exists");
				}
		
			} else if (strncmp(dsmCmd, "dsm_put", strlen("dsm_put")) == 0) {
				// void dsm_put(DsmNodeId nid, string blockId, void * content, int size);
				// dsm_put nid addr size datum
				// dsm_put 1 12134 45 hola estudiantes caso
				int beginDataIndex = 0;
				char seps[]   = " ";
				string token = strtok(dsmCmd, seps );
				beginDataIndex += (token.size() + 1);
				token = strtok(0, seps);
				beginDataIndex += (token.size() + 1);
				int nid = atoi(token.c_str());
				string blockId = strtok(0, seps);
				beginDataIndex += (blockId.size() + 1);
				token = strtok(0, seps);
				beginDataIndex += (token.size() + 1);
				int size = atoi(token.c_str());
				// (dsmCmd + beginDataIndex)
				if ((this->dsmServer).dsm_put(nid, blockId, (void *)&dsmCmd[beginDataIndex], bytesRead-beginDataIndex)) {
					(this->dsmClient)->send(blockId);
					(this->dsmServer).dsm_notify_put(nid, blockId);
					cout << "dsm_put " << nid << " " << blockId << " " << size << " <data>" << endl;
				} else {
					string errMsg = "ERROR in dsm_put: either blockId " + blockId + " already exists or nid " + str(nid) + " does not exist";
					(this->dsmClient)->send(errMsg);
					cout << errMsg << endl;
				}
			} else if (strncmp(dsmCmd, "dsm_get", strlen("dsm_get")) == 0) {
				string dsmCmd2(dsmCmd, 0, bytesRead);
				// DsmBlock dsm_get(DsmNodeId nid, void *addr); 
				int nid;
				string blockId;
				string cmd;
				istringstream ins; // Declare an input string stream.
				ins.str(dsmCmd2);        // Specify string to read.
				ins >> cmd >> nid >> blockId;     // Reads the integers from the string.
				DsmBlock block = (this->dsmServer).dsm_get(nid, blockId);
				if (block.blockId != "ERROR") {
					(this->dsmClient)->send((char *)block.addr, block.size);
					cout << "dsm_get " << nid << " " << blockId << endl;
				} else {
					(this->dsmClient)->send("ERROR in dsm_get: blockId " + blockId + " passed does not exist"); 
				}
				cout << "dsm_get " << nid << " " << blockId << endl;
			} else if (strncmp(dsmCmd, "dsm_free", strlen("dsm_free")) == 0) {
				// bool dsm_free(DsmNodeId nid, void* addr);
				string dsmCmd2(dsmCmd, 0, bytesRead);
				DsmNodeId nid;
				string cmd, blockId;
				istringstream ins; // Declare an input string stream.
				ins.str(dsmCmd2);        // Specify string to read.
				ins >> cmd >> nid >> blockId;     // Reads the integers from the string.
				bool blockFreed = (this->dsmServer).dsm_free(nid, blockId);
				if (blockFreed) {
					(this->dsmClient)->send(blockId);
					(this->dsmServer).dsm_notify_free(nid, blockId);
					cout << "dsm_free " << nid << " " << blockId << endl;
				} else {
					string msgError = "ERROR in dsm_free(" + str(nid) + ", " + blockId + "): node has not registered or it has not created the blockId passed";
					(this->dsmClient)->send(msgError);
					cerr << msgError << endl;
				}
			}  else {
				// Invalid command received
				(this->dsmClient)->send("ERROR: unrecognized command received");
			}
			delete []dsmCmd;
		}
	}

	
	DsmNotifierThread::DsmNotifierThread(string m): message(m) {
		/* create what looks like an ordinary UDP socket */
		if ((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0) {
			cerr << "Problems creating multicast socket" << endl;
			exit(1);
		}

		/* set up destination address */
		memset(&addr,0,sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr.s_addr=inet_addr(DSM_GROUP);
		addr.sin_port=htons(DSM_PORT);
	}

	DsmNotifierThread::~DsmNotifierThread() {
	}

	void DsmNotifierThread::run() {
		/* now just sendto() our destination! */
		if (sendto(fd, message.c_str(),message.size(),0,(struct sockaddr *) &addr, sizeof(addr)) < 0) {
			cerr << "ERROR: sendto in DsmNotifierThread::run()" << endl;
			perror("hola");
			exit(1);
		}
	}

}


// global variable
PracticaCaso::DsmServer * dsmServer_pointer;

// function called when CTRL-C is pressed
void ctrl_c(int)
{
    printf("\nCTRL-C was pressed...\n");
	dsmServer_pointer->stop();

}


void usage() {
	cout << "Usage: DsmServer <listening-port>" << endl;
	exit(1);
}


int main(int argc, char** argv) {
	signal(SIGINT,ctrl_c);

	if (argc != 2) {
		usage();
	}

	PracticaCaso::DsmServer dsmServer(atoi(argv[1]));
	dsmServer_pointer = &dsmServer;
	dsmServer.start();

	while (true) {
		PracticaCaso::TcpClient *client = dsmServer.acceptTcpClient();
		cout << "Tcpclient accepted: " << client << endl;
		PracticaCaso::DsmServerThread* t = new PracticaCaso::DsmServerThread(client, dsmServer);
		t->start();
	}

	dsmServer.stop();
}
