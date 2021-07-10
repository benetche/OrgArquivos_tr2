/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/

#include "bTree.h"
#include "util.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

chavePonteiro *criaChavePonteiroPreenchida(int32_t chave, int64_t ponteiroRef) {
  chavePonteiro *cp = (chavePonteiro *)malloc(sizeof(chavePonteiro));

  cp->chave = chave;
  cp->ponteiroRef = ponteiroRef;

  return cp;
}

// Cria estrutura auxiliar chavePonteiro
chavePonteiro *criaChavePonteiro() {
  return criaChavePonteiroPreenchida(-1, -1);
}

cabecalhoArv *criaCabecalhoArv() {
  cabecalhoArv *c = (cabecalhoArv *)malloc(sizeof(cabecalhoArv));
  c->lixo = (byte *)malloc(68 * sizeof(byte));
  memset(c->lixo, '@', 68);
  c->noRaiz = -1;
  c->RRNproxNo = 0;
  c->status = '1';
  return c;
}

void destroiCabecalhoArv(cabecalhoArv *c) {
  free(c->lixo);
  free(c);
}

// Cria um novo no
noArvoreB *criaNo(int32_t rrn) {
  int i;
  noArvoreB *n = (noArvoreB *)malloc(sizeof(noArvoreB));

  // Inicia valores de folha e nroChaves
  n->folha = '0';
  n->nroChavesIndexadas = 0;
  n->RRNdoNo = rrn;
  n->ponteiroSubArv = (int32_t *)malloc(ORDEM * sizeof(int32_t));
  n->ponteirosRef =
      (chavePonteiro **)malloc((ORDEM - 1) * sizeof(chavePonteiro *));

  // Setar ponteiros para subArvores como nulos
  for (i = 0; i < ORDEM; i++) {
    n->ponteiroSubArv[i] = -1;
  }

  // setar as chaves e os ponteiros de registro para -1 (nulo)
  for (i = 0; i < (ORDEM - 1); i++) {
    n->ponteirosRef[i] = criaChavePonteiro();
  }

  return n;
}

void destroiNo(noArvoreB *n) {
  int i;
  for (i = 0; i < (ORDEM - 1); i++) {
    free(n->ponteirosRef[i]);
  }
  free(n->ponteirosRef);
  free(n->ponteiroSubArv);
  free(n);
}

void escreveCabecalhoArvore(arvoreB *arvore) {
  if (!arvore || !arvore->arqIndice ||
      strcmp(arvore->modoDeAbertura, "rb") == 0) {
    return;
  }

  fseek(arvore->arqIndice, 0, SEEK_SET);

  fwrite(&arvore->cabecalho->status, sizeof(byte), 1, arvore->arqIndice);
  fwrite(&arvore->cabecalho->noRaiz, sizeof(int32_t), 1, arvore->arqIndice);
  fwrite(&arvore->cabecalho->RRNproxNo, sizeof(int32_t), 1, arvore->arqIndice);
  fwrite(arvore->cabecalho->lixo, sizeof(byte), 68, arvore->arqIndice);
}

void leCabecalhoArvore(arvoreB *arvore) {
  if (!arvore || !arvore->arqIndice) {
    return;
  }

  fseek(arvore->arqIndice, 0, SEEK_SET);

  fread(&arvore->cabecalho->status, sizeof(byte), 1, arvore->arqIndice);
  fread(&arvore->cabecalho->noRaiz, sizeof(int32_t), 1, arvore->arqIndice);
  fread(&arvore->cabecalho->RRNproxNo, sizeof(int32_t), 1, arvore->arqIndice);
}

arvoreB *criaArvoreB(char *fileName, char *modoDeAbertura) {

  arvoreB *a = (arvoreB *)malloc(sizeof(arvoreB));

  a->arqIndice = fopen(fileName, modoDeAbertura);

  if (!a->arqIndice) {
    free(a);
    return NULL;
  }

  a->cabecalho = criaCabecalhoArv();

  fseek(a->arqIndice, 0, SEEK_END);

  a->modoDeAbertura = modoDeAbertura;

  if (ftell(a->arqIndice) == 0) {
    a->cabecalho->status = '0';

    escreveCabecalhoArvore(a);
  } else {
    leCabecalhoArvore(a);
    if (a->cabecalho->status == '0') {
      destroiCabecalhoArv(a->cabecalho);
      fclose(a->arqIndice);
      free(a);
      // Arquivo inconsistente;
      return NULL;
    }
    a->cabecalho->status = '0';
    escreveCabecalhoArvore(a);
  }

  // a->tamanho = 0;

  return a;
}

