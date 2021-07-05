#include <stdio.h>
#include <stdlib.h>

int main(void){
    char reg_fileName[30];  //nome do arquivo .bin de registro
    char ind_fileName[30];  //nome do arquivo .bin de indices arvoreB
    unsigned int op;

    printf("Digite uma operacao:\n");

    scanf("%ud", &op);


    switch(op){
        case 9:
            printf("DIGITE [arquivo de registro] + [arquivo de indices]\n");
            scanf("%s", reg_fileName);
            scanf("%s", ind_fileName);
            break;

        default:

            break;
    }

    return 0;
}