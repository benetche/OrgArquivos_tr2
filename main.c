
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

  int op;

  scanf(" %d", &op);
  vehicleFile *vf;

  if (op == 9) {
    scanf(" %s %[^\n]s", regFileName, indFileName);

    vf = createVehicleFileStruct(regFileName, "rb");

    if (!vf) {
      printf("Falha no processamento do arquivo.\n");
    } else {

      arvoreB *arvore = criaArvoreB(indFileName);

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
  }

  free(indFileName);
  free(regFileName);

  return 0;
}
