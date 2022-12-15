#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>

int main(int argc,char *argv[]){
    int FROM_WORKER=100;
    int FROM_MASTER=000;
    int nsize,rank,taskid,numtasks,rc,i,j,k,
    numworkers,// numero de procesadores que van a trabajar
    source,
    dest,
    mtype,
    rows,
    averow,
    extra,offset;
    int filas=20,columnas=20;
    double startwtime, endwtime;
    double matrizA[filas][columnas],matrizB[filas][columnas],MatrizR[filas][columnas],MASTER=0;
    MPI_Status status; //Se utiliza para guardar información sobre operaciones de recepción de mensajes
    MPI_Init(&argc,&argv); //Inicializa la parte MPI del programa
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);// Devuelve el id de proceso de esos procesos.
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);//Devuelve el número total de procesos
    if (numtasks<2)
    {
        printf("\n---------------------------------------\n");
        printf("  Se necesitan almenos 2 procesadores");
        printf("\n---------------------------------------\n");
        MPI_Abort(MPI_COMM_WORLD,rc);//Finaliza el entorno de ejecución de MPI
        exit(1);//sale del programa
    }
    numworkers=numtasks-1;
    if(taskid==MASTER){
        printf("\n---------------------------------------\n");
        printf("MPI ha comenzado con %d procesadores",numtasks);
        printf("\n---------------------------------------\n");
        printf("\n---------------------------------------\n");
        printf("Creando Matrices");
        printf("\n---------------------------------------\n");
        for (int i = 0; i < filas; i++)
        {
            for (int j = 0; j < columnas; j++)
            {
                matrizA[i][j]=rand () % (1000-0+1) + 0;
            }
            
        }
        for (int i = 0; i < filas; i++)
        {
            for (int j = 0; j < columnas; j++)
            {
                matrizB[i][j]=rand () % (1000-0+1) + 0;
            }
            
        }
        printf("\n---------------------------------------\n");
        printf("Se crearon las matrices");
        printf("\n---------------------------------------\n");
        startwtime = MPI_Wtime();
        averow=filas/numworkers;// se divide las filas en el numero de procesadores libres
        extra=filas%numworkers; // en caso de que la divicion no sea exacta
        offset=0;
        mtype=FROM_MASTER;
        printf("\n---------------------------------------\n");
        printf("Se comienza a enviar la matriz a los procesadores");
        printf("\n---------------------------------------\n");
        for ( dest = 1; dest <= numworkers; dest++)
        {
            rows=(dest<=extra)?averow+1:averow;// en el caso de que la divicion de averow no sea exacta se le suma una fila +
            printf("\n---------------------------------------\n");
            printf("Se envian %d filas al procesador %d",rows,dest);
            printf("\n---------------------------------------\n");
            MPI_Send(&offset,1,MPI_INT,dest,mtype,MPI_COMM_WORLD);
            MPI_Send(&rows,1,MPI_INT,dest,mtype,MPI_COMM_WORLD);
            MPI_Send(&matrizA[offset][0],rows*filas,MPI_DOUBLE,dest,mtype,MPI_COMM_WORLD);
            MPI_Send(&matrizB,filas*columnas,MPI_DOUBLE,dest,mtype,MPI_COMM_WORLD);
            offset=offset+rows;
        }
        mtype=FROM_WORKER;
        for (int i = 1; i <= numworkers; i++)
            {
            source=i;
            MPI_Recv(&offset,1,MPI_INT,source,mtype,MPI_COMM_WORLD,&status);
            MPI_Recv(&rows,1,MPI_INT,source,mtype,MPI_COMM_WORLD,&status);
            MPI_Recv(&MatrizR[offset][0],rows*filas,MPI_DOUBLE,source,mtype,MPI_COMM_WORLD,&status);
            printf("\n---------------------------------------\n");
            printf("Resultados recividos desde la tarea %d ",source);
            printf("\n---------------------------------------\n");
        }   
        startwtime = MPI_Wtime();
        printf("\n-----------------------------------------------------\n");
        printf("\nMatriz resultante\n");
        for (int i = 0; i < filas; i++)
        {
            printf("\n");
            for ( j = 0; j < columnas; j++)
            {
                printf("%6.2f ",MatrizR[i][j]);
            }
            
        }
        endwtime = MPI_Wtime(); //end compute time
        printf("\nTiempo de computo = %lf\n", endwtime - startwtime);
        printf("\n ----------------------------------------------------- \n");
        
        
    }
    if (taskid>MASTER)
    {   
        printf("\n---------------------------------------\n");
        printf("Procesador %d ha comenzado a tabajar",taskid);
        printf("\n---------------------------------------\n");
        mtype=FROM_MASTER;
        MPI_Recv(&offset,1,MPI_INT,MASTER,mtype,MPI_COMM_WORLD,&status);
        MPI_Recv(&rows,1,MPI_INT,MASTER,mtype,MPI_COMM_WORLD,&status);
        MPI_Recv(&matrizA,rows*filas,MPI_DOUBLE,MASTER,mtype,MPI_COMM_WORLD,&status);
        MPI_Recv(&matrizB,filas*columnas,MPI_DOUBLE,MASTER,mtype,MPI_COMM_WORLD,&status);
        for (int k = 0; k < columnas; k++)
            for (int i = 0; i < rows; i++)
            {
                MatrizR[i][k]=0.0;
                for (int j = 0; j < filas; j++){
                    MatrizR[i][k]+=matrizA[i][j]*matrizB[j][k];
                }
            }
        mtype=FROM_WORKER;
        
        MPI_Send(&offset,1,MPI_INT,MASTER,mtype,MPI_COMM_WORLD);
        MPI_Send(&rows,1,MPI_INT,MASTER,mtype,MPI_COMM_WORLD);
        MPI_Send(&MatrizR,rows*columnas,MPI_DOUBLE,MASTER,mtype,MPI_COMM_WORLD);
    }
    MPI_Finalize();
}          