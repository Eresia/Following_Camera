// Ligne de compilation : gcc -o cam testCam.c `pkg-config --cflags opencv --libs opencv` -ldl -lm
#include <pthread.h>
#include "Information.h"
#include "Picture.h"
#include "Follow.h"

int main(int argc, char** argv){

	Info* info = malloc(sizeof(Info));
	pthread_t picture, follow;

	info->x = malloc(sizeof(int));
	info->y = malloc(sizeof(int));
	info->isEnd = calloc(1, sizeof(int));
	info->reset = calloc(1, sizeof(int));
	info->sizeX = NULL;
	info->sizeY = NULL;

	pthread_create(&picture, NULL, launch_picture, info);
	//pthread_create(&follow, NULL, launch_follow, info);

	pthread_join(picture, NULL);
	//pthread_join(follow, NULL);

	return 0;
}
