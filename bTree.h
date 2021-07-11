/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/

#ifndef B_TREE_H
#define B_TREE_H

#include "util.h"

#include <stdint.h>
#include <stdio.h>

#define TAM_PAG 77
#define ORDEM 5
#define PROMOCAO 1
#define SEM_PROMOCAO 0
#define ERRO_CHAVE_EXISTENTE -127

// Registro de cabecalho
typedef struct cabecalhoArv {
  byte status;
  int32_t noRaiz; // RRN
  int32_t RRNproxNo;
  byte *lixo;
} cabecalhoArv;

// Struct auxiliar para o conjunto CHAVE/PONTEIRO
typedef struct chavePonteiro {
  int32_t chave;
  int64_t ponteiroRef; // Byte offset do registro(char removido)
} chavePonteiro;

// Struct de no da arvore b
typedef struct noArvoreB {
  byte folha;
  int32_t nroChavesIndexadas;
  int32_t RRNdoNo;
  int32_t *ponteiroSubArv; // RRN's
  chavePonteiro **ponteirosRef;
} noArvoreB;

// Struct da arvore b
// Possui o arquivo da arvore b, numero de nos e registro de cabecalho
typedef struct arvoreB {
  FILE *arqIndice;
  char *modoDeAbertura;
  // int32_t tamanho;
  cabecalhoArv *cabecalho;
} arvoreB;

chavePonteiro *criaChavePonteiroPreenchida(int32_t chave, int64_t ponteiroRef);

arvoreB *criaArvoreB(char *fileName, char *modoDeAbertura);

void destroiArvoreB(arvoreB *arvore);

void inserirNaArvoreB(arvoreB *arvore, chavePonteiro *chaveAInserir);

int64_t buscaNaArvoreB(arvoreB *arvore, int32_t chaveDeBusca);

#endif
