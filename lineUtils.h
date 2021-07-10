/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#ifndef _LINE_UTIL_H_
#define _LINE_UTIL_H_

#include <stdint.h>
#define LINE_BUFFER 127

#include "line.h"
#include "util.h"

void printCard(char *aceitaCartao);

void parseCSVToLines(lineFile *vf, char *filename);

boolean checkCode(int32_t src, FILE *fp, int *read);

boolean checkCard(char *src, FILE *fp, int *read);

boolean checkName(char *src, FILE *fp, int *read);

boolean checkColor(char *src, FILE *fp, int *read);

#endif