void destroiArvoreB(arvoreB *arvore) {
  arvore->cabecalho->status = '1';
  escreveCabecalhoArvore(arvore);
  fclose(arvore->arqIndice);
  destroiCabecalhoArv(arvore->cabecalho);
  free(arvore);
}

void checaSeEhFolha(noArvoreB *no) {
  int i;
  for (i = 0; i < ORDEM; i++) {
    if (no->ponteiroSubArv[i] != -1) {
      no->folha = '0';
      return;
    }
  }
}

noArvoreB *recuperaNoPorRrn(arvoreB *arvore, int32_t rrn) {
  if (!arvore || !arvore->arqIndice) {
    return NULL;
  }

  int i;

  noArvoreB *no = criaNo(rrn);

  fseek(arvore->arqIndice, TAM_PAG * (rrn + 1), SEEK_SET);

  fread(&no->folha, sizeof(byte), 1, arvore->arqIndice);
  fread(&no->nroChavesIndexadas, sizeof(int32_t), 1, arvore->arqIndice);
  fread(&no->RRNdoNo, sizeof(int32_t), 1, arvore->arqIndice);

  for (i = 0; i < (ORDEM - 1); i++) {
    fread(&no->ponteiroSubArv[i], sizeof(int32_t), 1, arvore->arqIndice);
    fread(&no->ponteirosRef[i]->chave, sizeof(int32_t), 1, arvore->arqIndice);
    fread(&no->ponteirosRef[i]->ponteiroRef, sizeof(int64_t), 1,
          arvore->arqIndice);
  }

  fread(&no->ponteiroSubArv[i], sizeof(int32_t), 1, arvore->arqIndice);

  return no;
}

void escreveNoEmArvore(arvoreB *arvore, noArvoreB *no) {
  if (!arvore || !arvore->arqIndice) {
    return;
  }

  int i;

  fseek(arvore->arqIndice, TAM_PAG * (no->RRNdoNo + 1), SEEK_SET);

  fwrite(&no->folha, sizeof(byte), 1, arvore->arqIndice);
  fwrite(&no->nroChavesIndexadas, sizeof(int32_t), 1, arvore->arqIndice);
  fwrite(&no->RRNdoNo, sizeof(int32_t), 1, arvore->arqIndice);

  for (i = 0; i < (ORDEM - 1); i++) {
    fwrite(&no->ponteiroSubArv[i], sizeof(int32_t), 1, arvore->arqIndice);
    fwrite(&no->ponteirosRef[i]->chave, sizeof(int32_t), 1, arvore->arqIndice);
    fwrite(&no->ponteirosRef[i]->ponteiroRef, sizeof(int64_t), 1,
           arvore->arqIndice);
  }

  fwrite(&no->ponteiroSubArv[i], sizeof(int32_t), 1, arvore->arqIndice);
}

int buscaChaveEmNo(chavePonteiro **ponteirosRef, int esquerda, int direita,
                   int32_t chave, int *posicaoDaChave) {
  if (direita >= esquerda) {
    int meio = esquerda + ((direita - esquerda) / 2);

    if (ponteirosRef[meio]->chave == chave) {
      if (posicaoDaChave != NULL) {
        *posicaoDaChave = meio;
      }
      return 0;
    }

    if (ponteirosRef[meio]->chave > chave) {
      return buscaChaveEmNo(ponteirosRef, esquerda, meio - 1, chave,
                            posicaoDaChave);
    }

    return buscaChaveEmNo(ponteirosRef, meio + 1, direita, chave,
                          posicaoDaChave);
  }

  return esquerda + 1;
}

void insereEmOrdemNo(noArvoreB *no, chavePonteiro *chaveAInserir,
                     int32_t rrnSubArvoreDireita) {

  int i, j;

  // Working page
  chavePonteiro **tmpPonteirosRef =
      (chavePonteiro **)malloc(sizeof(chavePonteiro *) * (ORDEM));

  int32_t *tmpPonteiroSubArv = (int32_t *)malloc(sizeof(int32_t) * (ORDEM + 1));

  no->nroChavesIndexadas++;

  // Copia-se dados para working page inserindo a chave nova no local adequado
  // tmpPonteiroSubArv[0] = no->ponteiroSubArv[0];

  for (i = 0, j = 0; i < (ORDEM - 1); i++, j++) {
    if (chaveAInserir->chave < no->ponteirosRef[i]->chave ||
        no->ponteirosRef[i]->chave == -1) {
      tmpPonteirosRef[j] = chaveAInserir;
      tmpPonteiroSubArv[j + 1] = rrnSubArvoreDireita;
      break;
    }

    tmpPonteirosRef[j] = no->ponteirosRef[i];
    tmpPonteiroSubArv[j + 1] = no->ponteiroSubArv[i + 1];
  }

  for (j++; i < (ORDEM - 1); i++, j++) {
    tmpPonteirosRef[j] = no->ponteirosRef[i];
    tmpPonteiroSubArv[j + 1] = no->ponteiroSubArv[i + 1];
  }

  // no->ponteiroSubArv[0] = tmpPonteiroSubArv[0];
  for (i = 0; i < (ORDEM - 1); i++) {
    no->ponteirosRef[i] = tmpPonteirosRef[i];
    no->ponteiroSubArv[i + 1] = tmpPonteiroSubArv[i + 1];
  }

  // checaSeEhFolha(no);

  free(tmpPonteirosRef[ORDEM - 1]);
  free(tmpPonteirosRef);
  free(tmpPonteiroSubArv);
}

