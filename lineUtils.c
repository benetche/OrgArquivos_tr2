/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "line.h"
#include "lineUtils.h"
#include "util.h"

/**
 * @brief Imprime na tela campo aceita cartão
 * S - PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR
 * N - PAGAMENTO EM CARTAO E DINHEIRO
 * F - PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA
 * @param aceitaCartao
 */
void printCard(char *aceitaCartao) {
  switch (*aceitaCartao) {
  case 'S':
    printf("PAGAMENTO SOMENTE COM CARTAO SEM PRESENCA DE COBRADOR");
    break;
  case 'N':
    printf("PAGAMENTO EM CARTAO E DINHEIRO");
    break;
  case 'F':
    printf("PAGAMENTO EM CARTAO SOMENTE NO FINAL DE SEMANA");
    break;
  default:
    break;
  }
}

/**
 * @brief Transforma linha de cabeçalho do arquivo CSV em header de lf
 *
 * @param lf
 * @param line Linha de cabeçalho
 */
void parseDescLine(lineFile *lf, char *line) {
  char *rest = line;

  char *descreveCodigo = copyStr(strtok_single(rest, ",", &rest));
  char *descreveCartao = copyStr(strtok_single(rest, ",", &rest));
  char *descreveNome = copyStr(strtok_single(rest, ",", &rest));
  char *descreveCor = copyStr(strtok_single(rest, ",", &rest));

  setLineHeader(lf, descreveCodigo, descreveCartao, descreveNome, descreveCor);
}

/**
 * @brief Transforma linha de CSV em regsitro de linhas
 *
 * @param line Linha de CSV
 * @return lineRecord*
 */
lineRecord *parseLineFromStrLine(char *line) {
  char *rest = line;
  char *tmpCod;

  boolean removido;
  int32_t tamanhoNome;
  int32_t tamanhoCor;

  int32_t codLinha =
      atoi(tmpCod = safeToId(strtok_single(rest, ",", &rest), 4, &removido));
  // Desaloca string de codLinha temporária após uso
  free(tmpCod);
  char *aceitaCartao = safeToFixed(strtok_single(rest, ",", &rest), 1);
  char *nomeLinha =
      safeToDynamic(strtok_single(rest, ",", &rest), &tamanhoNome);
  char *corLinha = safeToDynamic(strtok_single(rest, ",", &rest), &tamanhoCor);

  return newLineRecord(removido, codLinha, aceitaCartao, tamanhoNome, nomeLinha,
                       tamanhoCor, corLinha);
}

/**
 * @brief Lê header e registros de um arquivo CSV presente em fp para lf
 *
 * @param lf
 * @param filename
 */
void parseCSVToLines(lineFile *lf, char *filename) {
  FILE *fp = fopen(filename, "r");

  if (!fp) {
    return;
  }

  char *tmp;
  lineRecord *tmpRec;

  tmp = readLine(fp); // Lê cabeçalho
  parseDescLine(lf, tmp);
  free(tmp);

  while (!feof(fp)) {
    tmp = readLine(fp);

    if ((*tmp) == 0) {
      free(tmp);
      continue;
    }

    tmpRec = parseLineFromStrLine(tmp);

    if (tmpRec->removido == '1') {
      lf->header->nroRegistros++;
    } else {
      lf->header->nroRegRemovidos++;
    }

    if ((lf->nRecords) % LINE_BUFFER == 0) {
      lf->records = (lineRecord **)realloc(
          lf->records, ((lf->nRecords) / LINE_BUFFER + 1) * LINE_BUFFER *
                           sizeof(lineRecord *));
    }
    lf->records[(lf->nRecords)++] = tmpRec;
    lf->header->byteProxReg += tmpRec->tamanhoRegistro + 5;

    free(tmp);
  }

  lf->records = (lineRecord **)realloc(lf->records,
                                       ((lf->nRecords)) * sizeof(lineRecord *));

  lf->readRecords = true;

  fclose(fp);
}
/**
 * @brief Recupera codLinha da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return int32_t
 */
int32_t getCode(FILE *fp, int *read) {
  int32_t codLinha;
  fread(&codLinha, sizeof(int32_t), 1, fp);

  *read = 4;

  return codLinha;
}

/**
 * @brief Recupera aceitaCartao da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return char*
 */
char *getCard(FILE *fp, int *read) {
  char *aceitaCartao = prepStr(1);
  fseek(fp, 4, SEEK_CUR);
  fread(aceitaCartao, sizeof(char), 1, fp);

  *read = 5;

  if (*aceitaCartao == 0) {
    free(aceitaCartao);
    return NULL;
  }
  return aceitaCartao;
}

/**
 * @brief Recupera nomeLinha da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return char*
 */
char *getName(FILE *fp, int *read) {
  int32_t len;

  fseek(fp, 5, SEEK_CUR);
  fread(&len, sizeof(int32_t), 1, fp);

  if (len == 0) {
    *read = 9;
    return NULL;
  }

  char *nomeLinha = prepStr(len);

  fread(nomeLinha, sizeof(char), len, fp);

  *read = 9 + len;

  return nomeLinha;
}
/**
 * @brief Recupera corLinha da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return char*
 */
char *getColor(FILE *fp, int *read) {
  int32_t lenN;
  int32_t lenC;

  fseek(fp, 5, SEEK_CUR);
  fread(&lenN, sizeof(int32_t), 1, fp);
  fseek(fp, lenN, SEEK_CUR);
  fread(&lenC, sizeof(int32_t), 1, fp);

  if (lenC == 0) {
    *read = 13 + lenN;
    return NULL;
  }

  char *corLinha = prepStr(lenC);

  fread(corLinha, sizeof(char), lenC, fp);

  *read = 13 + lenN + lenC;

  return corLinha;
}

/**
 * @brief Verifica se codLinha da posição corrente de fp é igual a src. Guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkCode(int32_t src, FILE *fp, int *read) {
  return getCode(fp, read) == src;
}

/**
 * @brief Verifica se aceitaCartao da posição corrente de fp é igual a src.
 * Guarda quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkCard(char *src, FILE *fp, int *read) {
  return checkMatch(src, getCard(fp, read));
}
/**
 * @brief Verifica se nomeLinha da posição corrente de fp é igual a src. Guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkName(char *src, FILE *fp, int *read) {
  return checkMatch(src, getName(fp, read));
}
/**
 * @brief Verifica se corLinha da posição corrente de fp é igual a src. Guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkColor(char *src, FILE *fp, int *read) {
  return checkMatch(src, getColor(fp, read));
}
