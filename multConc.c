// Programa que realiza a multiplicação de matrizes de forma concorrente

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

float *mat1; // primeira matriz de entrada
float *mat2; // segunda matriz de entrada
float *saida; // matriz de saída
int N; // numero de linhas
int M; // numero de colunas
int nthreads; // número de threads

// Descomentar a linha abaixo terá como efeito imprimir as matrizes da multiplicação e de saida
//#define TESTE

typedef struct{
    int id;
    int N;
    int M;
} tArgs;

void *multiplica(void *arg) {
    tArgs *args = (tArgs*) arg;

    for (int i = args->id; i < args->N; i += nthreads) {
        for (int j = 0; j < args->M; j++) {
            for (int k = 0; k < args->N; k++) {
                saida[i * args->M + j] += mat1[i * args->M + k] * mat2[k * args->M + j];
            }
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    pthread_t *tid; // identificadores das threads
    tArgs *args; // identificadores locais das threads e dimensão
    double inicio, fim, delta, total; // variaveis de monitoramento de tempo

    GET_TIME(inicio);
    // leitura e avaliação dos parâmetros de entrada
    if (argc < 4) {
        printf("Digite: %s <matriz1 de entrada> <matriz2 de entrada> <saida> <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[4]);
   
   FILE * descritorArquivoSaida; //descritor do arquivo de saída
   FILE *descritorArquivoEntrada1;
   FILE *descritorArquivoEntrada2;
   

    //inicializar mat1
    //abre o arquivo de entrada para leitura binaria
    descritorArquivoEntrada1 = fopen(argv[1], "rb");
    if(!descritorArquivoEntrada1) {
        fprintf(stderr, "Erro de abertura do arquivo de entrada\n");
        return 2;
    }

    //le as dimensoes da matriz e calcula a qtde de elementos da matriz
    size_t ret = fread(&N, sizeof(int), 1, descritorArquivoEntrada1);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }
    ret = fread(&M, sizeof(int), 1, descritorArquivoEntrada1);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }
    
    if (nthreads > N) nthreads=N;

    //aloca memória para a matriz
    mat1 = (float*) malloc(sizeof(float) * N * M);
    if(!mat1) {
        fprintf(stderr, "Erro de alocação de memória da matriz\n");
        return 3;
    }

    //carrega a matriz de elementos do tipo float do arquivo
    ret = fread(mat1, sizeof(float), N * M, descritorArquivoEntrada1);
    if(ret < N * M) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return 4;
    }

    #ifdef TESTE
    printf("Matriz 1 carregada:\n");
    for(int i=0; i<N; i++) {
      for(int j=0; j<M; j++){
        printf("%lf ", mat1[i*M+j]);
      }
      puts("");
   }
    puts("");
    #endif

    // alocação de memória para matriz de saída
    saida = malloc(sizeof(float) * N * M);
    if (saida == NULL) {
        printf("ERRO--malloc\n");
        return 7;
    }

    //inicializar mat2
    //abre o arquivo de entrada para leitura binaria
    descritorArquivoEntrada2 = fopen(argv[2], "rb");
    if(!descritorArquivoEntrada2) {
        fprintf(stderr, "Erro de abertura do arquivo de entrada\n");
        return 2;
    }

    //le as dimensoes da matriz e calcula a qtde de elementos da matriz
    ret = fread(&N, sizeof(int), 1, descritorArquivoEntrada2);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }
    ret = fread(&M, sizeof(int), 1, descritorArquivoEntrada2);
    if(!ret) {
        fprintf(stderr, "Erro de leitura das dimensoes da matriz arquivo \n");
        return 3;
    }

    //aloca memória para a matriz
    mat2 = (float*) malloc(sizeof(float) * N * M);
    if(!mat2) {
        fprintf(stderr, "Erro de alocação de memória da matriz\n");
        return 3;
    }

    //carrega a matriz de elementos do tipo float do arquivo
    ret = fread(mat2, sizeof(float), N * M, descritorArquivoEntrada2);
    if(ret < N * M) {
        fprintf(stderr, "Erro de leitura dos elementos da matriz\n");
        return 4;
    }

    #ifdef TESTE
    printf("Matriz 2 carregada:\n");
   for(int i=0; i<N; i++) {
      for(int j=0; j<M; j++){
        printf("%lf ", mat2[i*M+j]);
      }
      puts("");
   }
    puts("");
    #endif

    GET_TIME(fim);
    delta = fim - inicio;
    total = delta;
    printf("Tempo inicializacao:%lf\n", delta);
    puts("");

    //multiplicacao da matriz pelo vetor
    GET_TIME(inicio);
    // alocação das estruturas
    tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        printf("ERRO--malloc\n");
        return 8;
    }
    args = (tArgs *) malloc(sizeof(tArgs) * nthreads);
    if (args == NULL) {
        printf("ERRO--malloc\n");
        return 9;
    }

    // criação das threads
    for (int i = 0; i < nthreads; i++) {
        (args + i)->id = i;
        (args + i)->N = N;
        (args + i)->M = M;
        if(pthread_create(tid + i, NULL, multiplica, (void *) (args + i))) {
            printf("ERRO--pthread_create\n");
            return 10;
        }
    }

    // espera pelo término das threads
    for (int i = 0; i < nthreads; i++) {
        pthread_join(*(tid + i), NULL);
    }

    GET_TIME(fim)   
    delta = fim - inicio;
    total += delta;
    printf("Tempo multiplicacao: %lf\n", delta);
    puts("");

    // exibição dos resultados
    #ifdef TESTE
    printf("Matriz de saida gerada:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            printf("%lf ", saida[i * M + j]); // imprimindo a matriz de saída
        }
        puts("");
    }
    puts("");
    #endif

    GET_TIME(inicio);
    // abre o arquivo de saída para escrita binária
    descritorArquivoSaida = fopen(argv[3], "wb");
    if(!descritorArquivoSaida) {
        fprintf(stderr, "Erro de abertura do arquivo de saida\n");
        return 11;
    }

    //escreve numero de linhas e de colunas
    ret = fwrite(&N, sizeof(int), 1, descritorArquivoSaida);
    ret = fwrite(&M, sizeof(int), 1, descritorArquivoSaida);

    // escreve a matriz de saída no arquivo binário
    ret = fwrite(saida, sizeof(float), N * M, descritorArquivoSaida);
    if(ret < N * M) {
        fprintf(stderr, "Erro de escrita no arquivo de saida\n");
        return 12;
    }

    // liberação de memória e fechamento dos arquivos
    free(mat1);
    free(mat2);
    free(saida);
    free(args);
    free(tid);
    fclose(descritorArquivoEntrada1);
    fclose(descritorArquivoEntrada2);
    fclose(descritorArquivoSaida);
    GET_TIME(fim);   
    delta = fim - inicio;
    total += delta;
    printf("Tempo finalizacao:%lf\n", delta);
    printf("\nTempo total:%lf\n", total);
    return 0;
}