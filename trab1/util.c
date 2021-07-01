#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <ctype.h>

#include "user.h"
#include "util.h"

char *strtok_single(char *str, char const *delims, char **save_ptr)
{
    char *p, *ret = NULL;

    if (str != NULL)
        (*save_ptr) = str;

    if ((*save_ptr) == NULL)
        return NULL;

    if ((p = strpbrk((*save_ptr), delims)) != NULL)
    {
        *p = 0;
        ret = (*save_ptr);
        (*save_ptr) = ++p;
    }
    else if (*(*save_ptr))
    {
        ret = (*save_ptr);
        (*save_ptr) = NULL;
    }

    return ret;
}

// char *ltrim(char *s)
// {
//     if ((*s) == 0)
//     {
//         return s;
//     }
//     while (isspace(*s))
//     {
//         s++;
//     }
//     return s;
// }

// char *rtrim(char *s)
// {
//     if ((*s) == 0)
//     {
//         return s;
//     }

//     char *back = s + strlen(s);
//     while (isspace(*--back))
//         ;
//     *(back + 1) = 0;
//     return s;
// }

// char *trim(char *s)
// {
//     return rtrim(ltrim(s));
// }

char *copyStr(char *src)
{
    int len = strlen(src);
    if (len == 0)
    {
        return NULL;
    }
    char *copy = (char *)malloc(sizeof(char) * (len + 1));
    memcpy(copy, src, len + 1);
    return copy;
}

user *parseLine(char *line)
{
    char *rest = line;

    int id = atoi(strtok_single(rest, ",", &rest));
    char *name = copyStr(strtok_single(rest, ",", &rest));
    int age = atoi(strtok_single(rest, ",", &rest));

    user *u = newUser(id, name, age);

    return u;
}

user **parseCSVToUsers(char *filename, int *nUsers)
{
    FILE *fp = fopen(filename, "r");

    user **users = NULL;
    *nUsers = 0;

    if (!fp)
    {
        return NULL;
    }

    char *tmp;
    user *tmpUser;

    free(readLine(fp)); // Pula cabeçalho

    while (!feof(fp))
    {
        tmp = readLine(fp);

        if ((*tmp) == 0)
        {
            free(tmp);
            continue;
        }

        tmpUser = parseLine(tmp);

        if ((*nUsers) % USERS_BUFFER == 0)
        {
            users = (user **)realloc(users, ((*nUsers) / USERS_BUFFER + 1) * USERS_BUFFER * sizeof(user *));
        }
        users[(*nUsers)++] = tmpUser;

        free(tmp);
    }

    users = (user **)realloc(users, ((*nUsers)) * sizeof(user *));

    fclose(fp);

    return users;
}

char *readLine(FILE *stream)
{ // Lê strings
    char *string = NULL;
    int size = 0;
    char c;

    fscanf(stream, " ");

    do
    {
        fscanf(stream, "%c", &c);

        if ((c == '\n' && size == 0) || c == 13)
        {
            c = 0;
        }
        else
        {
            if (size % READLINE_BUFFER == 0)
            {
                string = (char *)realloc(string, (size / READLINE_BUFFER + 1) * READLINE_BUFFER * sizeof(char));
            }
            string[size++] = c;
        }

    } while (c != '\n' && !feof(stream));

    string[--size] = 0;

    string = (char *)realloc(string, (size + 1) * sizeof(char));

    return string;
}