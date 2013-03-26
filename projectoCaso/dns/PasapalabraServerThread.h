// PasapalabraServerThread.h
// author: dipina@eside.deusto.es
#ifndef __PASAPALABRASERVERTHREAD_H
#define __PASAPALABRASERVERTHREAD_H

#include "Thread.h"
#include "TcpListener.h"
#include "SQLiteMapPasapalabra.h"
#include <string.h>

namespace PracticaCaso
{
	class PasapalabraServerThread: public Thread {
		private:
			TcpClient* client;
			SQLiteMapPasapalabra * sqliteMapPasapalabra;
			void run();
		public:
			PasapalabraServerThread(TcpClient* c, string f);
			PasapalabraServerThread(const PasapalabraServerThread&);
			PasapalabraServerThread & operator = (const PasapalabraServerThread &);
			~PasapalabraServerThread();
			friend ostream & operator << (ostream &os, PasapalabraServerThread &n);
	};
};
#endif
