/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "vehicle.h"
#include "vehicleUtils.h"

/**
 * @brief Imprime mês presente numericamente em month de forma extensa
 *
 * @param month
 */
void printMonth(int month) {
  printf(" de ");
  switch (month) {
  case 1:
    printf("janeiro");
    break;
  case 2:
    printf("fevereiro");
    break;
  case 3:
    printf("março");
    break;
  case 4:
    printf("abril");
    break;
  case 5:
    printf("maio");
    break;
  case 6:
    printf("junho");
    break;
  case 7:
    printf("julho");
    break;
  case 8:
    printf("agosto");
    break;
  case 9:
    printf("setembro");
    break;
  case 10:
    printf("outubro");
    break;
  case 11:
    printf("novembro");
    break;
  case 12:
    printf("dezembro");
    break;
  default:
    break;
  }
  printf(" de ");
}

/**
 * @brief Imprime data no formato YYYY/MM/DD de forma extensa
 * Ex: 2004/02/01 = 01 de fevereiro de 2004
 *
 * @param data
 */
void printDate(char *data) {
  char month[3];

  month[0] = data[5];
  month[1] = data[6];
  month[2] = 0;

  printf("%.2s", data + 8);
  printMonth(atoi(month));
  printf("%.4s", data);
}

/**
 * @brief Transforma linha de cabeçalho do arquivo CSV em header de vf
 *
 * @param vf
 * @param line Linha de cabeçalho
 */
void parseDescVehicle(vehicleFile *vf, char *line) {
  char *rest = line;

  char *descrevePrefixo = copyStr(strtok_single(rest, ",", &rest));
  char *descreveData = copyStr(strtok_single(rest, ",", &rest));
  char *descreveLugares = copyStr(strtok_single(rest, ",", &rest));
  char *descreveLinha = copyStr(strtok_single(rest, ",", &rest));
  char *descreveModelo = copyStr(strtok_single(rest, ",", &rest));
  char *descreveCategoria = copyStr(strtok_single(rest, ",", &rest));

  setVehicleHeader(vf, descrevePrefixo, descreveData, descreveLugares,
                   descreveLinha, descreveModelo, descreveCategoria);
}

/**
 * @brief Transforma linha de CSV em regsitro de veículo
 *
 * @param line Linha de CSV
 * @return lineRecord*
 */
vehicleRecord *parseLineVehicle(char *line) {
  char *rest = line;

  boolean removido;
  int32_t tamanhoModelo;
  int32_t tamanhoCategoria;

  char *prefixo = safeToId(strtok_single(rest, ",", &rest), 5, &removido);
  char *data = safeToFixed(strtok_single(rest, ",", &rest), 10);
  int32_t quantidadeLugares = safeToInt(strtok_single(rest, ",", &rest));
  int32_t codLinha = safeToInt(strtok_single(rest, ",", &rest));
  char *model = safeToDynamic(strtok_single(rest, ",", &rest), &tamanhoModelo);
  char *categoria =
      safeToDynamic(strtok_single(rest, ",", &rest), &tamanhoCategoria);

  return newVehicleRecord(removido, prefixo, data, quantidadeLugares, codLinha,
                          tamanhoModelo, model, tamanhoCategoria, categoria);
}

/**
 * @brief Lê header e registros de um arquivo CSV presente em fp para vf
 *
 * @param vf
 * @param filename
 */
void parseCSVToVehicles(vehicleFile *vf, char *filename) {
  FILE *fp = fopen(filename, "r");

  if (!fp) {
    return;
  }

  char *tmp;
  vehicleRecord *tmpRec;

  tmp = readLine(fp); // Lê cabeçalho
  parseDescVehicle(vf, tmp);
  free(tmp);

  while (!feof(fp)) {
    tmp = readLine(fp);

    if ((*tmp) == 0) {
      free(tmp);
      continue;
    }

    tmpRec = parseLineVehicle(tmp);

    if (tmpRec->removido == '1') {
      vf->header->nroRegistros++;
    } else {
      vf->header->nroRegRemovidos++;
    }

    if ((vf->nRecords) % VEHICLE_BUFFER == 0) {
      vf->records = (vehicleRecord **)realloc(
          vf->records, ((vf->nRecords) / VEHICLE_BUFFER + 1) * VEHICLE_BUFFER *
                           sizeof(vehicleRecord *));
    }
    vf->records[(vf->nRecords)++] = tmpRec;
    vf->header->byteProxReg += tmpRec->tamanhoRegistro + 5;

    free(tmp);
  }

  vf->records = (vehicleRecord **)realloc(
      vf->records, ((vf->nRecords)) * sizeof(vehicleRecord *));

  vf->readRecords = true;

  fclose(fp);
}

