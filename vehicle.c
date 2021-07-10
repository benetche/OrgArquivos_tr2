/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#include "util.h"
#include "vehicleUtils.h"
#include "bTree.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Cria Struct que guarda dados de header, registros e informações
 * adicionais sobre o arquivo de veículos.
 *
 * @param filename Nome do arquivo à ser utilizado
 * @param mode Modo de abertura do arquivo (wb, rb, rb+)
 * @return vehicleFile* Retorna estrutura alocada na heap.
 */
vehicleFile *createVehicleFileStruct(char *filename, char *mode) {
  vehicleFile *vf = (vehicleFile *)malloc(sizeof(vehicleFile));
  vf->nRecords = 0;
  vf->openMode = mode;
  vf->readRecords = false;

  vf->fp = fopen(filename, mode);

  if (!vf->fp) {
    free(vf);
    return NULL;
  }

  // Verifica se arquivo já existe e se não está corrompido
  if (strcmp(mode, "wb") != 0) {
    byte status;
    if (fread(&status, sizeof(byte), 1, vf->fp) != 1 || status != '1') {
      fclose(vf->fp);
      free(vf);
      return NULL;
    }
  }

  vf->header = (vehicleFileHeader *)malloc(sizeof(vehicleFileHeader));
  vf->header->byteProxReg = 0;
  vf->header->nroRegRemovidos = vf->header->nroRegistros = 0;

  if (strcmp(mode, "rb") != 0) {
    setFileStatus(vf->fp, '0');
    vf->header->status = '0';
  }

  vf->records = NULL;

  return vf;
}
/**
 * @brief Cria novo registro de veículo na memória principal
 *
 * @param removido
 * @param prefixo
 * @param data
 * @param quantidadeLugares
 * @param codLinha
 * @param tamanhoModelo
 * @param model
 * @param tamanhoCategoria
 * @param categoria
 * @return vehicleRecord* Retorna registro alocado na heap
 */
vehicleRecord *newVehicleRecord(boolean removido, char *prefixo, char *data,
                                int32_t quantidadeLugares, int32_t codLinha,
                                int32_t tamanhoModelo, char *model,
                                int32_t tamanhoCategoria, char *categoria) {

  vehicleRecord *v = (vehicleRecord *)malloc(sizeof(vehicleRecord));

  v->removido = removido ? '1' : '0';
  v->prefixo = prefixo;
  v->data = data;
  v->quantidadeLugares = quantidadeLugares;
  v->codLinha = codLinha;
  v->tamanhoModelo = tamanhoModelo;
  v->model = model;
  v->tamanhoCategoria = tamanhoCategoria;
  v->categoria = categoria;

  v->tamanhoRegistro =
      5 + 10 + 4 + 4 + 4 + tamanhoModelo + 4 + tamanhoCategoria;

  return v;
}

/**
 * @brief Configura header da estrutura vehicleFile
 *
 * @param vf Struct a ser configurada
 * @param descrevePrefixo
 * @param descreveData
 * @param descreveLugares
 * @param descreveLinha
 * @param descreveModelo
 * @param descreveCategoria
 */
void setVehicleHeader(vehicleFile *vf, char *descrevePrefixo,
                      char *descreveData, char *descreveLugares,
                      char *descreveLinha, char *descreveModelo,
                      char *descreveCategoria) {
  vf->header->descrevePrefixo = descrevePrefixo;
  vf->header->descreveData = descreveData;
  vf->header->descreveLugares = descreveLugares;
  vf->header->descreveLinha = descreveLinha;
  vf->header->descreveModelo = descreveModelo;
  vf->header->descreveCategoria = descreveCategoria;
  vf->header->byteProxReg += 1 + 8 + 4 + 4 + 18 + 35 + 42 + 26 + 17 + 20;
}

/**
 * @brief Desaloca por completo estrutura de registro de veículo
 *
 * @param v Registro a ser desalocado
 */
