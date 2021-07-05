#include "arvoreb.h"


//inicializa um novo no
NO_ARVORE_B *criarNo(){
    NO_ARVORE_B *novo;

    novo = (NO_ARVORE_B *) malloc(sizeof(NO_ARVORE_B));
    novo->folha = -1;
    novo->nroChavesIndexadas = 0;

    //setar os ponteiros para -1 (nulo)
    for(int i = 0; i < ORDEM; i++) 
        novo->ponteiros[i] = -1;

    //setar as chaves e os ponteiros de registro para -1 (nulo)
    for(int i = 0; i < ORDEM-1; i++){
        novo->ponteirosRef[i].chave = -1;
        novo->ponteirosRef[i].pontRegistro = -1;
    }

    return novo;
}

ARVORE_B *criarArvoreB(char *fileName, char *mode){
    
    ARVORE_B *arvore = NULL;
    arvore = (ARVORE_B *) malloc(sizeof(ARVORE_B));
    NO_ARVORE_B *aux = NULL;

    arvore->arq_indices = fopen(fileName, mode);
    if(!arvore->arq_indices){
        free(arvore);
        return NULL;
    }

    arvore->header.noRaiz = 0;
    arvore->nNos = 1;
    aux = criarNo();
    aux->folha = 1;

    //falta inserir o no na arvore


    return arvore;

}