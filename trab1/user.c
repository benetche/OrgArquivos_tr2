#include <stdlib.h>
#include <stdio.h>

#include "user.h"

struct user
{
    int id;
    char *name;
    int age;
};

user *newUser(int id, char *name, int age)
{
    user *u = (user *)malloc(sizeof(user));
    u->id = id;
    u->name = name;
    u->age = age;
    return u;
}

void deleteUser(user *u)
{
    free(u->name);
    free(u);
}

void printIfOfAge(user *u)
{
    if (u->age >= 18)
    {
        if (u->name)
        {
            printf("O usuario %s eh de maior.\n", u->name);
        }
        else
        {
            printf("O usuario de identificacao %d eh de maior.\n", u->id);
        }
    }
    return;
}