#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define MAX_DATOS 1024

#define NORTE 0
#define SUR 1
#define ESTE 2
#define OESTE 3
#define L 4

void calcularNumMinimo(int rank, double num);
int* vecinosToroide(int nodo);
int contar();

int main(int argc, char *argv[]){
    int i=0;
    int* vecinoToroide;
    int nodoT=7;
    int datos;
    int continuar=0;
    double num = 0;
    double new_num = 0;
    double values[L*L];
    double value;
    char* char_fichero;
    char line[80];
    char *token;
    const char separator[2] = ",";
    FILE *archivo;

    MPI_Init(&argc,&argv); 
    MPI_Status status;
    int size,rank;

    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);


    if(rank==0){
        datos=contar();

        if(datos>size){
            continuar=1;
            printf("El numero de nodos a ejecutar es menor que el numero de datos\r\n");
            MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
        }else{
            if(L*L==size){
                continuar=0;
                printf("El numero de nodos a ejecutar correcto\r\n");
                MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
            }else{
                continuar=1;
                printf("El numero de nodos ejecutados es distinto a los nodos requeridos\r\n");
                MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
            }
            
        }

        if(continuar==0){
        
        if((archivo=fopen("./dirs/datos.dat","r"))==NULL){
            printf("Error al abrir el fichero\n");
            MPI_Finalize();
            exit(EXIT_FAILURE);
        }

        char_fichero=fgets(line,MAX_DATOS*sizeof(char),archivo);

        token = strtok(line, separator);

        while( token != NULL ) { 
            values[i++]=atof(token);
            token = strtok(NULL, separator);
        }

        fclose(archivo);

        for(i=1;i<(L*L);i++){
            num=values[i];
            MPI_Bsend(&num,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
        }
        num=values[0];
        vecinoToroide=vecinosToroide(rank);
        calcularNumMinimo(rank, num);
        }
    }else{

        MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
        if(continuar==0){

        MPI_Recv(&num,1,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        vecinoToroide=vecinosToroide(rank);
        calcularNumMinimo(rank, num);
        }
    }

    MPI_Finalize();
   
   return EXIT_SUCCESS;
}

void calcularNumMinimo(int rank, double num){
    int i;
    int *vecinoToroide;
    MPI_Status status;
    vecinoToroide = vecinosToroide(rank);
    double new_num;
    
    for (i=0;i<L;i++){
            MPI_Bsend(&num,1,MPI_DOUBLE,vecinoToroide[NORTE],0,MPI_COMM_WORLD);
            MPI_Recv(&new_num,1,MPI_DOUBLE,vecinoToroide[SUR],MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            if(new_num<num){
                num=new_num;
            }
        }
        for (i=0;i<L;i++){
            MPI_Bsend(&num,1,MPI_DOUBLE,vecinoToroide[ESTE],0,MPI_COMM_WORLD);
            MPI_Recv(&new_num,1,MPI_DOUBLE,vecinoToroide[OESTE],MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            if(new_num<num){
                num=new_num;
            }
        }

    if(rank==0){
        printf("El número minimo es: %f\n",num);
    }

}


int* vecinosToroide(int nodo){

    static int vecino [L];

    int modulo = nodo%L;
    int division  = nodo/L;

    /*Localizar vecino de ESTE y OESTE*/
    
    if(modulo == L-1) {

	vecino[ESTE]=nodo-1;
        vecino[OESTE]=nodo-(L-1);

    } else if(modulo == 0) {

	vecino[ESTE]=nodo+(L-1);
        vecino[OESTE]=nodo+1;

	}else {
           vecino[ESTE]=nodo-1;
           vecino[OESTE]=nodo+1;
	}

    /*Localizar vecino de NORTE y SUR*/
    
    if(division == L-1) {

        vecino[SUR]=nodo-L;
        vecino[NORTE]=nodo-(L*(L-1));

    } else if(division == 0) {

        vecino[SUR]=nodo+(L*(L-1));
        vecino[NORTE]=nodo+L;

	}else {
	   vecino[SUR]=nodo-L;
           vecino[NORTE]=nodo+L;
	}

    return vecino;
}

int contar(){
    int datos=0;

    char* char_fichero;
    char line[80];
    char *token;
    const char separator[2] = ",";
    FILE *archivo;
    
    if((archivo=fopen("./dirs/datos.dat","r"))==NULL){
            printf("Error al abrir el fichero\n");
            exit(EXIT_FAILURE);
        }

        char_fichero=fgets(line,MAX_DATOS*sizeof(char),archivo);

        token = strtok(line, separator);

        while( token != NULL ) { 
            datos++;
            token = strtok(NULL, separator);
        }

        fclose(archivo);

        return datos;
}
