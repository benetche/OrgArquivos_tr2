
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#include "vehicle.h"
#include "vehicleUtils.h"

#include "line.h"
#include "lineUtils.h"

#include "bTree.h"
#include "convertePrefixo.h"

int main(void) {
  char *regFileName = (char *)malloc(sizeof(char) * 128);
  char *indFileName = (char *)malloc(sizeof(char) * 128);
  char *chaveDeBusca = (char *)malloc(sizeof(char) * 128);
  arvoreB *arvore;
  int op;

  scanf(" %d", &op);
  vehicleFile *vf;
  lineFile *lf;

  switch(op){
    case 9:
    scanf(" %s %[^\n]s", regFileName, indFileName);

    vf = createVehicleFileStruct(regFileName, "rb");

    if (!vf) {
      printf("Falha no processamento do arquivo.\n");
    } else {

      arvore = criaArvoreB(indFileName, "wb+");

      readVehicleFileHeader(vf);

      vf->nRecords = vf->header->nroRegRemovidos + vf->header->nroRegistros;

      vehicleRecord *registroCorrente;
      int64_t offsetCorrente;

      for (int i = 0; i < vf->nRecords; i++) {
        registroCorrente = (vehicleRecord *)malloc(sizeof(vehicleRecord));
        offsetCorrente = ftell(vf->fp);
        readVehicleReg(vf->fp, registroCorrente);

        if (registroCorrente->removido == '1') {
          int32_t chave = convertePrefixo(registroCorrente->prefixo);
          inserirNaArvoreB(arvore,
                           criaChavePonteiroPreenchida(chave, offsetCorrente));
        }

        destroyVehicleRecord(registroCorrente);
      }

      destroiArvoreB(arvore);
      destroyVehicleFile(vf);
      binarioNaTela(indFileName);
    }
      break;

    case 10:
      scanf(" %s %[^\n]s", regFileName, indFileName);

      lf = createLineFileStruct(regFileName, "rb");

      if(!lf){
        printf("Falha no processamento do arquivo.\n");
      }
      else{
        arvore = criaArvoreB(indFileName, "wb+");

        readLineFileHeader(lf);

        lf->nRecords = lf->header->nroRegRemovidos + lf->header->nroRegistros;
        lineRecord *registroCorrente;
        int64_t offsetCorrente;

        for(int i = 0; i < lf->nRecords; i++){
          registroCorrente = (lineRecord *) malloc(sizeof(lineRecord));
          offsetCorrente = ftell(lf->fp);
          readLineReg(lf->fp, registroCorrente);

          if(registroCorrente->removido == '1'){
            inserirNaArvoreB(arvore, 
            criaChavePonteiroPreenchida(registroCorrente->codLinha, offsetCorrente));
          }
          destroyLineRecord(registroCorrente);
        }

        destroyLineFile(lf);
        destroiArvoreB(arvore);
        binarioNaTela(indFileName);
      }
      break;

    case 11:  
    scanf(" %s %s %*s", regFileName, indFileName);

    scan_quote_string(chaveDeBusca);

    vf = createVehicleFileStruct(regFileName, "rb");
    arvore = criaArvoreB(indFileName, "rb");

    if (!vf) {
      printf("Falha no processamento do arquivo.\n");
    } else if (!arvore) {
      destroyVehicleFile(vf);
      printf("Falha no processamento do arquivo.\n");
    } else {

      readVehicleFileHeader(vf);

      vf->nRecords = vf->header->nroRegRemovidos + vf->header->nroRegistros;

      int32_t chave = convertePrefixo(chaveDeBusca);
      printf("Chave de busca convertida: %d\n", chave);
      vehicleRecord *registroBuscado;

      int64_t offsetBuscado = buscaNaArvoreB(arvore, chave);

      if (offsetBuscado == -1) {
        printf("Registro inexistente.\n");
      } else {
        registroBuscado = (vehicleRecord *)malloc(sizeof(vehicleRecord));

        fseek(vf->fp, offsetBuscado, SEEK_SET);

        readVehicleReg(vf->fp, registroBuscado);

        if (registroBuscado->removido == '0') {
          printf("Registro inexistente.\n");
        } else {
          printVehicleRecord(vf->header, registroBuscado);
        }

        destroyVehicleRecord(registroBuscado);
      }

      destroiArvoreB(arvore);
      destroyVehicleFile(vf);
    }
      break;

    case 12:
      scanf(" %s %s %*s", regFileName, indFileName);
      scan_quote_string(chaveDeBusca);

      lf = createLineFileStruct(regFileName, "rb");
      arvore = criaArvoreB(indFileName, "rb");
      if(!lf){
        printf("Falha no processamento do arquivo.\n");
      }
      else if(!arvore){
        printf("Falha no processamento do arquivo.\n");
      }
      else{
        readLineFileHeader(lf);
        lf->nRecords = lf->header->nroRegRemovidos + lf->header->nroRegistros;

        int32_t chave = atoi(chaveDeBusca);
        lineRecord *registroBuscado;

        int64_t offsetBuscado = buscaNaArvoreB(arvore, chave);
        
        if(offsetBuscado == -1){
          printf("Registro inexistente.\n");
        }
        else{
          registroBuscado = (lineRecord *) malloc(sizeof(lineRecord));
          fseek(lf->fp, offsetBuscado, SEEK_SET);
          readLineReg(lf->fp, registroBuscado);

          if(registroBuscado->removido == '0'){
            printf("Registro removido.\n");
          }
          else{
            printLineRecord(lf->header, registroBuscado);
          }

          destroyLineRecord(registroBuscado);
        }

        destroiArvoreB(arvore);
        destroyLineFile(lf);

      }

      
      break;

    default:
      scanf(" %[^\n]s", indFileName);
      binarioNaTela(indFileName);
      exit(0);
  }

  free(indFileName);
  free(regFileName);
  free(chaveDeBusca);

  return 0;
}
