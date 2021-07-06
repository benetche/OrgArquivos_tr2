#include "bTree.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Cria estrutura auxiliar chavePonteiro
chavePonteiro *criaChavePonteiro() {
  chavePonteiro *cp = (chavePonteiro *)malloc(sizeof(chavePonteiro));

  cp->chave = -1;
  cp->ponteiroReg = -1;

  return cp;
}

cabecalhoArv *criaCabecalhoArv() {
  cabecalhoArv *c = (cabecalhoArv *)malloc(sizeof(cabecalhoArv));
  c->lixo = (byte *)malloc(68 * sizeof(byte));
  memset(c->lixo, '@', 68);
  c->noRaiz = -1;
  c->RRNproxNo = 0;
  c->status = 1;
  return c;
}

void destroiCabecalhoArv(cabecalhoArv *c) {
  free(c->lixo);
  free(c);
}

// Cria um novo no
noArvoreB *criaNo() {
  int i;
  noArvoreB *n = (noArvoreB *)malloc(sizeof(noArvoreB));

  // Inicia valores de folha e nroChaves
  n->folha = 1;
  n->nroChavesIndexadas = 0;
  n->ponteiroSubArv = (int32_t *)malloc(ORDEM * sizeof(int32_t));
  n->ponteirosReg =
      (chavePonteiro **)malloc((ORDEM - 1) * sizeof(chavePonteiro *));

  // Setar ponteiros para subArvores como nulos
  for (i = 0; i < ORDEM; i++) {
    n->ponteiroSubArv[i] = -1;
  }

  // setar as chaves e os ponteiros de registro para -1 (nulo)
  for (i = 0; i < (ORDEM - 1); i++) {
    n->ponteirosReg[i] = criaChavePonteiro();
  }

  return n;
}

void destroiNo(noArvoreB *n) {
  int i;
  for (i = 0; i < (ORDEM - 1); i++) {
    free(n->ponteirosReg[i]);
  }
  free(n->ponteirosReg);
  free(n->ponteiroSubArv);
  free(n);
}

arvoreB *criaArvoreB(char *fileName, char *mode) {

  arvoreB *a = (arvoreB *)malloc(sizeof(arvoreB));

  a->indexFile = fopen(fileName, mode);

  if (!a->indexFile) {
    free(a);
    return NULL;
  }

  a->header = criaCabecalhoArv();
  a->size = 0;

  return a;
}