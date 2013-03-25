// NameServer.h
// author: dipina@eside.deusto.es
#ifndef __NAMESERVER_H
#define __NAMESERVER_H

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

#include "TcpListener.h" //todo servidor debe incluir esa libreria.
#include "Thread.h"		//para hacerlo multihilo.
#include "SQLiteMap.h"	//para almacenar todo lo que queremos aprender.

using namespace std;

namespace PracticaCaso
{
	class NameServer: public TcpListener {
		private:
			string domain;
			SQLiteMap * sqliteMap;
			map<string, string> dns2IpPortMap;
			bool leerCache;
			/*
			 * false: lanza servidor de nombres y solo lee el fichero de configuracon
			 * true: se hara uso del objeto sqlitemap para coger lo que se ha guardado en otra ocasion.
			 */

			void loadMappings(string mappinsFileName);
			string delegateExternalDnsServer(string serverDetails, string dnsName);
		public:
			NameServer(int p, string m, bool leerCache);
			NameServer(const NameServer&);
			NameServer & operator = (const NameServer &);
			~NameServer();
			string translate(string dnsEntry); //recibe todas las peticiones de traduccion
			//mira en el mapa, si no lo tenemos, llamamos al metodo de delegacion. Si lo tenemos, lo devolvemos.
			friend ostream & operator << (ostream &os, NameServer &n);

	};


	class NameServerThread: public Thread {
		private:
			TcpClient* dnsClient;
			NameServer& dnsServer;
			void run();
		public:
			NameServerThread(TcpClient* c, NameServer& s): dnsClient(c), dnsServer(s) {}
			~NameServerThread();
	};
};
#endif
