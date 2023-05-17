#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <math.h>

int primo (int n) {
    int i;
	for (i = 3; i < (int)(sqrt(n) + 1); i+=2) {
			if(n%i == 0) return 0;
	}
	return 1;
}

int main(int argc, char *argv[]) { /* mpi_primosbag.c  */
    double t_inicial, t_final;
    int cont = 0, total = 0;
    int i, n;
    int meu_ranque, num_procs, inicio, dest, raiz=0, tag=1, stop=0;
    int vetor[num_procs-1];
    MPI_Status estado;
    MPI_Request request;
    int tamanho;
    /* Verifica o número de argumentos passados */
	if (argc < 2) {
        printf("Entre com o valor do maior inteiro como parâmetro para o programa.\n");
       	 return 0;
    } else {
        n = strtol(argv[1], (char **) NULL, 10);
    }
    
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &meu_ranque);
	MPI_Comm_size(MPI_COMM_WORLD, &num_procs);
    tamanho = (n-3)/(num_procs-1);

/* Se houver menos que dois processos aborta */
    if (num_procs < 2) {
        printf("Este programa deve ser executado com no mínimo dois processos.\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
       	return(1);
    }
    /* Registra o tempo inicial de execução do programa */
    t_inicial = MPI_Wtime();
    /* Envia pedaços com tamanho números para cada processo */
    if (meu_ranque != 0) {
        /* Espera uma mensagem com da onde comecar a contagem do processo principal*/
        MPI_Irecv(&inicio, 1, MPI_INT, raiz, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
    } 
    
    MPI_Barrier(MPI_COMM_WORLD);

    if (meu_ranque == 0) {
        /* Envia pedaços com tamanho números para cada processo */
        for (dest=1; dest<num_procs; dest++) {
            inicio = 3 + (dest-1)*tamanho;
            MPI_Rsend(&inicio, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        }

        /* Fica recebendo as contagens parciais de cada processo */
        for (i=1; i<num_procs; i++) {
            MPI_Irecv(&vetor[i-1], 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &request);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (meu_ranque != 0) {
        /* Cada processo escravo faz a contagem dos primos no intervalo */
        for (i=inicio; i<inicio+tamanho; i+=2) {
            if (primo(i)) cont++;
        }
        /* Ultimo processo calcula o resto */
        if (meu_ranque == num_procs-1) {
            for (i=inicio+tamanho; i<=n; i+=2) {
                if (primo(i)) cont++;
            }
        }
            
        /* Envia a contagem parcial para o processo mestre */
        MPI_Rsend(&cont, 1, MPI_INT, raiz, tag, MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (meu_ranque == 0) {
        /*Faz a contagem de todos os resultados parciais*/
        for (i=0; i<num_procs-1; i++) {
            total += vetor[i];
        }
    }

	if (meu_ranque == 0) {
		t_final = MPI_Wtime();
        total += 1;    /* Acrescenta o 2, que é primo */
		printf("Quant. de primos entre 1 e %d: %d \n", n, total);
		printf("Tempo de execucao: %1.3f \n", t_final - t_inicial);  	 
	}
/* Finaliza o programa */
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return(0);
}