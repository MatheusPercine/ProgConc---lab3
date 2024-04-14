// Programa que realiza a multiplicação de matrizes de forma sequencial
/*
Este programa recebe duas matrizes de entrada no formato de arquivos binários e matriz resultante da
multiplicação é escrita em um arquivo binário de saida. Além disso, os tempos de inicialização, multiplicação,
finzaliçao e total são calculados.
*/

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

float *mat1; // primeira matriz de entrada
float *mat2; // segunda matriz de entrada
float *saida; // matriz de saída
int N; // numero de linhas
int M; // numero de colunas

// Descomentar a linha abaixo terá como efeito imprimir as matrizes da multiplicação e de saida
//#define TESTE

int main(int argc, char* argv[]) {
    double inicio, fim, delta, total; // variaveis de monitoramento de tempo

    //inicio da avaliacao do tempo de inicializacao e de tempo total
    GET_TIME(inicio);

    // leitura e avaliação dos parâmetros de entrada
    if (argc < 3) {
        printf("Digite: %s <matriz1 de entrada> <matriz2 de entrada> <saida>\n", argv[0]);
        return 1;
    }
   
   FILE * descritorArquivoSaida; //descritor do arquivo de saída
   FILE *descritorArquivoEntrada1; //descritor do arquivo da matriz1 de entrada
   FILE *descritorArquivoEntrada2; //descritor do arquivo da matriz2 de entrada
 
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

    //aloca memória para a matriz
    float *mat1 = (float*) malloc(sizeof(float) * N * M);
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
    float *mat2 = (float*) malloc(sizeof(float) * N * M);
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

    // alocação de memória para matriz de saída
    saida = malloc(sizeof(float) * N * M);
    if (saida == NULL) {
        printf("ERRO--malloc\n");
        return 7;
    }

    //fim da avaliacao do tempo de inicializacao
    GET_TIME(fim);
    delta = fim - inicio;
    total = delta;
    printf("Tempo inicializacao:%lf\n", delta);
    puts("");

    //multiplicacao da matriz pelo vetor
    //inicio da avaliacao do tempo de multiplicacao
    GET_TIME(inicio);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            for (int k = 0; k < N; k++) {
                saida[i * M + j] += mat1[i * M + k] * mat2[k * M + j];
            }
        }
    }

    //fim da avaliacao do tempo de multiplicacao
    GET_TIME(fim)   
    delta = fim - inicio;
    total += delta;
    printf("Tempo multiplicacao: %lf\n", delta);
    puts("");

    // exibição dos resultados
    #ifdef TESTE
    printf("Matriz 1 carregada:\n");
    for(int i=0; i<N; i++) {
      for(int j=0; j<M; j++){
        printf("%lf ", mat1[i*M+j]);
      }
      puts("");
   }
    puts("");
    printf("Matriz 2 carregada:\n");
   for(int i=0; i<N; i++) {
      for(int j=0; j<M; j++){
        printf("%lf ", mat2[i*M+j]);
      }
      puts("");
   }
    puts("");
    printf("Matriz de saida gerada:\n");
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            printf("%lf ", saida[i * M + j]); // imprimindo a matriz de saída
        }
        puts("");
    }
    puts("");
    #endif

    //inicio da avaliacao do tempo de finalizacao
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
    fclose(descritorArquivoEntrada1);
    fclose(descritorArquivoEntrada2);
    fclose(descritorArquivoSaida);

    //fim da avaliacao do tempo de finalizacao e tempo total
    GET_TIME(fim); 
    delta = fim - inicio;
    total += delta;
    printf("Tempo finalizacao:%lf\n", delta);
    printf("\nTempo total:%lf\n", total);
    return 0;
}