#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void display(int src, int dest){
    char str[100];
    sprintf(str, "M(%d,%d)", src, dest);
    printf("%s\n", str);
}

void calculate(int length_array, int *pos, int nr_calc, int *v, int vx){
    MPI_Send(&length_array, 1, MPI_INT, vx, 0,  MPI_COMM_WORLD);
    if((*pos) + nr_calc < length_array){
        MPI_Send(pos, 1, MPI_INT, vx, 0,  MPI_COMM_WORLD);
        MPI_Send(&nr_calc, 1, MPI_INT, vx, 0,  MPI_COMM_WORLD);
        MPI_Send(v, length_array, MPI_INT, vx, 0,  MPI_COMM_WORLD);
        MPI_Recv(v, length_array, MPI_INT, vx, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        (*pos) += nr_calc;
    }
    else {
        int nr_calc_final = length_array - (*pos);
        MPI_Send(pos, 1, MPI_INT, vx, 0,  MPI_COMM_WORLD);
        MPI_Send(&nr_calc_final, 1, MPI_INT, vx, 0,  MPI_COMM_WORLD);
        MPI_Send(v, length_array, MPI_INT, vx, 0,  MPI_COMM_WORLD);
        MPI_Recv(v, length_array, MPI_INT, vx, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        (*pos) = length_array;
    }
}

void send_v_to_next(int length_array, int pos, int nr_calc, int rank, int *v){
    MPI_Send(&length_array, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
    MPI_Send(&pos, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
    MPI_Send(&nr_calc, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
    MPI_Send(v, length_array, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
}

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int nr;
    int worker;
   
    if(rank == 0){
        
        int disconnected = atoi(argv[2]);
        display(rank, 3);
        MPI_Send(&disconnected, 1, MPI_INT, 3, 0,  MPI_COMM_WORLD);

        if(disconnected == 0){
            FILE *file;
            file = fopen("cluster0.txt", "r");
            fscanf(file, "%d", &nr);
            int v0[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v0[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);
            char message[10000];
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);
            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v0[j]);
                strcat(message, str);
            }
            char str2[10];
            sprintf(str2, "%d ", v0[nr-1]);
            strcat(message, str2);
            display(rank, rank+1);
            MPI_Send(&message, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);
        
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i ++){
                display(rank, v0[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v0[i], 0,  MPI_COMM_WORLD);
            }

            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);
            display(rank, rank+1);
            MPI_Send(&message_done, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);
            
            //cerinta 2
            int length_array = atoi(argv[1]);
            int v[length_array];

            for(int i = 0; i < length_array; i++)
                v[i] = length_array - i - 1;

            int nr_calc;  
            nr_calc = length_array / (numtasks - 4);

            int pos = 0;

            for(int i = 0; i < nr; i++){
                display(rank, v0[i]);
                calculate(length_array, &pos, nr_calc, v, v0[i]);  
            }

            display(rank, rank+1);
            send_v_to_next(length_array, pos, nr_calc, rank, v);

            MPI_Recv(v, length_array, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pos, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i++){
                display(rank, v0[i]);
                calculate(length_array, &pos, 1, v, v0[i]);  
            }
            display(rank, rank+1);
            MPI_Send(v, length_array, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
            MPI_Send(&pos, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);

            MPI_Recv(v, length_array, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("Rezultat: ");
            for(int i = 0; i < length_array; i++)
                printf("%d ", v[i]);
            printf("\n");
        }  
        //cerinta 3
        else {
            FILE *file;
            file = fopen("cluster0.txt", "r");
            fscanf(file, "%d", &nr);
            int v0[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v0[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);
            char message[10000];
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);
            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v0[j]);
                strcat(message, str);
            }
            char str2[10];
            sprintf(str2, "%d ", v0[nr-1]);
            strcat(message, str2);

            display(rank, 3);
            MPI_Send(&message, 10000, MPI_CHAR, 3, 0,  MPI_COMM_WORLD);
        
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i ++){
                display(rank, v0[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v0[i], 0,  MPI_COMM_WORLD);
            }

            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);
            display(rank, 3);
            MPI_Send(&message_done, 10000, MPI_CHAR, 3, 0,  MPI_COMM_WORLD);
            
            //cerinta 2
            int length_array = atoi(argv[1]);
            int v[length_array];

            for(int i = 0; i < length_array; i++)
                v[i] = length_array - i - 1;

            int nr_calc;
            nr_calc = length_array / (numtasks - 4);

            int pos = 0;

            for(int i = 0; i < nr; i++){
                display(rank, v0[i]);
                calculate(length_array, &pos, nr_calc, v, v0[i]);  
            }

            display(rank, 3);
            send_v_to_next(length_array, pos, nr_calc, 2, v);

            MPI_Recv(v, length_array, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pos, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i++){
                display(rank, v0[i]);
                calculate(length_array, &pos, 1, v, v0[i]);
            }
            printf("Rezultat: ");
            for(int i = 0; i < length_array; i++)
               printf("%d ", v[i]);
            printf("\n");
        }  
    }

    if(rank == 1){

        int disconnected;
        MPI_Recv(&disconnected, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
       
        if(disconnected == 0){
            FILE *file;
            file = fopen("cluster1.txt", "r");
            fscanf(file, "%d", &nr);
            int v1[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v1[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);
            char message[10000];
            MPI_Recv(&message, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);
            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v1[j]);
                strcat(message, str);
            }
            char str2[10];
            sprintf(str2, "%d ", v1[nr-1]);
            strcat(message, str2);
            display(rank, rank+1);
            MPI_Send(&message, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);
        
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i ++){
                display(rank, v1[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v1[i], 0,  MPI_COMM_WORLD);
            }
            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);
            display(rank, rank+1);
            MPI_Send(&message_done, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);

            //cerinta 2
            int length_array;
            MPI_Recv(&length_array, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int pos, nr_calc, v[length_array];
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&nr_calc, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i++){
                display(rank, v1[i]);
                calculate(length_array, &pos, nr_calc, v, v1[i]);  
            }

            display(rank, rank+1);
            send_v_to_next(length_array, pos, nr_calc, rank, v);

            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i++){
                display(rank, v1[i]);
                calculate(length_array, &pos, 1, v, v1[i]);  
            }
            display(rank, rank+1);
            MPI_Send(v, length_array, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
            MPI_Send(&pos, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
        }
        // cerinta 3
        else {
            FILE *file;
            file = fopen("cluster1.txt", "r");
            fscanf(file, "%d", &nr);
            int v1[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v1[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);
            char message[10000];
            MPI_Recv(&message, 10000, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);
            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v1[j]);
                strcat(message, str);
            }
            char str2[10];
            sprintf(str2, "%d ", v1[nr-1]);
            strcat(message, str2);

            display(rank, rank+1);
            MPI_Send(&message, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);
        
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i ++){
                display(rank, v1[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v1[i], 0,  MPI_COMM_WORLD);
            }
            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);
            

            //cerinta 2
            int length_array;
            MPI_Recv(&length_array, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int pos, nr_calc, v[length_array];
            MPI_Recv(&pos, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&nr_calc, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(v, length_array, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i++){
                display(rank, v1[i]);
                calculate(length_array, &pos, nr_calc, v, v1[i]);  
            }

            for(int i = 0; i < nr; i++){
                display(rank, v1[i]);
                calculate(length_array, &pos, 1, v, v1[i]);  
            }

            display(rank, rank+1);
            MPI_Send(v, length_array, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
            MPI_Send(&pos, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
        }
        
    }

    if(rank == 2){
        
        int disconnected;
        MPI_Recv(&disconnected, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        display(rank, rank-1);
        MPI_Send(&disconnected, 1, MPI_INT, rank-1, 0,  MPI_COMM_WORLD);

        if(disconnected == 0){
            FILE *file;
            file = fopen("cluster2.txt", "r");
            fscanf(file, "%d", &nr);
            int v2[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v2[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);

            char str[100];
            
            sprintf(str, "M(%d,%d)", rank, rank+1);
            char message[10000];
            MPI_Recv(&message, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);
            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v2[j]);
                strcat(message, str);
            }
            char str2[10];
            sprintf(str2, "%d ", v2[nr-1]);
            strcat(message, str2);
            display(rank, rank+1);
            MPI_Send(&message, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);
        
        
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i ++){
                display(rank, v2[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v2[i], 0,  MPI_COMM_WORLD);
            }
            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);
            display(rank, rank+1);
            MPI_Send(&message_done, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);

              //cerinta 2
            int length_array;
            MPI_Recv(&length_array, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int pos, nr_calc, v[length_array];
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&nr_calc, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i++){
                display(rank, v2[i]);
                calculate(length_array, &pos, nr_calc, v, v2[i]);
            }

           
            display(rank, rank+1);
            send_v_to_next(length_array, pos, nr_calc, rank, v);

            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i++){
                display(rank, v2[i]);
                calculate(length_array, &pos, 1, v, v2[i]);  
            }
            display(rank, rank+1);
            MPI_Send(v, length_array, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
            MPI_Send(&pos, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
        }
        // cerinta 3
        else{
            FILE *file;
            file = fopen("cluster2.txt", "r");
            fscanf(file, "%d", &nr);
            int v2[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v2[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);
            
            char message[10000];
            MPI_Recv(&message, 10000, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            display(rank, rank-1);
            MPI_Send(&message, 10000, MPI_CHAR, rank-1, 0,  MPI_COMM_WORLD);
            MPI_Recv(&message, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);

            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v2[j]);
                strcat(message, str);
            }

            char str2[10];
            sprintf(str2, "%d ", v2[nr-1]);
            strcat(message, str2);
            display(rank, rank+1);
            MPI_Send(&message, 10000, MPI_CHAR, rank+1, 0,  MPI_COMM_WORLD);
         
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i ++){
                display(rank, v2[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v2[i], 0,  MPI_COMM_WORLD);
            }
            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);
            display(rank, rank-1);
            MPI_Send(&message_done, 10000, MPI_CHAR, rank-1, 0,  MPI_COMM_WORLD);

              //cerinta 2
            int length_array;
            MPI_Recv(&length_array, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int pos, nr_calc, v[length_array];
            MPI_Recv(&pos, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&nr_calc, 1, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(v, length_array, MPI_INT, rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i++){
                display(rank, v2[i]);
                calculate(length_array, &pos, nr_calc, v, v2[i]);
            }

           
            display(rank, rank-1);
            send_v_to_next(length_array, pos, nr_calc, 0, v);

            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i++){
                display(rank, v2[i]);
                calculate(length_array, &pos, 1, v, v2[i]);
            }
            display(rank, rank+1);
            MPI_Send(v, length_array, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
            MPI_Send(&pos, 1, MPI_INT, rank+1, 0,  MPI_COMM_WORLD);
         }
        
    }

    if(rank == 3){
        int disconnected;
        MPI_Recv(&disconnected, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        display(rank, rank-1);
        MPI_Send(&disconnected, 1, MPI_INT, rank-1, 0,  MPI_COMM_WORLD);
        if(disconnected == 0){
            FILE *file;
            file = fopen("cluster3.txt", "r");
            fscanf(file, "%d", &nr);
            int v3[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v3[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);

            char message[10000];
            MPI_Recv(&message, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);
            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v3[j]);
                strcat(message, str);
            }
            char str2[10];
            sprintf(str2, "%d\n", v3[nr-1]);
            strcat(message, str2);
            display(rank, 0);
            MPI_Send(&message, 10000, MPI_CHAR, 0, 0,  MPI_COMM_WORLD);
                  
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i ++){
                display(rank, v3[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v3[i], 0,  MPI_COMM_WORLD);
            }
            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);

             //cerinta 2
            int length_array;
            MPI_Recv(&length_array, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int pos, nr_calc, v[length_array];
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&nr_calc, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i++){
                display(rank, v3[i]);
                calculate(length_array, &pos, nr_calc, v, v3[i]);
            }
            display(rank, 0);
            MPI_Send(v, length_array, MPI_INT, 0, 0,  MPI_COMM_WORLD);
            MPI_Send(&pos, 1, MPI_INT, 0, 0,  MPI_COMM_WORLD);

            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i++){
                display(rank, v3[i]);
                calculate(length_array, &pos, 1, v, v3[i]);  
            }
            
            display(rank, 0);
            MPI_Send(v, length_array, MPI_INT, 0, 0,  MPI_COMM_WORLD);
        }
        // cerinta 3
        else{
            FILE *file;
            file = fopen("cluster3.txt", "r");
            fscanf(file, "%d", &nr);
            int v3[nr];
            for(int i = 0; i < nr; i++){
                fscanf(file, "%d", &worker);
                display(rank, worker);
                v3[i] = worker;
                MPI_Send(&rank, 1, MPI_INT, worker, 0,  MPI_COMM_WORLD);
                int son;
                MPI_Recv(&son, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            fclose(file);

            char message[10000];
            MPI_Recv(&message, 10000, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            display(rank, rank-1);
            MPI_Send(&message, 10000, MPI_CHAR, rank-1, 0,  MPI_COMM_WORLD);
            MPI_Recv(&message, 10000, MPI_CHAR, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            char s[10];
            sprintf(s, "%d:", rank);
            strcat(message, s);
            for(int j = 0; j < nr - 1; j++){
                char str[10];
                sprintf(str, "%d,", v3[j]);
                strcat(message, str);
            }
            char str2[10];
            sprintf(str2, "%d\n", v3[nr-1]);
            strcat(message, str2);
            display(rank, 0);
            MPI_Send(&message, 10000, MPI_CHAR, 0, 0,  MPI_COMM_WORLD);
                  
            char message_done[10000];
            char prefix[10];
            MPI_Recv(&message_done, 10000, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i ++){
                display(rank, v3[i]);
                MPI_Send(&message_done, 10000, MPI_CHAR, v3[i], 0,  MPI_COMM_WORLD);
            }
            sprintf(prefix, "%d -> ", rank);
            char message_displayed[10000];
            strcpy(message_displayed, prefix);
            strcat(message_displayed, message_done);
            printf("%s", message_displayed);
            display(rank, rank-1);
            MPI_Send(&message_done, 10000, MPI_CHAR, rank-1, 0,  MPI_COMM_WORLD);
             //cerinta 2
            int length_array;
            MPI_Recv(&length_array, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            int pos, nr_calc, v[length_array];
            MPI_Recv(&pos, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&nr_calc, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(v, length_array, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int i = 0; i < nr; i++){
                display(rank, v3[i]);
                calculate(length_array, &pos, nr_calc, v, v3[i]);
            }

            display(rank, 2);
            
            send_v_to_next(length_array, pos, nr_calc, 1, v);

            MPI_Recv(v, length_array, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pos, 1, MPI_INT, rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for(int i = 0; i < nr; i++){
                display(rank, v3[i]);
                calculate(length_array, &pos, 1, v, v3[i]);
            }
            display(rank, 0);
            MPI_Send(v, length_array, MPI_INT, 0, 0,  MPI_COMM_WORLD);
            MPI_Send(&pos, 1, MPI_INT, 0, 0,  MPI_COMM_WORLD);
            
        }
           
    }

    if(rank > 3) {
        int father;
        MPI_Recv(&father, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        display(rank, father);
        MPI_Send(&rank, 1, MPI_INT, father, 0,  MPI_COMM_WORLD);
        char message_done[10000];
        char prefix[10];
        MPI_Recv(&message_done, 10000, MPI_CHAR, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sprintf(prefix, "%d -> ", rank);
        char message_displayed[10000];
        strcpy(message_displayed, prefix);
        strcat(message_displayed, message_done);
        printf("%s", message_displayed);

        //cerinta 2
        int length_array;
        MPI_Recv(&length_array, 1, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int pos, nr_calc, v[length_array];
        MPI_Recv(&pos, 1, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&nr_calc, 1, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v, length_array, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < nr_calc; i++){
            int aux = v[pos + i];
            v[pos + i] = aux * 5;
        }
        display(rank, father);
        MPI_Send(v, length_array, MPI_INT, father, 0,  MPI_COMM_WORLD);
        
        MPI_Recv(&length_array, 1, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        MPI_Recv(&pos, 1, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&nr_calc, 1, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(v, length_array, MPI_INT, father, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for(int i = 0; i < nr_calc; i++){
            int aux = v[pos + i];
            v[pos + i] = aux * 5;
        }
        display(rank, father);
        MPI_Send(v, length_array, MPI_INT, father, 0,  MPI_COMM_WORLD);
        
    }
    
    MPI_Finalize();

}

