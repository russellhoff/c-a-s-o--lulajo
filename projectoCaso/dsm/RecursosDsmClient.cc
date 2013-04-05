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

void printRecursosDisponibles(char * resources){

	cout << "Recursos disponibles: " << endl;
	char *p = strtok(resources, ";");
	while (p) {

		//printf ("Token: %s\n", p);
		cout << "\t " << p << endl;
		p = strtok(NULL, ";");

	}

}



/*bool existsResource(char * resources, char * resource){

	bool enc = false;
	cout << "El recurso a comprobar es:" << resource << endl;
	char *p = strtok(resources, ";");
	while (p) {
		cout << "Recurso actual a testear: " << p << endl;
		if(p == resource){
			cout << "Existe!! Saliendo..." << endl;
			enc = true;
			break;
		}

		p = strtok(NULL, ";");

	}

	return enc;

}*/

int main(int argc, char** argv) {

	if ( argc < 4 || argc > 6 ) {
		usage();
	}

	int port =atoi(argv[1]);
	char * domain =argv[2];
	char * action = argv[3];

	string act = string(action);

	// Hacer lookup dsm.deusto.es
	PracticaCaso::DsmDriver * driver = new PracticaCaso::DsmDriver("127.0.0.1", port, domain);
	PracticaCaso::DsmData dataResources, dataResource, dataResourcePass;

	int nid = driver->get_nid();

	//string _REC_ = "RESOURCES";

	/*
	 * El string resources no se puede actualizar (dsm_free) por otro nodo que no sea el que lo creo.
	 * Por ello, cada vez que se quiera modificar "RESOURCES" se debera crear uno nuevo con su nid,
	 * de tal modo que se llamara de esta manera:
	 *
	 * "RESOURCES" + NID
	 *
	 * El primer RESOURCES creado se llamara RESOURCES0. Cada vez que se haga un put, se creara un nuevo
	 * Resources con su nid, asi que RESOURCES1, RESOURCES2,...RESOURCESN para la N vez que se haga dsm_put.
	 *
	 */
	/*
	 * Ahora trataremos de adivinar cual es el ultimo resources creado:
	 */
	/*string jj = "RESOURCES";
	int num = 0;
	do {

		try {
			driver->dsm_get(jj+static_cast<ostringstream*>( &(ostringstream() << num) )->str());
		} catch (DsmException dsme) {
			cout << "No encontrado RESOURCES" << num << endl;
			break;
		}
		num++;
	} while (true);
	if(num>0)
		num = num - 1;
	string ultimoResources = "RESOURCES" + static_cast<ostringstream*>( &(ostringstream() << num) )->str();

	cout << "El ultimo string de RESOURCES se llama: " << ultimoResources << endl;*/

	/*
	 * Fin de adivinar el ultimo string de RESOURCES
	 */
	if( act!="get" && act!="put" /*&& act!="list_resources"*/){
		//cout << "+++Mostrando usage()... Action: " << action << endl;
		usage();
		exit(1);
	}//else{
		/*
		 * There will be a resources string, which will contain every resource name separated by a ';'. This
		 * string is named "RESOURCES"
		 *		resources = "resource1;resource2;...;resourceN";
		 * Each resource will be present by means of the following:
		 *
		 * 		resourceN = "the info for the resource N"
		 * 		resourceN_passphrase = <pashphrase-for-resourceN>
		 *
		 */
		//cout << "Hacer resources..." << endl;
//		char * resources;

		/*try {
			cout << "Try..." << endl;
			dataResources = driver->dsm_get(ultimoResources);
			cout << "Tras dsm_get..." << endl;
			resources = ((char *)dataResources.addr);
			cout << "Tras pillar resources..." << endl;
			cout << "Resources string already initialized." << endl;
		} catch (DsmException dsme) {*/
/*		if(ultimoResources == "RESOURCES0"){
			cout << "Resources string is NOT initialized. Trying to initialize it... " << endl;

			resources = "recurso-vacio;";

			driver->dsm_malloc(ultimoResources, strlen(resources));

			try {
				driver->dsm_put(ultimoResources, (void *)resources, strlen(resources));
			} catch (DsmException dsme) {
				cerr << "ERROR: dsm_put(\"" << ultimoResources << "\", a, " << strlen(resources) << ")): " << dsme << endl;
				driver->dsm_free(ultimoResources);
				exit(1);
			}

		}*/

//	}

	cout << "Client successfully created with nid \'" << nid << "\' in order to " << action << " data." << endl;

	/*
	 * Vamos a ver que nos ha pasado el usuario como parametro...
	 */
	if( act=="get"){

		if( argc == 6){

			/*
			 * DONE
			 */

			char * rec = argv[4];
			string recStr = string(rec);
			char * secret = argv[5];
			string secretStr = string(secret);

			//Pillar info en el DSM.
			//cout << "Vamos a pillar el recurso " << recStr << " con el secreto " << secretStr << endl;

			/*if(!existsResource(recs,rec)){
				cout << "Resource \'" << rec << "\' not found." << endl;
			}else{*/
				//if we go on, var it is supposed to contain the index of the desired info and secret/password

				char * recursoInfo;
				try{
					dataResource = driver->dsm_get(recStr);


					recursoInfo = ((char *)dataResource.addr);

					cout << "Vamos a pillar el recurso " << recStr << " con el secreto " << secretStr << endl;

					char * recursoSecret;
					try{
						string auxGetResourcePass;
						cout << "****Contrasenya a poner: " << secretStr << endl;
						auxGetResourcePass = recStr;
						auxGetResourcePass = auxGetResourcePass + "_passphrase";
						cout << "Vamos a pillar este bloque: " << auxGetResourcePass << endl;
						dataResourcePass = driver->dsm_get(auxGetResourcePass);

						recursoSecret = ((char *)dataResourcePass.addr);

						//cout << "Secret written: " << secretStr << endl;
						//cout << "Secret of the resource: " << recursoSecret << endl;
						if( recursoSecret == secretStr ){
							//Secret is correct!
							cout << "Secret valid for \'" << rec << "\'!" << endl;
							cout << "Resource \'" << rec << "\': " << recursoInfo << endl;
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


			//}
			//exit(0);

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

			//Poner info en el DSM.
			//cout << "Vamos a poner el recurso " << recStr << " con el secreto " << secretStr << endl;

			//char * recursos;

			try{
				//dataResources = driver->dsm_get(ultimoResources);

				//recursos = ((char *)dataResources.addr);
				//char * recursosCopy;
				//recursosCopy = recursos;
				//cout << "*******Recursos ya metidos anteriormente: " << recursos << endl;

				/*bool enc = false;
				cout << "El recurso a comprobar es:" << rec << endl;
				char *p = strtok(recursos, ";");
				while (p) {
					cout << "Recurso actual a testear: " << p << endl;
					if(p == rec){
						cout << "Existe!! Saliendo..." << endl;
						enc = true;
						break;
					}

					p = strtok(NULL, ";");

				}*/

				/*if(enc ){
					cerr << "El recurso " << rec << " ya esta introducido." << endl;
					exit(1);
				}*/

				//cout << "*******Recursos ya metidos anteriormente: " << recursos << endl;

				cout << "Now type your info, please: " << endl;

				/**/
				string input;

				getline(cin, input);
				//cout << "hola1" << endl;
				char * inputInCharStar = (char *) input.c_str();
				//cout << "hola2" << endl;
				//char * recursosAux = "";
				//cout << "hola3" << endl;

			//	recursosAux=recursos;
				//strcat(recursosAux,";");cout << "Tras asignar lo anterior de recursos y;: " << recursosAux << endl;
			//	cout << "hola4" << endl;
			//	strcat(recursosAux,rec);
				//recursosAux = recursosAux + rec;
				//cout << "Tras anyadir el recurso actual: " << recursosAux << endl;
			//	strcat(recursosAux,";"); cout << "hola 4.5" << endl;
				//cout << "Tras meter ;: " << recursosAux << endl;
			//	cout << "hola5" << endl;
				//cout << "Asi queda el string de recursos: " << recursosAux << endl;
				//cout << "hola6" << endl;

				/*
				 * Tenemos que crear un nuevo RESOURCES con el NUM superior al anterior:
				 */
				/*int asada = num + 1;
				string elNum = static_cast<ostringstream*>( &(ostringstream() << asada) )->str();
				cout << "El nuevo resources va a tener el num: " << elNum.c_str() << endl;
				string resourcesActual = "RESOURCES" + elNum;
				cout << "A introducir " << resourcesActual << endl;*/
		/*		driver->dsm_malloc(resourcesActual, strlen(recursosAux));
				cout << "Pasamos del malloc..." << endl;
				try {
					driver->dsm_put(resourcesActual, (void *)recursosAux, strlen(recursosAux));
				} catch (DsmException dsme) {
					cerr << "ERROR: dsm_put(\"" << resourcesActual << "\", a, " << strlen(recursosAux) << ")): " << dsme << endl;
					driver->dsm_free(resourcesActual);
					exit(1);
				}
				cout << "Actualizada RESOURCES con nombre " << resourcesActual << " que contiene " << recursosAux << endl;
				//Guardar la Info del recurso y su passphrase (sus nombres: recurso-->info , recurso_passphrase-->passphrase)
		*/
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


			//exit(0);
		}else{
			usage();
		}

	}/*else if( act=="list_resources"){
		/*
		 * HECHO. si recursos es vacio, mostrar un texto.
		 */
	/*	cout << "Vamos a listar todos los recursos disponibles" << endl;

		//El array de recursos es de 100 elementos max
		char * recursos;
		cout << "Consultando el resources: " << ultimoResources << endl;
		dataResources = driver->dsm_get(ultimoResources);
		recursos = ((char *)dataResources.addr);

		printRecursosDisponibles(recursos);
		exit(0);
	}*/else{
		usage();
	}

}
