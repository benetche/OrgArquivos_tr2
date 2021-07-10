/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#ifndef _UTIL_H_
#define _UTIL_H_

#define NULO "campo com valor nulo"

#define false 0
#define true 1

#define READLINE_BUFFER 255

// MACRO TEMPORARIO PARA DEBUG
#define UNUSED(x) (void)x;

#include <stdint.h>
#include <stdio.h>

typedef char boolean;
typedef char byte;

char *readLine(FILE *stream);

char *strtok_single(char *str, char const *delims, char **save_ptr);

char *copyStr(char *src);

int32_t safeToInt(char *val);

char *safeToFixed(char *val, int size);

char *safeToId(char *val, int size, boolean *removido);

char *safeToDynamic(char *val, int32_t *size);

void binarioNaTela(char *nomeArquivoBinario);

char *prepStr(int size);

boolean isStrNull(char *str);

boolean isIntNull(int32_t i);

void scan_quote_string(char *str);

void setFileStatus(FILE *fp, byte status);

boolean checkMatch(char *src, char *dest);

#endif
