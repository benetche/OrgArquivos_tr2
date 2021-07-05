#ifndef ARVOREB_H
#define ARVOREB_H
    #include <stdio.h>
    #include <stdlib.h>

    #define PAG_REGISTRO 77
    #define ORDEM 5
    
    //registro de cabecalho
    typedef struct REG_CABECALHO{
        char status;
        int noRaiz;
        int RRNproxNo;
        char lixo[68] = "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@";

    }regCabecalho;

    //struct auxiliar para o conjunto CHAVE/PONTEIRO
    typedef struct CHAVES_PONTEIROS{
        int chave;
        long long int pontRegistro; //byte offset do registro(char removido)
    };

    //struct de no da arvore b
    typedef struct NO_ARVORE_B{
        char folha;
        int nroChavesIndexadas;
        int RRNdoNo;
        int ponteiros[ORDEM];
        CHAVES_PONTEIROS ponteirosRef[ORDEM-1];   
    };



#endif