#ifndef _USER_H_
#define _USER_H_

typedef struct user user;

user *newUser(int id, char *name, int age);

void deleteUser(user *u);

void printIfOfAge(user *u);

#endif