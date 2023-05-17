#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo (long int n) { /* mpi_primos.c  */
    int i;

    for (i = 3; i < (int)(sqrt(n) + 1); i+=2) {
            if(n%i == 0) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    double t_inicial, t_final;
    int cont = 0, total = 0;
    long int i, n;
    int meu_ranque, num_procs, inicio, salto;
    int vetor[num_procs-1];
    MPI_Request request;
    MPI_Status estado;

    if (argc < 2) {
            printf("Valor inválido! Entre com um valor do maior inteiro\n");
                return 0;
        } else {
            n = strtol(argv[1], (char **) NULL, 10);
           }

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    t_inicial = MPI_Wtime();
    inicio = 3 + meu_ranque*2;
    salto = num_procs*2;
    for (i = inicio; i <= n; i += salto) 
    {
        if(primo(i) == 1) cont++;
    }


    if(meu_ranque == 0){
        total = cont;
        
        for (i=1; i<num_procs; i++) {
            MPI_Irecv(&vetor[i-1], 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        }
    }    
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    if(meu_ranque != 0) {
        MPI_Rsend(&cont, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    
    if (meu_ranque == 0)
    {
        for (i=0; i<num_procs-1; i++) {
            total += vetor[i];
        }
    }
    

    t_final = MPI_Wtime();

    if (meu_ranque == 0) {
        total += 1;    /* Acrescenta o dois, que também é primo */
        printf("Quant. de primos entre 1 e n: %d \n", total);
        printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);
    }
    MPI_Finalize();
    return(0);
}