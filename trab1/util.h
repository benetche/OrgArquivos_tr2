#ifndef _UTIL_H_
#define _UTIL_H_

#define READLINE_BUFFER 255

#define USERS_BUFFER 127

#include <stdio.h>
#include "user.h"

typedef char bool;

char *readLine(FILE *stream);

user **parseCSVToUsers(char *filename, int *nUsers);

#endif