void split(chavePonteiro *chaveInsercao, int32_t rrnInsercao,
           noArvoreB *noCorrente, chavePonteiro **chavePromovida,
           int32_t *subArvoreDireita, noArvoreB *novoNo) {
  int i, j;
  // Working page
  chavePonteiro **tmpPonteirosRef =
      (chavePonteiro **)malloc(sizeof(chavePonteiro *) * ORDEM);

  int32_t *tmpPonteiroSubArv = (int32_t *)malloc(sizeof(int32_t) * (ORDEM + 1));

  // Copia-se dados para working page inserindo a chave nova no local adequado
  tmpPonteiroSubArv[0] = noCorrente->ponteiroSubArv[0];

  for (i = 0, j = 0; i < (ORDEM - 1); i++, j++) {
    if (chaveInsercao->chave < noCorrente->ponteirosRef[i]->chave) {
      tmpPonteirosRef[j] = chaveInsercao;
      tmpPonteiroSubArv[j + 1] = rrnInsercao;
      break;
    }

    tmpPonteirosRef[j] = noCorrente->ponteirosRef[i];
    tmpPonteiroSubArv[j + 1] = noCorrente->ponteiroSubArv[i + 1];
  }

  // Caso seja maior que todos
  if (i == (ORDEM - 1)) {
    tmpPonteirosRef[j] = chaveInsercao;
    tmpPonteiroSubArv[j + 1] = rrnInsercao;
  }

  for (j++; i < (ORDEM - 1); i++, j++) {
    tmpPonteirosRef[j] = noCorrente->ponteirosRef[i];
    tmpPonteiroSubArv[j + 1] = noCorrente->ponteiroSubArv[i + 1];
  }

  // Define-se a chave promovida e sua subArvore à direita
  *chavePromovida = tmpPonteirosRef[ORDEM / 2];
  *subArvoreDireita = novoNo->RRNdoNo;

  // Anterior à promoção vai para o nó corrente
  noCorrente->nroChavesIndexadas = 0;
  noCorrente->ponteiroSubArv[0] = tmpPonteiroSubArv[0];
  for (i = 0; i < (ORDEM / 2); i++) {
    noCorrente->nroChavesIndexadas++;
    noCorrente->ponteirosRef[i] = tmpPonteirosRef[i];
    noCorrente->ponteiroSubArv[i + 1] = tmpPonteiroSubArv[i + 1];
  }

  // O restante torna-se nulo
  for (i = (ORDEM / 2); i < (ORDEM - 1); i++) {
    noCorrente->ponteirosRef[i] = criaChavePonteiro();
    noCorrente->ponteiroSubArv[i + 1] = -1;
  }

  // Posterior à promoção vai para o novo nó
  novoNo->nroChavesIndexadas = 0;
  novoNo->ponteiroSubArv[0] = tmpPonteiroSubArv[(ORDEM / 2) + 1];
  for (i = ((ORDEM / 2) + 1); i < ORDEM; i++) {
    free(novoNo->ponteirosRef[i - ((ORDEM / 2) + 1)]);
    novoNo->nroChavesIndexadas++;
    novoNo->ponteirosRef[i - ((ORDEM / 2) + 1)] = tmpPonteirosRef[i];
    novoNo->ponteiroSubArv[i - ((ORDEM / 2))] = tmpPonteiroSubArv[i + 1];
  }

  // checaSeEhFolha(noCorrente);
  // checaSeEhFolha(novoNo);
  novoNo->folha = noCorrente->folha;

  free(tmpPonteiroSubArv);
  free(tmpPonteirosRef);
}