void destroyVehicleRecord(vehicleRecord *v) {
  free(v->prefixo);
  free(v->data);
  free(v->categoria);
  free(v->model);
  free(v);
}

/**
 * @brief Desaloca por completo header de veículo
 *
 * @param vh Header a ser desalocado
 */
void destroyVehicleHeader(vehicleFileHeader *vh) {
  free(vh->descrevePrefixo);
  free(vh->descreveData);
  free(vh->descreveLugares);
  free(vh->descreveLinha);
  free(vh->descreveModelo);
  free(vh->descreveCategoria);
  free(vh);
}
/**
 * @brief Desaloca por completo estrutura de arquivo de veículos
 *
 * @param vf Struct de Arquivo de Veículos a ser desalocado
 */
void destroyVehicleFile(vehicleFile *vf) {

  if (vf->readRecords) {
    for (int i = 0; i < vf->nRecords; i++) {
      destroyVehicleRecord(vf->records[i]);
    }

    free(vf->records);
  }

  if (strcmp(vf->openMode, "rb") != 0) {
    setFileStatus(vf->fp, '1');
    vf->header->status = '1';
  }

  destroyVehicleHeader(vf->header);

  fclose(vf->fp);
  free(vf);
}
/**
 * @brief Escreve o header da memória principal no arquivo descrito por vf
 *
 * @param vf
 */
void writeVehicleFileHeader(vehicleFile *vf) {
  fseek(vf->fp, 1, SEEK_SET);
  fwrite(&vf->header->byteProxReg, sizeof(int64_t), 1, vf->fp);
  fwrite(&vf->header->nroRegistros, sizeof(int32_t), 1, vf->fp);
  fwrite(&vf->header->nroRegRemovidos, sizeof(int32_t), 1, vf->fp);
  fwrite(vf->header->descrevePrefixo, sizeof(char), 18, vf->fp);
  fwrite(vf->header->descreveData, sizeof(char), 35, vf->fp);
  fwrite(vf->header->descreveLugares, sizeof(char), 42, vf->fp);
  fwrite(vf->header->descreveLinha, sizeof(char), 26, vf->fp);
  fwrite(vf->header->descreveModelo, sizeof(char), 17, vf->fp);
  fwrite(vf->header->descreveCategoria, sizeof(char), 20, vf->fp);
}
/**
 * @brief Escreve o registro de veículo vr no arquivo fp
 *
 * @param fp
 * @param vr
 */
void writeVehicleReg(FILE *fp, vehicleRecord *vr) {
  fwrite(&vr->removido, sizeof(byte), 1, fp);
  fwrite(&vr->tamanhoRegistro, sizeof(int32_t), 1, fp);
  fwrite(vr->prefixo, sizeof(char), 5, fp);
  fwrite(vr->data, sizeof(char), 10, fp);
  fwrite(&vr->quantidadeLugares, sizeof(int32_t), 1, fp);
  fwrite(&vr->codLinha, sizeof(int32_t), 1, fp);
  fwrite(&vr->tamanhoModelo, sizeof(int32_t), 1, fp);
  fwrite(vr->model, sizeof(char), vr->tamanhoModelo, fp);
  fwrite(&vr->tamanhoCategoria, sizeof(int32_t), 1, fp);
  fwrite(vr->categoria, sizeof(char), vr->tamanhoCategoria, fp);
}

/**
 * @brief Escreve o header e os registros de veículo no arquivo descrito por vf
 *
 * @param vf
 */
void writeVehicleFile(vehicleFile *vf) {
  writeVehicleFileHeader(vf);

  for (int i = 0; i < vf->nRecords; i++) {
    writeVehicleReg(vf->fp, vf->records[i]);
  }
}

/**
 * @brief Recupera header do arquivo descrito por vf para a memória principal
 *
 * @param vf
 */
