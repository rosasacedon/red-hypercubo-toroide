#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

#define MAX_DATOS 1024
#define D 3

int* vecinosHipercubo(int nodo);
double maximo(double a, double b);
int contador();
void calcularNumMaximo(int rank, double num);


int main(int argc, char *argv[]){
    int i=0;
    int nodoHiper=0;
    int* vecinoHipercubo;
    double num = 0;
    double new_num = 0;
    int tamVect=pow(2,D);
    double values[tamVect];
    char* char_fichero;
    double value;
    char line[80];
    char *token;
    const char separator[2] = ",";
    FILE *archivo;
    int indice_char=0;
    MPI_Status status;
    int size,rank;
    int datos;
    int continuar=0;

    MPI_Init(&argc,&argv); 
   
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    if(rank==0){
         datos=contador();

        if(datos>size){
            continuar=1;
            printf("El numero de nodos a ejecutar es menor que el numero de datos\r\n");
            MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
        }else{
            if(pow(2,D)==size){
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
            exit(EXIT_FAILURE);
        }

        char_fichero=fgets(line,MAX_DATOS*sizeof(char),archivo);

        token = strtok(line, separator);

        while( token != NULL ) { 
            values[i++]=atof(token);
            token = strtok(NULL, separator);
        }

        fclose(archivo);

        for(i=1;i<tamVect;i++){
            num=values[i];
            MPI_Bsend(&num,1,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
        }
        num=values[0];
        
        vecinoHipercubo=vecinosHipercubo(rank);
        calcularNumMaximo(rank, num);
    	}
    }else{
        MPI_Bcast(&continuar,1,MPI_INT,0,MPI_COMM_WORLD);
        if(continuar==0){

        MPI_Recv(&num,1,MPI_DOUBLE,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        
        vecinoHipercubo=vecinosHipercubo(rank);
        calcularNumMaximo(rank, num);
        }
    }

    MPI_Finalize();
   
   return EXIT_SUCCESS;
}

void calcularNumMaximo(int rank, double num){
    int i;
    int *vecinoHipercubo;
    MPI_Status status;
    vecinoHipercubo = vecinosHipercubo(rank);
    double new_num;

    for (i=0;i<D;i++){
            MPI_Bsend(&num,1,MPI_DOUBLE,vecinoHipercubo[i],0,MPI_COMM_WORLD);
            MPI_Recv(&new_num,1,MPI_DOUBLE,vecinoHipercubo[i],MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            num=maximo(new_num,num);
    }

    if(rank==0){
        printf("El numero maximo es: %.2f\n",num);
    }

}

double maximo(double a, double b){
    if(a>b){
        return a;
    }
    else{
        return b;
    }

}

int* vecinosHipercubo(int nodo){
    static int vecinos [D];
    int i=0;
    int desplazamiento=1;

    for (i=0;i<D;i++){
        vecinos[i]=nodo^desplazamiento;
        desplazamiento=desplazamiento<<1;
    }
    return vecinos;
}

int contador(){
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