/**
 * @brief Recupera prefixo da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return char*
 */
char *getPrefix(FILE *fp, int *read) {
  char *prefixo = prepStr(5);
  fread(prefixo, sizeof(char), 5, fp);

  *read = 5;

  if (*prefixo == 0) {
    free(prefixo);
    return NULL;
  }
  return prefixo;
}

/**
 * @brief Recupera data da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return char*
 */
char *getDate(FILE *fp, int *read) {
  char *data = prepStr(10);
  fseek(fp, 5, SEEK_CUR);
  fread(data, sizeof(char), 10, fp);

  *read = 15;

  if (*data == 0) {
    free(data);
    return NULL;
  }
  return data;
}

/**
 * @brief Recupera quantidadeLugares da posição corrente de fp e guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return int32_t
 */
int32_t getPlaces(FILE *fp, int *read) {
  int32_t i;
  fseek(fp, 15, SEEK_CUR);
  fread(&i, sizeof(int32_t), 1, fp);
  *read = 19;
  return i;
}

/**
 * @brief Recupera modelo da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return char*
 */
char *getModel(FILE *fp, int *read) {
  int32_t len;

  fseek(fp, 23, SEEK_CUR);
  fread(&len, sizeof(int32_t), 1, fp);

  if (len == 0) {
    *read = 27;
    return NULL;
  }

  char *model = prepStr(len);

  fread(model, sizeof(char), len, fp);

  *read = 27 + len;

  return model;
}
/**
 * @brief Recupera categoria da posição corrente de fp e guarda quantidade de
 * bytes pulados/lidos em read
 *
 * @param fp
 * @param read
 * @return char*
 */
char *getCategory(FILE *fp, int *read) {
  int32_t lenM;
  int32_t lenCat;

  fseek(fp, 23, SEEK_CUR);
  fread(&lenM, sizeof(int32_t), 1, fp);
  fseek(fp, lenM, SEEK_CUR);
  fread(&lenCat, sizeof(int32_t), 1, fp);

  if (lenCat == 0) {
    *read = 31 + lenM;
    return NULL;
  }

  char *categoria = prepStr(lenCat);

  fread(categoria, sizeof(char), lenCat, fp);

  *read = 31 + lenM + lenCat;

  return categoria;
}

/**
 * @brief Verifica se prefixo da posição corrente de fp é igual a src. Guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkPrefix(char *src, FILE *fp, int *read) {
  return checkMatch(src, getPrefix(fp, read));
}
/**
 * @brief Verifica se data da posição corrente de fp é igual a src. Guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkDate(char *src, FILE *fp, int *read) {
  return checkMatch(src, getDate(fp, read));
}
/**
 * @brief Verifica se quantidadeLugares da posição corrente de fp é igual a src.
 * Guarda quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkPlaces(int src, FILE *fp, int *read) {
  return getPlaces(fp, read) == src;
}
/**
 * @brief Verifica se modelo da posição corrente de fp é igual a src. Guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkModel(char *src, FILE *fp, int *read) {
  return checkMatch(src, getModel(fp, read));
}
/**
 * @brief Verifica se categoria da posição corrente de fp é igual a src. Guarda
 * quantidade de bytes pulados/lidos em read
 *
 * @param src
 * @param fp
 * @param read
 * @return boolean
 */
boolean checkCategory(char *src, FILE *fp, int *read) {
  return checkMatch(src, getCategory(fp, read));
}
