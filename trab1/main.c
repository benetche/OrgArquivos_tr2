// Autor: Eduardo Amaral - NUSP 11735021

#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#include "vehicle.h"
#include "vehicleUtils.h"

#include "line.h"
#include "lineUtils.h"

/**
 * @brief Método inicial do programa
 * 
 * @return int Retorna 0 se a execução do programa ocorreu sem problemas
 */
int main() {

    int opt;
    int nInserts;
    char *csvFilename = malloc(sizeof(char) * 128);
    char *binFilename = malloc(sizeof(char) * 128);
    char *fieldValue = malloc(sizeof(char) * 128);
    char *fieldName = malloc(sizeof(char) * 32);

    vehicleFile *vf;
    lineFile *lf;

    scanf(" %d", &opt);

    switch (opt) {
    case 1:
        scanf(" %s %[^\n]s", csvFilename, binFilename);
        vf = createVehicleFileStruct(binFilename, "wb");

        if (!vf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        parseCSVToVehicles(vf, csvFilename);

        if (!vf->readRecords) {
            printf("Falha no processamento do arquivo.\n");
            destroyVehicleFile(vf);
            break;
        }

        writeVehicleFile(vf);
        destroyVehicleFile(vf);
        binarioNaTela(binFilename);
        break;
    case 2:
        scanf(" %s %[^\n]s", csvFilename, binFilename);
        lf = createLineFileStruct(binFilename, "wb");

        if (!lf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        parseCSVToLines(lf, csvFilename);

        if (!lf->readRecords) {
            printf("Falha no processamento do arquivo.\n");
            destroyLineFile(lf);
            break;
        }

        writeLineFile(lf);
        destroyLineFile(lf);
        binarioNaTela(binFilename);
        break;
    case 3:
        scanf(" %[^\n]s", binFilename);

        vf = createVehicleFileStruct(binFilename, "rb");

        if (!vf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        readVehicleFile(vf, true);

        if (vf->nRecords == 0) {
            printf("Registro inexistente.\n");
        }

        printVehicleFile(vf);

        destroyVehicleFile(vf);
        break;
    case 4:
        scanf(" %[^\n]s", binFilename);
        lf = createLineFileStruct(binFilename, "rb");

        if (!lf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        readLineFile(lf, true);

        if (lf->nRecords == 0) {
            printf("Registro inexistente.\n");
        }

        printLineFile(lf);

        destroyLineFile(lf);
        break;
    case 5:
        scanf(" %s %s", binFilename, fieldName);
        scan_quote_string(fieldValue);
        vf = createVehicleFileStruct(binFilename, "rb");

        if (!vf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        readVehicleFile(vf, false);

        if (!printMatchingRecordsVehicle(vf, fieldName, fieldValue)) {
            printf("Registro inexistente.\n");
        }

        destroyVehicleFile(vf);
        break;
    case 6:
        scanf(" %s %s", binFilename, fieldName);
        scan_quote_string(fieldValue);
        lf = createLineFileStruct(binFilename, "rb");

        if (!lf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        readLineFile(lf, false);

        if (!printMatchingRecordsLine(lf, fieldName, fieldValue)) {
            printf("Registro inexistente.\n");
        }

        destroyLineFile(lf);
        break;
    case 7:
        scanf(" %s %d", binFilename, &nInserts);
        vf = createVehicleFileStruct(binFilename, "rb+");

        if (!vf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        readVehicleFile(vf, false);

        insertVehicles(nInserts, vf);

        destroyVehicleFile(vf);

        binarioNaTela(binFilename);

        break;
    case 8:
        scanf(" %s %d", binFilename, &nInserts);
        lf = createLineFileStruct(binFilename, "rb+");

        if (!lf) {
            printf("Falha no processamento do arquivo.\n");
            break;
        }

        readLineFile(lf, false);

        insertLines(nInserts, lf);

        destroyLineFile(lf);

        binarioNaTela(binFilename);
        break;
    default:
        break;
    }

    free(fieldName);
    free(fieldValue);
    free(csvFilename);
    free(binFilename);

    return 0;
}
