#include <stdio.h>
#include <stdlib.h>

#include "user.h"
#include "util.h"

int main()
{
    int i;
    int nUsers = 0;
    char *filename = NULL;
    user **users = NULL;

    filename = readLine(stdin);

    users = parseCSVToUsers(filename, &nUsers);

    for (i = 0; i < nUsers; i++)
    {
        printIfOfAge(users[i]);
    }

    for (i = 0; i < nUsers; i++)
    {
        deleteUser(users[i]);
    }

    free(users);
    free(filename);

    return 0;
}