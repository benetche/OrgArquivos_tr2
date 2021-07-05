
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#include "vehicle.h"
#include "vehicleUtils.h"

#include "line.h"
#include "lineUtils.h"


int main(void){
    char reg_fileName[30];  //nome do arquivo .bin de registro
    char ind_fileName[30];  //nome do arquivo .bin de indices arvoreB
    unsigned int op;

    printf("Digite uma operacao:\n");

    scanf("%ud", &op);


    switch(op){
        case 10:
            printf("DIGITE [arquivo de linha] + [arquivo de indices]\n");
            scanf("%s", reg_fileName);
            scanf("%s", ind_fileName);
            break;

        default:

            break;
    }

    return 0;
}