/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/**
 * @brief Verifica se string é nula ('vazia')
 *
 * @param str
 * @return boolean
 */
boolean isStrNull(char *str) { return *str == 0; }

/**
 * @brief Verifica se o inteiro é nulo (-1)
 *
 * @param i
 * @return boolean
 */
boolean isIntNull(int32_t i) { return i == -1; }

/**
 * @brief Escreve status como o primeiro byte do arquivo fp
 *
 * @param fp
 * @param status
 */
void setFileStatus(FILE *fp, byte status) {

  fseek(fp, 0, SEEK_SET);

  fwrite(&status, sizeof(byte), 1, fp);
}

/**
 * @brief Verifica se a string presente em src é igual a presente em dest.
 * Desaloca a string dest
 *
 * @param src
 * @param dest
 * @return boolean
 */
boolean checkMatch(char *src, char *dest) {

  if (dest == NULL) {
    return src == NULL;
  }

  if (dest == NULL) {
    free(dest);
    return false;
  }

  boolean ans = strcmp(dest, src) == 0;

  free(dest);

  return ans;
}
/**
 * @brief Aloca size+1 bytes para criação de uma string. Escreve \0 no fim do
 * espaço alocado.
 *
 * @param size
 * @return char*
 */
char *prepStr(int size) {
  char *str = (char *)malloc(sizeof(char) * size + 1);
  str[size] = 0;
  return str;
}
/**
 * @brief Implementação thread-safe da função strtok
 *
 * @param str
 * @param delims
 * @param save_ptr
 * @return char*
 */
char *strtok_single(char *str, char const *delims, char **save_ptr) {
  char *p, *ret = NULL;

  if (str != NULL)
    (*save_ptr) = str;

  if ((*save_ptr) == NULL)
    return NULL;

  if ((p = strpbrk((*save_ptr), delims)) != NULL) {
    *p = 0;
    ret = (*save_ptr);
    (*save_ptr) = ++p;
  } else if (*(*save_ptr)) {
    ret = (*save_ptr);
    (*save_ptr) = NULL;
  }

  return ret;
}

/**
 * @brief Retorna cópia de src
 *
 * @param src
 * @return char*
 */
char *copyStr(char *src) {
  int len = strlen(src);
  if (len == 0) {
    return NULL;
  }
  char *copy = (char *)malloc(sizeof(char) * (len + 1));
  memcpy(copy, src, len + 1);
  return copy;
}

/**
 * @brief Preenche string val por completo até size. Utiliza \0 e '@' caso
 * necessário.
 *
 * @param val
 * @param size
 */
void paddString(char *val, int size) {
  int len = strlen(val);

  if (len == size) {
    return;
  }

  memset(val + (len + 1), '@', size - len);

  // Descomente para melhor entendimento da função
  // printf("%d\n", size);

  // for (int i = 0; i <size+1; i++) {
  //     if (val[i] == 0) {
  //         printf(".");
  //         continue;
  //     }
  //     printf("%c", val[i]);
  // }
  // printf("\n");
}

/**
 * @brief Transforma val em inteiro considerando possibilidade de val ser nulo.
 *
 * @param val
 * @return int32_t
 */
int32_t safeToInt(char *val) {
  if (strcmp(val, "NULO") == 0) {
    return -1;
  }
  return atoi(val);
}
/**
 * @brief Transforma val em string de tamanho fixo considerando a possibilidade
 * de val ser nulo.
 *
 * @param val
 * @param size
 * @return char*
 */
char *safeToFixed(char *val, int size) {
  char *res = NULL;

  if (strcmp(val, "NULO") == 0) {
    res = (char *)calloc(size + 1, sizeof(char));
  } else {
    res = copyStr(val);
    res = (char *)realloc(res, (size + 1) * sizeof(char));
  }

  paddString(res, size);

  return res;
}

/**
 * @brief Transforma val em string de tamanho fixo considerando a possibilidade
 * de val ser nulo. Verifica se string indica que o registro é removido e salva
 * indicação em removido
 *
 * @param val
 * @param size
 * @param removido
 * @return char*
 */
char *safeToId(char *val, int size, char *removido) {
  char *res = NULL;
  *removido = true;

  if (val[0] == '*') {
    *removido = false;
    val++;
  }

  res = copyStr(val);
  res = (char *)realloc(res, (size + 1) * sizeof(char));

  paddString(res, size);

  return res;
}
/**
 * @brief Transforma val em string de tamanho dinâmico considerando a
 * possibilidade de val ser nulo.
 *
 * @param val
 * @param size
 * @return char*
 */
