
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#include "vehicle.h"
#include "vehicleUtils.h"

#include "line.h"
#include "lineUtils.h"


int main(void){
    char *regFileName = (char *) malloc(sizeof(char) * 128);
    char *indFileName = (char *) malloc(sizeof(char) * 128);

    unsigned int op;

    printf("Digite uma operacao:\n");

    scanf("%ud", &op);
    lineFile *lf;

    switch(op){
        case 10:
            printf("DIGITE [arquivo de linha] + [arquivo de indices]\n");
            scanf("%[^\n]s", regFileName);
            scanf("%[^\n]s", indFileName);

            lf = createLineFileStruct(regFileName, "rb");

            if (!lf) {
                printf("Falha no processamento do arquivo.\n");
                break;
            }

            readLineFile(lf, true);

            if(lf->nRecords == 0){
                printf("Registro inexistente.\n");
            }

            
            destroyLineFile(lf);
            break;

        default:

            break;
    }

    return 0;
}