int inserir(arvoreB *arvore, int32_t rrnCorrente, chavePonteiro *novaChave,
            chavePonteiro **chavePromovida, int32_t *subArvoreDireita) {
  if (rrnCorrente == -1) {
    *chavePromovida = novaChave;
    *subArvoreDireita = -1;
    return PROMOCAO;
  }

  noArvoreB *noCorrente = recuperaNoPorRrn(arvore, rrnCorrente);
  int resultadoBuscaEmNo = buscaChaveEmNo(noCorrente->ponteirosRef, 0,
                                          noCorrente->nroChavesIndexadas - 1,
                                          novaChave->chave, NULL);

  // P_B_RRN e P_B_KEY
  int32_t rrnInferiorParaInserir;
  chavePonteiro *chaveInferiorParaInserir;

  if (resultadoBuscaEmNo == 0) {
    destroiNo(noCorrente);
    return ERRO_CHAVE_EXISTENTE;
  }

  int returnValue =
      inserir(arvore, noCorrente->ponteiroSubArv[resultadoBuscaEmNo - 1],
              novaChave, &chaveInferiorParaInserir, &rrnInferiorParaInserir);

  if (returnValue != PROMOCAO) {
    destroiNo(noCorrente);
    return returnValue;
  }

  if (noCorrente->nroChavesIndexadas < (ORDEM - 1)) {
    insereEmOrdemNo(noCorrente, chaveInferiorParaInserir,
                    rrnInferiorParaInserir);
    escreveNoEmArvore(arvore, noCorrente);
    destroiNo(noCorrente);
    return SEM_PROMOCAO;
  }

  noArvoreB *novoNo = criaNo(arvore->cabecalho->RRNproxNo++);

  split(chaveInferiorParaInserir, rrnInferiorParaInserir, noCorrente,
        chavePromovida, subArvoreDireita, novoNo);

  escreveNoEmArvore(arvore, noCorrente);
  escreveNoEmArvore(arvore, novoNo);

  destroiNo(novoNo);
  destroiNo(noCorrente);

  return PROMOCAO;
}

void inserirNaArvoreB(arvoreB *arvore, chavePonteiro *chaveAInserir) {
  if (arvore->cabecalho->noRaiz == -1) {
    noArvoreB *novoNo = criaNo(arvore->cabecalho->RRNproxNo++);
    novoNo->folha = '1';
    free(novoNo->ponteirosRef[0]);
    novoNo->ponteirosRef[0] = chaveAInserir;
    novoNo->nroChavesIndexadas++;
    escreveNoEmArvore(arvore, novoNo);
    arvore->cabecalho->noRaiz = novoNo->RRNdoNo;
    destroiNo(novoNo);
    return;
  }

  chavePonteiro *chavePromovida;
  int32_t subArvoreDireita;

  int returnValue = inserir(arvore, arvore->cabecalho->noRaiz, chaveAInserir,
                            &chavePromovida, &subArvoreDireita);

  if (returnValue == PROMOCAO) {
    noArvoreB *novoNo = criaNo(arvore->cabecalho->RRNproxNo++);
    free(novoNo->ponteirosRef[0]);
    novoNo->ponteirosRef[0] = chavePromovida;
    novoNo->nroChavesIndexadas++;
    novoNo->ponteiroSubArv[0] = arvore->cabecalho->noRaiz;
    novoNo->ponteiroSubArv[1] = subArvoreDireita;
    escreveNoEmArvore(arvore, novoNo);
    arvore->cabecalho->noRaiz = novoNo->RRNdoNo;
    destroiNo(novoNo);
  }
}

int64_t busca(arvoreB *arvore, int32_t rrnCorrente, int32_t chaveDeBusca) {
  if (rrnCorrente == -1) {
    return -1;
  }

  noArvoreB *noCorrente = recuperaNoPorRrn(arvore, rrnCorrente);

  int posicaoDaChave;

  int resultadoBuscaEmNo = buscaChaveEmNo(noCorrente->ponteirosRef, 0,
                                          noCorrente->nroChavesIndexadas - 1,
                                          chaveDeBusca, &posicaoDaChave);

  if (resultadoBuscaEmNo == 0) {
    int64_t offsetDaChave =
        noCorrente->ponteirosRef[posicaoDaChave]->ponteiroRef;
    destroiNo(noCorrente);
    return offsetDaChave;
  }

  int32_t rrnProvavel = noCorrente->ponteiroSubArv[resultadoBuscaEmNo - 1];

  destroiNo(noCorrente);

  return busca(arvore, rrnProvavel, chaveDeBusca);
}

int64_t buscaNaArvoreB(arvoreB *arvore, int32_t chaveDeBusca) {
  return busca(arvore, arvore->cabecalho->noRaiz, chaveDeBusca);
}
