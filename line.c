/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/

#include "line.h"
#include "lineUtils.h"
#include "util.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Cria Struct que guarda dados de header, registros e informações
 * adicionais sobre o arquivo de linhas.
 *
 * @param filename Nome do arquivo à ser utilizado
 * @param mode Modo de abertura do arquivo (wb, rb, rb+)
 * @return lineFile* Retorna estrutura alocada na heap.
 */
lineFile *createLineFileStruct(char *filename, char *mode) {
  lineFile *lf = (lineFile *)malloc(sizeof(lineFile));
  lf->nRecords = 0;
  lf->openMode = mode;
  lf->readRecords = false;

  lf->fp = fopen(filename, mode);

  if (!lf->fp) {
    free(lf);
    return NULL;
  }

  // Verifica se arquivo já existe e se não está corrompido
  if (strcmp(mode, "wb") != 0) {
    byte status;
    if (fread(&status, sizeof(byte), 1, lf->fp) != 1 || status != '1') {
      fclose(lf->fp);
      free(lf);
      return NULL;
    }
  }

  lf->header = (lineFileHeader *)malloc(sizeof(lineFileHeader));
  if(strcmp(mode, "rb") == 0){
    lf->header->byteProxReg = 0;
    lf->header->nroRegRemovidos = lf->header->nroRegistros = 0;
  }

  if (strcmp(mode, "rb") != 0) {
    setFileStatus(lf->fp, '0');
    lf->header->status = '0';
  }

  lf->records = NULL;

  return lf;
}

/**
 * @brief Cria novo registro de linha na memória principal.
 *
 * @param removido
 * @param codLinha
 * @param aceitaCartao
 * @param tamanhoNome
 * @param nomeLinha
 * @param tamanhoCor
 * @param corLinha
 * @return lineRecord* Retorna registro alocado na heap.
 */
lineRecord *newLineRecord(boolean removido, int32_t codLinha,
                          char *aceitaCartao, int32_t tamanhoNome,
                          char *nomeLinha, int32_t tamanhoCor, char *corLinha) {

  lineRecord *l = (lineRecord *)malloc(sizeof(lineRecord));

  l->removido = removido ? '1' : '0';
  l->codLinha = codLinha;
  l->aceitaCartao = aceitaCartao;
  l->tamanhoNome = tamanhoNome;
  l->nomeLinha = nomeLinha;
  l->tamanhoCor = tamanhoCor;
  l->corLinha = corLinha;

  l->tamanhoRegistro = 4 + 1 + 4 + tamanhoNome + 4 + tamanhoCor;

  return l;
}

/**
 * @brief Configura o header da estrutura lineFile
 *
 * @param lf Struct a ser configurada
 * @param descreveCodigo
 * @param descreveCartao
 * @param descreveNome
 * @param descreveCor
 */

void setLineHeader(lineFile *lf, char *descreveCodigo, char *descreveCartao,
                   char *descreveNome, char *descreveCor) {
  lf->header->descreveCodigo = descreveCodigo;
  lf->header->descreveCartao = descreveCartao;
  lf->header->descreveNome = descreveNome;
  lf->header->descreveCor = descreveCor;
  lf->header->byteProxReg += 1 + 8 + 4 + 4 + 15 + 13 + 13 + 24;
}
/**
 * @brief Desaloca por completo estrutura de registro de linha
 *
 * @param l Registro a ser deasalocado
 */
void destroyLineRecord(lineRecord *l) {
  free(l->aceitaCartao);
  free(l->nomeLinha);
  free(l->corLinha);
  free(l);
}

/**
 * @brief Desaloca por completo estrutura de header de linha
 *
 * @param lh Header a ser desalocado
 */
void destroyLineHeader(lineFileHeader *lh) {
  free(lh->descreveCodigo);
  free(lh->descreveCartao);
  free(lh->descreveNome);
  free(lh->descreveCor);
  free(lh);
}

/**
 * @brief Desaloca por completo estrutura de arquivo de linhas
 *
 * @param lf Struct de Arquivo de Linhas a ser desalocado
 */
