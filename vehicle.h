// Autor: Eduardo Amaral - NUSP 11735021

#ifndef _VEHICLE_H_
#define _VEHICLE_H_

#include "util.h"

#include <stdint.h>
#include <stdio.h>
/**
 * @brief Estrutura responsável pelos registros de veículos
 *
 */
typedef struct vehicleRecord {
  byte removido;
  int32_t tamanhoRegistro;
  char *prefixo;
  char *data;
  int32_t quantidadeLugares;
  int32_t codLinha;
  int32_t tamanhoModelo;
  char *model;
  int32_t tamanhoCategoria;
  char *categoria;
} vehicleRecord;
/**
 * @brief Estrutura responsável pelo header dos arquivos de veículo
 *
 */
typedef struct vehicleFileHeader {
  byte status;
  int64_t byteProxReg;
  int32_t nroRegistros;
  int32_t nroRegRemovidos;
  char *descrevePrefixo;
  char *descreveData;
  char *descreveLugares;
  char *descreveLinha;
  char *descreveModelo;
  char *descreveCategoria;
} vehicleFileHeader;
/**
 * @brief Estrutura responsável por guardar header, registros e infos adicionais
 * sobre o arquivo de veículos
 *
 */
typedef struct vehicleFile {
  FILE *fp;
  boolean readRecords;
  char *openMode;
  int nRecords;
  vehicleFileHeader *header;
  vehicleRecord **records;
} vehicleFile;

vehicleFile *createVehicleFileStruct(char *filename, char *mode);

void destroyVehicleFile(vehicleFile *vf);

void setVehicleHeader(vehicleFile *vf, char *descrevePrefixo,
                      char *descreveData, char *descreveLugares,
                      char *descreveLinha, char *descreveModelo,
                      char *descreveCategoria);
vehicleRecord *newVehicleRecord(boolean deleted, char *prefixo, char *data,
                                int32_t quantidadeLugares, int32_t codLinha,
                                int32_t tamanhoModelo, char *model,
                                int32_t tamanhoCategoria, char *categoria);

void writeVehicleFile(vehicleFile *vf);

void readVehicleFile(vehicleFile *vf, boolean readRecords);

void printVehicleFile(vehicleFile *vf);

boolean printMatchingRecordsVehicle(vehicleFile *vf, char *field, char *val);

void insertVehicles(int n, vehicleFile *vf);

void readVehicleFileHeader(vehicleFile *vf);

void readVehicleReg(FILE *fp, vehicleRecord *vr);

#endif