char *safeToDynamic(char *val, int32_t *size) {
  if (strcmp(val, "NULO") == 0) {
    *size = 0;
    return NULL;
  } else {
    *size = strlen(val);
    return copyStr(val);
  }
}
/**
 * @brief Lê linha terminada por \n ou de fim de arquivo do arquivo stream.
 *
 * @param stream
 * @return char*
 */
char *readLine(FILE *stream) {
  char *string = NULL;
  int size = 0;
  char c;

  fscanf(stream, " ");

  do {
    fscanf(stream, "%c", &c);

    if ((c == '\n' && size == 0) || c == 13) {
      c = 0;
    } else {
      if (size % READLINE_BUFFER == 0) {
        string = (char *)realloc(string, (size / READLINE_BUFFER + 1) *
                                             READLINE_BUFFER * sizeof(char));
      }
      string[size++] = c;
    }

  } while (c != '\n' && !feof(stream));

  string[--size] = 0;

  string = (char *)realloc(string, (size + 1) * sizeof(char));

  return string;
}

/**
 * @brief Função fornecida pelo docente
 *
 * @param nomeArquivoBinario
 */
void binarioNaTela(char *nomeArquivoBinario) { /* Você não precisa entender o
                                                  código dessa função. */

  /* Use essa função para comparação no run.codes. Lembre-se de ter fechado
   * (fclose) o arquivo anteriormente. Ela vai abrir de novo para leitura e
   * depois fechar (você não vai perder pontos por isso se usar ela). */

  unsigned long i, cs;
  unsigned char *mb;
  size_t fl;
  FILE *fs;
  if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
    fprintf(stderr,
            "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
            "não foi possível abrir o arquivo que me passou para leitura. "
            "Ele existe e você tá passando o nome certo? Você lembrou de "
            "fechar ele com fclose depois de usar?\n");
    return;
  }
  fseek(fs, 0, SEEK_END);
  fl = ftell(fs);
  fseek(fs, 0, SEEK_SET);
  mb = (unsigned char *)malloc(fl);
  fread(mb, 1, fl, fs);

  cs = 0;
  for (i = 0; i < fl; i++) {
    cs += (unsigned long)mb[i];
  }
  printf("%lf\n", (cs / (double)100));
  free(mb);
  fclose(fs);
}

/**
 * @brief Função fornecida pelo docente
 *
 * @param str
 */
void scan_quote_string(char *str) {

  /*
   *	Use essa função para ler um campo string delimitado entre aspas (").
   *	Chame ela na hora que for ler tal campo. Por exemplo:
   *
   *	A entrada está da seguinte forma:
   *		nomeDoCampo "MARIA DA SILVA"
   *
   *	Para ler isso para as strings já alocadas str1 e str2 do seu programa,
   *você faz: scanf("%s", str1); // Vai salvar nomeDoCampo em str1
   *		scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2
   *(sem as aspas)
   *
   */

  char R;

  while ((R = getchar()) != EOF && isspace(R))
    ; // ignorar espaços, \r, \n...

  if (R == 'N' || R == 'n') { // campo NULO
    getchar();
    getchar();
    getchar();       // ignorar o "ULO" de NULO.
    strcpy(str, ""); // copia string vazia
  } else if (R == '\"') {
    if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
      strcpy(str, "");
    }
    getchar();           // ignorar aspas fechando
  } else if (R != EOF) { // vc tá tentando ler uma string que não tá entre
                         // aspas! Fazer leitura normal %s então, pois deve
                         // ser algum inteiro ou algo assim...
    str[0] = R;
    scanf("%s", &str[1]);
  } else { // EOF
    strcpy(str, "");
  }
}

/* ---------------- EXTRA ----------------

OPCIONAL: dicas sobre scanf() e fscanf():

scanf("%[^,]", string) -> lê até encontrar o caractere ',', não incluindo o
mesmo na leitura.

Exemplo de entrada: "Oi, esse é um exemplo."
Nesse caso, o scanf("%[^,]") tem como resultado a string "Oi";

scanf("%[^\"]", string) -> lê até encontrar o caractere '"', não incluindo o
mesmo na leitura. scanf("%[^\n]", string) -> lê até encontrar o fim da linha,
não incluindo o '\n' na leitura.

scanf("%*c") --> lê um char e não guarda em nenhuma variável, como se tivesse
ignorado ele

*/
