/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
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
  int nroInsercoes;
  vehicleRecord *vr;
  lineRecord *lr;

  scanf(" %d", &op);
  vehicleFile *vf;
  lineFile *lf;

  switch(op){
    //funcionalidade: cria um arquivo de arvore B (indices) a partir de um arquivo de registros
    //de veiculo
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
    //funcionalidade: cria um arquivo de arvore B (indices) a partir de um arquivo de registros
    //de linha
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
    //funcionalidade: busca na arvore B a partir de um campo de registro de veiculo
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
      //funcionalidade: busca na arvore B a partir de um campo de registro de linha
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
    
    case 13:
      //funcionalidade: insercao de registros no arquivo de veiculos + insercao na arvore B
      scanf(" %s %s %d", regFileName, indFileName, &nroInsercoes);
      vf = createVehicleFileStruct(regFileName, "rb+");
      arvore = criaArvoreB(indFileName, "rb+");

      if(!vf){
        printf("Falha no processamento do arquivo.\n");
        break;
      }
      else if(!arvore){
        printf("Falha no processamento do arquivo.\n");
        break;
      }

      readVehicleFile(vf, false);
      //le registros a serem inseridos, insere-os no arquivo de registros e na arvore B
      for(int i = 0; i < nroInsercoes; i++){
        int64_t offsetCorrente = vf->header->byteProxReg;
        vr = insertOneVehicle(vf);
        int chave = convertePrefixo(vr->prefixo);
        inserirNaArvoreB(arvore, criaChavePonteiroPreenchida(chave, offsetCorrente));
        
      }
      writeVehicleFileHeader(vf);
      destroiArvoreB(arvore);
      destroyVehicleFile(vf);
      binarioNaTela(indFileName);
      
      break;
    
    case 14:
      //funcionalidade: insercao de registros no arquivo de linha + insercao na arvore B
      scanf(" %s %s %d", regFileName, indFileName, &nroInsercoes);

      lf = createLineFileStruct(regFileName, "rb+");
      arvore = criaArvoreB(indFileName, "rb+");

      if(!lf){
        printf("Falha no processamento do arquivo.\n");
        break;
      }
      else if(!arvore){
        printf("Falha no processamento do arquivo.\n");
        break;
      }
      
      readLineFile(lf, false);
      //le registros a serem inseridos, insere-os no arquivo de registros e na arvore B
      for(int i = 0; i < nroInsercoes; i++){
        int64_t offsetCorrente = lf->header->byteProxReg;
        lr = insertOneLine(lf);
        int chave = lr->codLinha;
        inserirNaArvoreB(arvore, criaChavePonteiroPreenchida(chave, offsetCorrente));
      }

      writeLineFileHeader(lf);
      destroiArvoreB(arvore);
      destroyLineFile(lf);
      binarioNaTela(indFileName);
      break;

    default:  //debug
      // scanf(" %[^\n]s", indFileName);
      // lf = createLineFileStruct(indFileName, "rb");
      // readLineFileHeader(lf);
      // printf("n registros: %d\n", lf->header->nroRegistros);
      // printf("BPR: %ld\n", lf->header->byteProxReg);
      // destroyLineFile(lf);
      // binarioNaTela(indFileName);
      exit(0);
  }

  free(indFileName);
  free(regFileName);
  free(chaveDeBusca);

  return 0;
}