void destroyLineFile(lineFile *lf) {

  if (lf->readRecords) {
    for (int i = 0; i < lf->nRecords; i++) {
      destroyLineRecord(lf->records[i]);
    }

    free(lf->records);
  }

  if (strcmp(lf->openMode, "rb") != 0) {
    setFileStatus(lf->fp, '1');
    lf->header->status = '1';
  }

  destroyLineHeader(lf->header);

  fclose(lf->fp);
  free(lf);
}
/**
 * @brief Escreve o header da memória principal no arquivo descrito por lf
 *
 * @param lf
 */
void writeLineFileHeader(lineFile *lf) {
  fseek(lf->fp, 1, SEEK_SET);
  fwrite(&lf->header->byteProxReg, sizeof(int64_t), 1, lf->fp);
  fwrite(&lf->header->nroRegistros, sizeof(int32_t), 1, lf->fp);
  fwrite(&lf->header->nroRegRemovidos, sizeof(int32_t), 1, lf->fp);
  fwrite(lf->header->descreveCodigo, sizeof(char), 15, lf->fp);
  fwrite(lf->header->descreveCartao, sizeof(char), 13, lf->fp);
  fwrite(lf->header->descreveNome, sizeof(char), 13, lf->fp);
  fwrite(lf->header->descreveCor, sizeof(char), 24, lf->fp);
}
/**
 * @brief Escreve o registro de linha lr no arquivo fp
 *
 * @param fp
 * @param lr
 */
void writeLineReg(FILE *fp, lineRecord *lr) {
  fwrite(&lr->removido, sizeof(byte), 1, fp);
  fwrite(&lr->tamanhoRegistro, sizeof(int32_t), 1, fp);
  fwrite(&lr->codLinha, sizeof(int32_t), 1, fp);
  fwrite(lr->aceitaCartao, sizeof(char), 1, fp);
  fwrite(&lr->tamanhoNome, sizeof(int32_t), 1, fp);
  fwrite(lr->nomeLinha, sizeof(char), lr->tamanhoNome, fp);
  fwrite(&lr->tamanhoCor, sizeof(int32_t), 1, fp);
  fwrite(lr->corLinha, sizeof(char), lr->tamanhoCor, fp);
}

/**
 * @brief Escreve o header e os registros de linhas no arquivo descrito por lf
 *
 * @param lf
 */
void writeLineFile(lineFile *lf) {
  writeLineFileHeader(lf);

  for (int i = 0; i < lf->nRecords; i++) {
    writeLineReg(lf->fp, lf->records[i]);
  }
}

/**
 * @brief Recupera header do arquivo descrito por lf para a memória principal
 *
 * @param lf
 */
void readLineFileHeader(lineFile *lf) {
  fseek(lf->fp, 0, SEEK_SET);
  fread(&lf->header->status, sizeof(byte), 1, lf->fp);
  fread(&lf->header->byteProxReg, sizeof(int64_t), 1, lf->fp);
  fread(&lf->header->nroRegistros, sizeof(int32_t), 1, lf->fp);
  fread(&lf->header->nroRegRemovidos, sizeof(int32_t), 1, lf->fp);

  lf->header->descreveCodigo = prepStr(15);
  lf->header->descreveCartao = prepStr(13);
  lf->header->descreveNome = prepStr(13);
  lf->header->descreveCor = prepStr(24);

  fread(lf->header->descreveCodigo, sizeof(char), 15, lf->fp);
  fread(lf->header->descreveCartao, sizeof(char), 13, lf->fp);
  fread(lf->header->descreveNome, sizeof(char), 13, lf->fp);
  fread(lf->header->descreveCor, sizeof(char), 24, lf->fp);
}

/**
 * @brief Recupera registro presente na posição corrente de fp e o salva em lr
 *
 * @param fp
 * @param lr
 */
void readLineReg(FILE *fp, lineRecord *lr) {
  fread(&lr->removido, sizeof(byte), 1, fp);
  fread(&lr->tamanhoRegistro, sizeof(int32_t), 1, fp);
  fread(&lr->codLinha, sizeof(int32_t), 1, fp);
  lr->aceitaCartao = prepStr(1);
  fread(lr->aceitaCartao, sizeof(char), 1, fp);
  fread(&lr->tamanhoNome, sizeof(int32_t), 1, fp);
  lr->nomeLinha = prepStr(lr->tamanhoNome);
  fread(lr->nomeLinha, sizeof(char), lr->tamanhoNome, fp);
  fread(&lr->tamanhoCor, sizeof(int32_t), 1, fp);
  lr->corLinha = prepStr(lr->tamanhoCor);
  fread(lr->corLinha, sizeof(char), lr->tamanhoCor, fp);
}

