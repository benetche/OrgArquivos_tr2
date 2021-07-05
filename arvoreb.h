#ifndef ARVOREB_H
#define ARVOREB_H
    #include <stdio.h>
    #include <stdlib.h>

    #define PAG_REGISTRO 77
    #define ORDEM 5
    #define LIXO "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"

    //registro de cabecalho
    typedef struct REG_CABECALHO{
        char status;
        int noRaiz;
        int RRNproxNo;
        const char lixo[68];

    }regCabecalho;

    //struct auxiliar para o conjunto CHAVE/PONTEIRO
    typedef struct CHAVES_PONTEIROS{
        int chave;
        long long int pontRegistro; //byte offset do registro(char removido)
    }CHAVES_PONTEIROS;

    //struct de no da arvore b
    typedef struct NO_ARVORE_B{
        char folha;
        int nroChavesIndexadas;
        int RRNdoNo;
        int ponteiros[ORDEM];
        CHAVES_PONTEIROS ponteirosRef[ORDEM-1];   
    }NO_ARVORE_B;

    //struct da arvore b
    //possui o arquivo da arvore b, numero de nos e registro de cabecalho
    typedef struct ARVORE_B{
        FILE *arq_indices;
        int nNos;
        regCabecalho header;
    }ARVORE_B;

#endif