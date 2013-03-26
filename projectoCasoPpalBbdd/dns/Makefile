# Makefile
# author: dipina@eside.deusto.es
CC=g++
LDFLAGS=-L../util -lm -lpthread -lesidesocket -lesidethread 
INCLUDE=-I../util

main: EchoTcpListener EchoTcpClient NameServer NameClient PasapalabraTcpListener PasapalabraTcpClient

EchoTcpListener: EchoServerThread.o EchoTcpListener.o
	$(CC) EchoServerThread.o EchoTcpListener.o $(LDFLAGS) -o EchoTcpListener

EchoTcpClient: EchoTcpClient.o
	$(CC) EchoTcpClient.o $(LDFLAGS) -o EchoTcpClient

NameServer: NameServer.o
	$(CC) NameServer.o $(LDFLAGS) -lesidesqlite -o NameServer

NameClient: NameClient.o
	$(CC) NameClient.o $(LDFLAGS) -o NameClient

EchoServerThread.o: EchoServerThread.cc EchoServerThread.h
	$(CC) $(INCLUDE) -c EchoServerThread.cc

EchoTcpListener.o: EchoTcpListener.cc
	$(CC) $(INCLUDE) -c EchoTcpListener.cc

EchoTcpClient.o: EchoTcpClient.cc
	$(CC) $(INCLUDE) -c EchoTcpClient.cc

NameServer.o: NameServer.cc NameServer.h
	$(CC) $(INCLUDE) -c NameServer.cc

NameClient.o: NameClient.cc 
	$(CC) $(INCLUDE) -c NameClient.cc

PasapalabraTcpListener: PasapalabraServerThread.o PasapalabraTcpListener.o
	$(CC) PasapalabraServerThread.o PasapalabraTcpListener.o $(LDFLAGS) -lesideppalsqlite -o PasapalabraTcpListener

PasapalabraTcpClient: PasapalabraTcpClient.o
	$(CC) PasapalabraTcpClient.o $(LDFLAGS) -o PasapalabraTcpClient

PasapalabraServerThread.o: PasapalabraServerThread.cc PasapalabraServerThread.h
	$(CC) $(INCLUDE) -c PasapalabraServerThread.cc

PasapalabraTcpListener.o: PasapalabraTcpListener.cc
	$(CC) $(INCLUDE) -c PasapalabraTcpListener.cc

PasapalabraTcpClient.o: PasapalabraTcpClient.cc
	$(CC) $(INCLUDE) -c PasapalabraTcpClient.cc


clean:
	rm *.o
	rm EchoTcpListener EchoTcpClient NameServer NameClient PasapalabraTcpListener PasapalabraTcpClient
	rm *.exe