void readVehicleFileHeader(vehicleFile *vf) {
  fseek(vf->fp, 0, SEEK_SET);
  fread(&vf->header->status, sizeof(byte), 1, vf->fp);
  fread(&vf->header->byteProxReg, sizeof(int64_t), 1, vf->fp);
  fread(&vf->header->nroRegistros, sizeof(int32_t), 1, vf->fp);
  fread(&vf->header->nroRegRemovidos, sizeof(int32_t), 1, vf->fp);

  vf->header->descrevePrefixo = prepStr(18);
  vf->header->descreveData = prepStr(35);
  vf->header->descreveLugares = prepStr(42);
  vf->header->descreveLinha = prepStr(26);
  vf->header->descreveModelo = prepStr(17);
  vf->header->descreveCategoria = prepStr(20);

  fread(vf->header->descrevePrefixo, sizeof(char), 18, vf->fp);
  fread(vf->header->descreveData, sizeof(char), 35, vf->fp);
  fread(vf->header->descreveLugares, sizeof(char), 42, vf->fp);
  fread(vf->header->descreveLinha, sizeof(char), 26, vf->fp);
  fread(vf->header->descreveModelo, sizeof(char), 17, vf->fp);
  fread(vf->header->descreveCategoria, sizeof(char), 20, vf->fp);
}

/**
 * @brief Recupera registro presente na posição corrente de fp e o salva em vr
 *
 * @param fp
 * @param vr
 */
void readVehicleReg(FILE *fp, vehicleRecord *vr) {
  fread(&vr->removido, sizeof(byte), 1, fp);
  fread(&vr->tamanhoRegistro, sizeof(int32_t), 1, fp);
  vr->prefixo = prepStr(5);
  vr->data = prepStr(10);
  fread(vr->prefixo, sizeof(char), 5, fp);
  fread(vr->data, sizeof(char), 10, fp);
  fread(&vr->quantidadeLugares, sizeof(int32_t), 1, fp);
  fread(&vr->codLinha, sizeof(int32_t), 1, fp);
  fread(&vr->tamanhoModelo, sizeof(int32_t), 1, fp);
  vr->model = prepStr(vr->tamanhoModelo);
  fread(vr->model, sizeof(char), vr->tamanhoModelo, fp);
  fread(&vr->tamanhoCategoria, sizeof(int32_t), 1, fp);
  vr->categoria = prepStr(vr->tamanhoCategoria);
  fread(vr->categoria, sizeof(char), vr->tamanhoCategoria, fp);
}
/**
 * @brief Lê arquivo binário de veículos para a estrutura vf
 *
 * @param vf
 * @param readRecords Define se os registros do arquivo devem ser lidos para a
 * memória principal
 */
void readVehicleFile(vehicleFile *vf, boolean readRecords) {
  readVehicleFileHeader(vf);

  vf->nRecords = vf->header->nroRegRemovidos + vf->header->nroRegistros;

  if (readRecords) {
    vf->readRecords = true;
    vf->records =
        (vehicleRecord **)malloc(sizeof(vehicleRecord *) * vf->nRecords);

    for (int i = 0; i < vf->nRecords; i++) {
      vf->records[i] = (vehicleRecord *)malloc(sizeof(vehicleRecord));
      readVehicleReg(vf->fp, vf->records[i]);
    }
  }
}

/**
 * @brief Imprime registro de veículo na tela caso este não tenha sido removido
 * logicamente
 *
 * @param vh Header contendo metadados de registros de veículo
 * @param vr Registro à ser impresso
 */
