// PasapalabraServerThread.h
// author: dipina@eside.deusto.es
#ifndef __PASAPALABRASERVERTHREAD_H
#define __PASAPALABRASERVERTHREAD_H

#include "Thread.h"
#include "TcpListener.h"

namespace PracticaCaso
{
	class PasapalabraServerThread: public Thread {
		private:
			TcpClient* client;
			void run();
		public:
			PasapalabraServerThread(TcpClient* c): client(c) {}
			~PasapalabraServerThread();
	};
};
#endif
