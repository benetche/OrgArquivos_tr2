#ifndef B_TREE_H
#define B_TREE_H

#include "util.h"

#include <stdint.h>
#include <stdio.h>

#define PAG_REGISTRO 77
#define ORDEM 5

// Registro de cabecalho
typedef struct cabecalhoArv {
  boolean status;
  int32_t noRaiz;
  int32_t RRNproxNo;
  byte *lixo;
} cabecalhoArv;

// Struct auxiliar para o conjunto CHAVE/PONTEIRO
typedef struct chavePonteiro {
  int32_t chave;
  int64_t ponteiroReg; // Byte offset do registro(char removido)
} chavePonteiro;

// Struct de no da arvore b
typedef struct noArvoreB {
  boolean folha;
  int32_t nroChavesIndexadas;
  int32_t RRNdoNo;
  int32_t *ponteiroSubArv;
  chavePonteiro **ponteirosReg;
} noArvoreB;

// Struct da arvore b
// Possui o arquivo da arvore b, numero de nos e registro de cabecalho
typedef struct arvoreB {
  FILE *indexFile;
  int32_t size;
  cabecalhoArv *header;
} arvoreB;

#endif