void printVehicleRecord(vehicleFileHeader *vh, vehicleRecord *vr) {
  if (vr->removido == '1') {

    printf("%s: ", vh->descrevePrefixo);
    printf("%s", vr->prefixo);
    printf("\n");

    printf("%s: ", vh->descreveModelo);
    if (isStrNull(vr->model)) {
      printf(NULO);
    } else {
      printf("%s", vr->model);
    }
    printf("\n");

    printf("%s: ", vh->descreveCategoria);
    if (isStrNull(vr->categoria)) {
      printf(NULO);
    } else {
      printf("%s", vr->categoria);
    }
    printf("\n");

    printf("%s: ", vh->descreveData);
    if (isStrNull(vr->data)) {
      printf(NULO);
    } else {
      printDate(vr->data);
    }
    printf("\n");

    printf("%s: ", vh->descreveLugares);
    if (isIntNull(vr->quantidadeLugares)) {
      printf(NULO);
    } else {
      printf("%d", vr->quantidadeLugares);
    }
    printf("\n");
    printf("\n");
  }
}

/** @brief Imprime todos os registros não removidos presentes em vf
 *
 * @param vf
 */
void printVehicleFile(vehicleFile *vf) {
  for (int i = 0; i < vf->nRecords; i++) {
    printVehicleRecord(vf->header, vf->records[i]);
  }
}
/**
 * @brief Imprime registro presente posição corrente de vf->fp caso este tenha
 * valor do campo field igual a val
 *
 * @param vf
 * @param field Campo a ser comparado
 * @param val Valor de comparação
 * @return boolean Retorna true caso o registro atendeu as condições de busca
 */
boolean printIfMatchesVehicle(vehicleFile *vf, char *field, char *val) {
  byte removido;
  int32_t regSize;

  fread(&removido, sizeof(byte), 1, vf->fp);
  fread(&regSize, sizeof(int32_t), 1, vf->fp);

  if (removido == '1') {

    if (*val == 0) {
      val = NULL;
    }

    boolean matches = false;
    int32_t valP = -1;
    int read = 0;

    switch (field[0]) {
    case 'p':
      // prefixo
      matches = checkPrefix(val, vf->fp, &read);
      break;
    case 'd':
      // data
      matches = checkDate(val, vf->fp, &read);
      break;
    case 'q':
      // quantidadeLugares
      if (val != NULL) {
        valP = atoi(val);
      }
      matches = checkPlaces(valP, vf->fp, &read);
      break;
    case 'm':
      // modelo
      matches = checkPrefix(val, vf->fp, &read);
      break;
    case 'c':
      // categoria
      matches = checkPrefix(val, vf->fp, &read);
      break;
    default:
      break;
    }

    if (matches) {

      // Retorna à posição inicial do registro
      fseek(vf->fp, -(read + 5), SEEK_CUR);

      vehicleRecord *tmp = (vehicleRecord *)malloc(sizeof(vehicleRecord));

      readVehicleReg(vf->fp, tmp);

      printVehicleRecord(vf->header, tmp);

      destroyVehicleRecord(tmp);

      return true;
    }

    // Pula restante do registro
    fseek(vf->fp, regSize - read, SEEK_CUR);

    return false;
  }

  // Pula registro por completo
  fseek(vf->fp, regSize, SEEK_CUR);

  return false;
}
/**
 * @brief Imprime todos regitros de vf que possuem valor do campo field igual a
 * val
 *
 * @param vf
 * @param field Campo a ser comparado
 * @param val  Valor de comparação
 * @return boolean Retorna true caso algum registro de vf satisfaça as condições
 * de busca
 */
boolean printMatchingRecordsVehicle(vehicleFile *vf, char *field, char *val) {
  boolean matched = false;
  for (int i = 0; i < vf->nRecords; i++) {
    matched |= printIfMatchesVehicle(vf, field, val);
  }
  return matched;
}
/**
 * @brief Insere n novos registros de veículos lidos de stdin em vf
 *
 * @param n Número de linhas a serem inseridas
 * @param vf
 */