/**
 * @brief Lê arquivo binário de linhas para a estrutura lf
 *
 * @param lf
 * @param readRecords Define se os registros do arquivo devem ser lidos para a
 * memória principal
 */
void readLineFile(lineFile *lf, boolean readRecords) {
  readLineFileHeader(lf);

  lf->nRecords = lf->header->nroRegRemovidos + lf->header->nroRegistros;

  if (readRecords) {
    lf->readRecords = true;
    lf->records = (lineRecord **)malloc(sizeof(lineRecord *) * lf->nRecords);

    for (int i = 0; i < lf->nRecords; i++) {
      lf->records[i] = (lineRecord *)malloc(sizeof(lineRecord));
      readLineReg(lf->fp, lf->records[i]);
    }
  }
}

/**
 * @brief Imprime registro de linha na tela caso este não tenha sido removido
 * logicamente
 *
 * @param lh Header contendo metadados de registros de linha
 * @param lr Registro à ser impresso
 */
void printLineRecord(lineFileHeader *lh, lineRecord *lr) {
  if (lr->removido == '1') {

    printf("%s: ", lh->descreveCodigo);
    printf("%d", lr->codLinha);
    printf("\n");

    printf("%s: ", lh->descreveNome);
    if (isStrNull(lr->nomeLinha)) {
      printf(NULO);
    } else {
      printf("%s", lr->nomeLinha);
    }
    printf("\n");

    printf("%s: ", lh->descreveCor);
    if (isStrNull(lr->corLinha)) {
      printf(NULO);
    } else {
      printf("%s", lr->corLinha);
    }
    printf("\n");

    printf("%s: ", lh->descreveCartao);
    if (isStrNull(lr->aceitaCartao)) {
      printf(NULO);
    } else {
      printCard(lr->aceitaCartao);
    }
    printf("\n");
    printf("\n");
  }
}

/**
 * @brief Imprime todos os registros não removidos presentes em lf
 *
 * @param lf
 */
void printLineFile(lineFile *lf) {
  for (int i = 0; i < lf->nRecords; i++) {
    printLineRecord(lf->header, lf->records[i]);
  }
}

/**
 * @brief Imprime registro presente posição corrente de lf->fp caso este tenha
 * valor do campo field igual a val
 *
 * @param lf
 * @param field Campo a ser comparado
 * @param val Valor de comparação
 * @return boolean Retorna true caso o registro atendeu as condições de busca
 */
boolean printIfMatchesLine(lineFile *lf, char *field, char *val) {
  byte removido;
  int32_t regSize;

  fread(&removido, sizeof(byte), 1, lf->fp);
  fread(&regSize, sizeof(int32_t), 1, lf->fp);

  if (removido == '1') {

    if (*val == 0) {
      val = NULL;
    }

    boolean matches = false;
    int read = 0;

    switch (field[2]) {
      // codLinha
    case 'd':
      matches = checkCode(atoi(val), lf->fp, &read);
      break;
      // aceitaCartao
    case 'e':
      matches = checkCard(val, lf->fp, &read);
      break;
      // nomeLinha
    case 'm':
      matches = checkName(val, lf->fp, &read);
      break;
      // corLinha
    case 'r':
      matches = checkColor(val, lf->fp, &read);
      break;
    default:
      break;
    }

    if (matches) {
      // Retorna à posição inicial do registro
      fseek(lf->fp, -(read + 5), SEEK_CUR);

      lineRecord *tmp = (lineRecord *)malloc(sizeof(lineRecord));

      readLineReg(lf->fp, tmp);

      printLineRecord(lf->header, tmp);

      destroyLineRecord(tmp);

      return true;
    }
    // Pula restante do registro
    fseek(lf->fp, regSize - read, SEEK_CUR);

    return false;
  }

  // Pula registro por completo
  fseek(lf->fp, regSize, SEEK_CUR);

  return false;
}
/**
 * @brief Imprime todos regitros de lf que possuem valor do campo field igual a
 * val
 *
 * @param lf
 * @param field Campo a ser comparado
 * @param val  Valor de comparação
 * @return boolean Retorna true caso algum registro de lf satisfaça as condições
 * de busca
 */
