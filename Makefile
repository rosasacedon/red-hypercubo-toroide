DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIR := dirs/

CC := mpicc
CR := mpirun


all : dirs RedToroide RedHipercubo

dirs:
	mkdir -p $(DIROBJ) $(DIREXE) $(DIRHEA) $(DIR)

RedToroide: 
	$(CC) RedToroide.c -o $(DIROBJ)/RedToroide.o
	$(CC) RedToroide.c -o $(DIREXE)/RedToroide

RedHipercubo: 
	$(CC) RedHipercubo.c -o $(DIROBJ)/RedHipercubo.o
	$(CC) RedHipercubo.c -o $(DIREXE)/RedHipercubo

testToroide:
	$(CR) -n 16 ./$(DIREXE)RedToroide

testHipercubo:
	$(CR) -n 8 ./$(DIREXE)RedHipercubo

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA) 

cleanDirs : 
	rm -rf *~ core $(DIR)