void insertVehicles(int n, vehicleFile *vf) {
  fseek(vf->fp, vf->header->byteProxReg, SEEK_SET);

  vehicleRecord *vr = (vehicleRecord *)malloc(sizeof(vehicleRecord));
  vr->removido = '1';

  char nullDate[] = {0, '@', '@', '@', '@', '@', '@', '@', '@', '@'};
  char prefixo[6];
  char data[10];
  char quantidadeLugares[12];
  char codLinha[12];
  char model[128];
  char categoria[128];

  for (int i = 0; i < n; i++) {
    vf->nRecords++;
    vf->header->nroRegistros++;

    scan_quote_string(prefixo);
    vr->prefixo = prefixo;

    scan_quote_string(data);
    if (*data == 0) {
      vr->data = nullDate;
    } else {
      vr->data = data;
    }

    scan_quote_string(quantidadeLugares);
    if (*quantidadeLugares == 0) {
      vr->quantidadeLugares = -1;
    } else {
      vr->quantidadeLugares = atoi(quantidadeLugares);
    }

    scan_quote_string(codLinha);
    if (*codLinha == 0) {
      vr->codLinha = -1;
    } else {
      vr->codLinha = atoi(codLinha);
    }

    scan_quote_string(model);
    if (*model == 0) {
      vr->tamanhoModelo = 0;
      vr->model = NULL;
    } else {
      vr->tamanhoModelo = strlen(model);
      vr->model = model;
    }

    scan_quote_string(categoria);
    if (*categoria == 0) {
      vr->tamanhoCategoria = 0;
      vr->categoria = NULL;
    } else {
      vr->tamanhoCategoria = strlen(categoria);
      vr->categoria = categoria;
    }

    vr->tamanhoRegistro =
        5 + 10 + 4 + 4 + 4 + vr->tamanhoModelo + 4 + vr->tamanhoCategoria;

    vf->header->byteProxReg += vr->tamanhoRegistro + 5;

    writeVehicleReg(vf->fp, vr);
  }

  writeVehicleFileHeader(vf);

  free(vr);
}

/**
 * @brief Le um registro a ser inserido e o insere no arquivo de registro
 * de veiculo
 * @param vf
 * @return retorna o registro lido e inserido para poder ser inserido na arvore B
 */

vehicleRecord* insertOneVehicle(vehicleFile *vf){
  fseek(vf->fp, vf->header->byteProxReg, SEEK_SET);

  vehicleRecord *vr = (vehicleRecord *)malloc(sizeof(vehicleRecord));
  vr->removido = '1';

  char nullDate[] = {0, '@', '@', '@', '@', '@', '@', '@', '@', '@'};
  char prefixo[6];
  char data[10];
  char quantidadeLugares[12];
  char codLinha[12];
  char model[128];
  char categoria[128];

  vf->nRecords++;
  scan_quote_string(prefixo);
  vr->prefixo = prefixo;

  scan_quote_string(data);
  if (*data == 0) {
    vr->data = nullDate;
  } else {
    vr->data = data;
  }

  scan_quote_string(quantidadeLugares);
  if (*quantidadeLugares == 0) {
    vr->quantidadeLugares = -1;
  } else {
    vr->quantidadeLugares = atoi(quantidadeLugares);
  }

  scan_quote_string(codLinha);
  if (*codLinha == 0) {
    vr->codLinha = -1;
  } else {
    vr->codLinha = atoi(codLinha);
  }

  scan_quote_string(model);
  if (*model == 0) {
    vr->tamanhoModelo = 0;
    vr->model = NULL;
  } else {
    vr->tamanhoModelo = strlen(model);
    vr->model = model;
  }

  scan_quote_string(categoria);
  if (*categoria == 0) {
    vr->tamanhoCategoria = 0;
    vr->categoria = NULL;
  } else {
    vr->tamanhoCategoria = strlen(categoria);
    vr->categoria = categoria;
  }

  vr->tamanhoRegistro =
      5 + 10 + 4 + 4 + 4 + vr->tamanhoModelo + 4 + vr->tamanhoCategoria;

  vf->header->byteProxReg += vr->tamanhoRegistro + 5;
  writeVehicleReg(vf->fp, vr);

  return vr;
}
