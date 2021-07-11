/*
Autores: 
-Eduardo Amaral - NUSP 11735021
-Vitor Beneti Martins - NUSP 11877635
*/
#ifndef _VEHICLE_UTIL_H_
#define _VEHICLE_UTIL_H_

#include <stdint.h>
#define VEHICLE_BUFFER 127

#include "util.h"
#include "vehicle.h"

void printDate(char *data);

void parseCSVToVehicles(vehicleFile *vf, char *filename);

boolean checkPrefix(char *src, FILE *fp, int *read);

boolean checkDate(char *src, FILE *fp, int *read);

boolean checkPlaces(int32_t src, FILE *fp, int *read);

boolean checkModel(char *src, FILE *fp, int *read);

boolean checkCategory(char *src, FILE *fp, int *read);

#endif
