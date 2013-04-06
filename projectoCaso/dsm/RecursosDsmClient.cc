#include "TcpListener.h"
#include "Dsm.h"
#include <string.h>
#include <string>

#define BUFFER_SIZE 1024

void usage() {
	cout << "Usage: RecursosDsmClient <name-server-port> <dsm-domain> <action-to-do> <...>" << endl;
	cout << "<action-to-do> options: "
		 << "\n\t1) [not implemented] list_resources : prints the available resources within the DSM Server."
		 << "\n\t2) get <resource-name> <resource-passphrase> - In order to fetch some data in the DSM Server. "
		 << "\n\t\t\t\t<resource-name> : name of the resource to fetch."
		 << "\n\t\t\t\t<resource-passphrase> : resource\'s passphrase."
		 << "\n\t3) put <resource-name> <resource-passphrase> - In order to put some data in the DSM Server. "
		 << "\n\t\t\t\t<resource-name> : name of the resource to put."
		 << "\n\t\t\t\t<resource-passphrase> : resource\'s passphrase."
		 << endl;
	exit(1);
}

int main(int argc, char** argv) {

	if ( argc < 4 || argc > 6 ) {
		usage();
	}

	int port =atoi(argv[1]);
	char * domain =argv[2];
	char * action = argv[3];

	string act = string(action);

	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver("127.0.0.1", port, domain);
	PracticaCaso::DsmData dataResources, dataResource, dataResourcePass;

	int nid = driver->get_nid();


	if( act!="get" && act!="put" /*&& act!="list_resources"*/){

		usage();
		exit(1);
	}
	cout << "Client successfully created with nid \'" << nid << "\' in order to " << action << " data." << endl;

	if( act=="get"){

		if( argc == 6){

			/*
			 * DONE
			 */

			char * rec = argv[4];
			string recStr = string(rec);
			char * secret = argv[5];
			string secretStr = string(secret);

				char * recursoInfo;
				try{
					dataResource = driver->dsm_get(recStr);


					recursoInfo = ((char *)dataResource.addr);
					string recursoInfoStr = string(recursoInfo);

					cout << "Vamos a pillar el recurso " << recStr << " con el secreto " << secretStr << endl;

					char * recursoSecret;
					try{
						string auxGetResourcePass;
						//cout << "****Contrasenya a poner: " << secretStr << endl;
						auxGetResourcePass = recStr;
						auxGetResourcePass = auxGetResourcePass + "_passphrase";
						//cout << "Vamos a pillar este bloque: " << auxGetResourcePass << endl;
						dataResourcePass = driver->dsm_get(auxGetResourcePass);

						recursoSecret = ((char *)dataResourcePass.addr);

						if( recursoSecret == secretStr ){
							//Secret is correct!
							cout << "Secret valid for \'" << rec << "\'!" << endl;
							cout << "Resource \'" << rec << "\': " << recursoInfoStr << endl;
						}else{
							//Secret is not valid!

							cout << "Secret for resource \'" << rec << "\' not valid." << endl;

						}

					}catch(DsmException dsme){
						cerr << "Exception: " << dsme << endl;
					}


				}catch(DsmException & dsme){
					cerr << "Exception: " << dsme << endl;
				}


		}else{
			usage();
		}


	}else if( act=="put"){

		if( argc == 6 ){
			/*
			 * DONE
			 */


			char * rec = argv[4];
			string recStr = string(rec);
			char * secret = argv[5];
			string secretStr = string(secret);


			try{


				cout << "Now type your info, please: " << endl;

				/**/
				string input;

				fflush(stdin);

				getline(cin, input);

				char * inputInCharStar = (char *) input.c_str();

				driver->dsm_malloc(rec, strlen(inputInCharStar));

				try {
					driver->dsm_put(rec, (void *)inputInCharStar, strlen(inputInCharStar));

					char * recPasshraseAux;
					recPasshraseAux = rec;

					strcat(recPasshraseAux,"_passphrase");
					try{

						driver->dsm_malloc(recPasshraseAux, secretStr.size());

						try {
							driver->dsm_put(recPasshraseAux, (void *)secretStr.c_str(), secretStr.size());
							//cout << "Fin!! Se ha introducido la info... con contrasenya: " << secretStr << endl;
						} catch (DsmException dsme) {
							cerr << "ERROR: dsm_put(\"" << recPasshraseAux << "\", a, " << strlen(secret) << ")): " << dsme << endl;
							driver->dsm_free(rec);
							driver->dsm_free(recPasshraseAux);
							exit(1);
						}
					}catch (DsmException dsme) {
						cerr << "ERROR:en malloc the la contraseña: " << dsme << endl;
						exit(1);
					}


				} catch (DsmException dsme) {
					cerr << "ERROR: dsm_put(\"" << rec << "\", a, " << strlen(rec) << ")): " << dsme << endl;
					driver->dsm_free(rec);

					exit(1);
				}



			}catch(DsmException & dsme){
				cerr << "Exception: " << dsme << endl;
			}



		}else{
			usage();
		}

	}else{
		usage();
	}

}
