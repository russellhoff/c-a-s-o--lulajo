# Makefile
# author: dipina@eside.deusto.es
CC=g++ -fPIC
LDFLAGS=-L../util -lm -lpthread -lesidesocket -lesidethread
INCLUDE=-I../util

main: libesidedsm.so DsmServer DsmTest MatrixEvenAdder MatrixOddAdder DsmTimeServer DsmTimeClient RecursosDsmClient

libesidedsm.so: Dsm.o
	$(CC) -shared $(LDFLAGS) Dsm.o -o libesidedsm.so
	cp libesidedsm.so libesidedsm.dll
	cp libesidedsm.* ../util/
        
Dsm.o: Dsm.cc Dsm.h DsmTypes.h
	$(CC) $(INCLUDE) -c Dsm.cc
        
DsmServer: DsmServer.o
	$(CC)  DsmServer.o $(LDFLAGS) -o DsmServer
        
DsmTest: DsmTest.o
	$(CC) DsmTest.o -L. -lesidedsm $(LDFLAGS) -o DsmTest
        
MatrixEvenAdder: MatrixEvenAdder.o
	$(CC) MatrixEvenAdder.o -L. -lesidedsm $(LDFLAGS) -o MatrixEvenAdder
        
MatrixOddAdder: MatrixOddAdder.o
	$(CC) MatrixOddAdder.o -L. -lesidedsm $(LDFLAGS) -o MatrixOddAdder
	
RecursosDsmClient: RecursosDsmClient.o
	$(CC) RecursosDsmClient.o -L. -lesidedsm $(LDFLAGS) -o RecursosDsmClient	
        
DsmServer.o: DsmServer.cc DsmServer.h DsmTypes.h
	$(CC) $(INCLUDE) -c DsmServer.cc

DsmTest.o: DsmTest.cc
	$(CC) $(INCLUDE) -c DsmTest.cc
        
MatrixEvenAdder.o: MatrixEvenAdder.cc
	$(CC) $(INCLUDE) -c MatrixEvenAdder.cc
        
MatrixOddAdder.o: MatrixOddAdder.cc
	$(CC) $(INCLUDE) -c MatrixOddAdder.cc
	
RecursosDsmClient.o: RecursosDsmClient.cc
	$(CC) $(INCLUDE) -c RecursosDsmClient.cc	
        
DsmTimeServer: DsmTimeServer.o
	$(CC) DsmTimeServer.o -L. -lesidedsm $(LDFLAGS) -o DsmTimeServer
        
DsmTimeClient: DsmTimeClient.o
	$(CC) DsmTimeClient.o -L. -lesidedsm $(LDFLAGS) -o DsmTimeClient
	
DsmTimeServer.o: DsmTimeServer.cc
	$(CC) $(INCLUDE) -c DsmTimeServer.cc
	
DsmTimeClient.o: DsmTimeClient.cc
	$(CC) $(INCLUDE) -c DsmTimeClient.cc

clean:
	rm *.o
	rm *.so
	rm *.dll
	rm DsmServer DsmTest MatrixEvenAdder MatrixOddAdder DsmTimeServer DsmTimeClient RecursosDsmClient