boolean printMatchingRecordsLine(lineFile *lf, char *field, char *val) {
  boolean matched = false;

  for (int i = 0; i < lf->nRecords; i++) {
    matched |= printIfMatchesLine(lf, field, val);
  }

  return matched;
}

/**
 * @brief Insere n novos registros de linhas lidos de stdin em lf
 *
 * @param n Número de linhas a serem inseridas
 * @param lf
 */
void insertLines(int n, lineFile *lf) {
  fseek(lf->fp, lf->header->byteProxReg, SEEK_SET);

  lineRecord *lr = (lineRecord *)malloc(sizeof(lineRecord));

  char codLinha[5];
  char aceitaCartao[2];
  char nomeLinha[128];
  char corLinha[128];

  for (int i = 0; i < n; i++) {
    lf->nRecords++;

    scan_quote_string(codLinha);

    if (codLinha[0] == '*') {
      lr->removido = '0';
      lf->header->nroRegRemovidos++;
      lr->codLinha = atoi(codLinha + 1);
    } else {
      lr->removido = '1';
      lf->header->nroRegistros++;
      lr->codLinha = atoi(codLinha);
    }

    scan_quote_string(aceitaCartao);
    lr->aceitaCartao = aceitaCartao;

    scan_quote_string(nomeLinha);
    if (*nomeLinha == 0) {
      lr->tamanhoNome = 0;
      lr->nomeLinha = NULL;
    } else {
      lr->tamanhoNome = strlen(nomeLinha);
      lr->nomeLinha = nomeLinha;
    }

    scan_quote_string(corLinha);
    if (*corLinha == 0) {
      lr->tamanhoCor = 0;
      lr->corLinha = NULL;
    } else {
      lr->tamanhoCor = strlen(corLinha);
      lr->corLinha = corLinha;
    }

    lr->tamanhoRegistro = 4 + 1 + 4 + lr->tamanhoNome + 4 + lr->tamanhoCor;

    lf->header->byteProxReg += lr->tamanhoRegistro + 5;

    writeLineReg(lf->fp, lr);
  }

  writeLineFileHeader(lf);

  free(lr);
}

/**
 * @brief Le um registro a ser inserido e o insere no arquivo de registro
 * de linha
 * @param lf
 * @return retorna o registro lido e inserido para poder ser inserido na arvore B
 */


lineRecord *insertOneLine(lineFile *lf){
  fseek(lf->fp, lf->header->byteProxReg, SEEK_SET);

  lineRecord *lr = (lineRecord *)malloc(sizeof(lineRecord));

  char codLinha[5];
  char aceitaCartao[2];
  char nomeLinha[128];
  char corLinha[128];
  lf->nRecords++;

  scan_quote_string(codLinha);

  if (codLinha[0] == '*') {
    lr->removido = '0';
    lf->header->nroRegRemovidos++;
    lr->codLinha = atoi(codLinha + 1);
  } else {
    lr->removido = '1';
    lf->header->nroRegistros++;
    lr->codLinha = atoi(codLinha);
  }

  scan_quote_string(aceitaCartao);
  lr->aceitaCartao = aceitaCartao;

  scan_quote_string(nomeLinha);
  if (*nomeLinha == 0) {
    lr->tamanhoNome = 0;
    lr->nomeLinha = NULL;
  } else {
    lr->tamanhoNome = strlen(nomeLinha);
    lr->nomeLinha = nomeLinha;
  }

  scan_quote_string(corLinha);
  if (*corLinha == 0) {
    lr->tamanhoCor = 0;
    lr->corLinha = NULL;
  } else {
    lr->tamanhoCor = strlen(corLinha);
    lr->corLinha = corLinha;
  }

  lr->tamanhoRegistro = 4 + 1 + 4 + lr->tamanhoNome + 4 + lr->tamanhoCor;

  lf->header->byteProxReg += lr->tamanhoRegistro + 5;

  writeLineReg(lf->fp, lr);
  return lr;

}