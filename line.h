/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#ifndef _LINE_H_
#define _LINE_H_

#include "util.h"
#include <stdint.h>
#include <stdio.h>
/**
 * @brief Estrutura responsável pelo header dos arquivos de linha
 *
 */
typedef struct lineFileHeader {
  byte status;
  int64_t byteProxReg;
  int32_t nroRegistros;
  int32_t nroRegRemovidos;
  char *descreveCodigo;
  char *descreveCartao;
  char *descreveNome;
  char *descreveCor;
} lineFileHeader;

/**
 * @brief Estrutura responsável pelos registros de linhas
 *
 */
typedef struct lineRecord {
  byte removido;
  int32_t tamanhoRegistro;
  int32_t codLinha;
  char *aceitaCartao;
  int32_t tamanhoNome;
  char *nomeLinha;
  int32_t tamanhoCor;
  char *corLinha;
} lineRecord;

/**
 * @brief Estrutura responsável por guardar header, registros e infos adicionais
 * sobre o arquivo de linhas
 *
 */
typedef struct lineFile {
  FILE *fp;
  boolean readRecords;
  char *openMode;
  int nRecords;
  lineFileHeader *header;
  lineRecord **records;
} lineFile;

lineFile *createLineFileStruct(char *filename, char *mode);

void destroyLineFile(lineFile *lf);
void destroyLineRecord(lineRecord *l);

void setLineHeader(lineFile *lf, char *descreveCodigo, char *descreveCartao,
                   char *descreveNome, char *descreveCor);

lineRecord *newLineRecord(boolean removido, int32_t codLinha,
                          char *aceitaCartao, int32_t tamanhoNome,
                          char *nomeLinha, int32_t tamanhoCor, char *corLinha);

void writeLineFile(lineFile *lf);

void readLineFile(lineFile *lf, boolean readRecords);

void readLineFileHeader(lineFile *lf);
void readLineReg(FILE *fp, lineRecord *lr);
void writeLineFileHeader(lineFile *lf);
void printLineFile(lineFile *lf);
void printLineRecord(lineFileHeader *lh, lineRecord *lr);
lineRecord *insertOneLine(lineFile *lf);
boolean printMatchingRecordsLine(lineFile *lf, char *field, char *val);

void insertLines(int n, lineFile *lf);

#endif
