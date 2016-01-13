#ifndef FOLLOW
#define FOLLOW

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>

#include "Information.h"

#define ACM "/dev/ttyACM0"
#define SIZE_MAX_CSV 1000

struct termios toptions;

typedef struct Serial_com Serial_com;
struct Serial_com{
	char name[20];
	int speed ;
	int fd ;
	char parity ;
	int stopBit ;
};

int open_s(Serial_com* sc, char *name);
int write_s(Serial_com* sc, char *buffer, int nbyte);
int close_s(Serial_com* sc);
void writeOnFile(FILE * fichier, char* texte);
void send_instruction(int taille_ecran_x, int taille_ecran_y, Serial_com* sc, int x, int y);
void reset(Serial_com* sc);
int calculDelta(int x, int y, int sizeXScreen, int sizeYScreen);
void writeInCsv(int* tabWr, int* tabWu,FILE *f);
void* launch_follow(void* info_void);

#endif
