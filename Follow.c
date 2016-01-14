#include "Follow.h"

int open_s(Serial_com* sc, char *name){

	struct termios toptions;

	strcpy(sc->name, name) ;


	#ifdef __DEBUG
	printf("Ouverture du port : %s\n", sc->name) ;
	#endif

	sc->fd = open(sc->name, O_RDWR) ;
	if(sc->fd == -1){
		return -1;
	}

	#ifdef __DEBUG
	printf("wait\n") ;
	#endif


	#ifdef __DEBUG
	printf("Ok\n") ;
	#endif

	tcgetattr(sc->fd, &toptions) ;

	cfsetispeed(&toptions, B9600) ;
	cfsetospeed(&toptions, B9600) ;
	toptions.c_cflag &= ~PARENB ;
	toptions.c_cflag &= ~CSTOPB ;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8 ;

	toptions.c_cflag &= ~CRTSCTS;

	tcsetattr(sc->fd, TCSANOW, &toptions) ;

	usleep(3000000);

	return 1;
}

int write_s(Serial_com* sc, char *buffer, int nbyte){
	return write(sc->fd, buffer, nbyte);
}

int read_s(Serial_com* sc){
	char* buffer = malloc(sizeof(char));
	return read(sc->fd, buffer,1);
}

int close_s(Serial_com* sc){
	return close(sc->fd);
}

void writeOnFile(FILE * fichier, char* texte){
	fputs(texte, fichier);
}

void send_instruction(int taille_ecran_x, int taille_ecran_y, Serial_com* sc, int x, int y)
{

	if((x != -1) || (y != -1)){

		#ifdef DEBUG
		printf("-----------debut message----------\n");
		#endif

		int centre_x=taille_ecran_x/2;
		int centre_y=taille_ecran_y/2;

		int delta_x;
		int delta_y;
		int delta, deltaMax;
		//int absx;
		//int absy;
		//recupération de  la position pas encore fait


		//déclaration des instructions
		char angle_x;
		char angle_y;


		//calcul du delta
		delta_x = centre_x - x;
		delta_y = centre_y - y;

		delta = (sqrt((delta_x * delta_x) + (delta_y * delta_y)));
		deltaMax = (sqrt((centre_x * centre_x) + (centre_y * centre_y)));

		//printf("delta : %d\n", delta);


		//calcul de la commande à envoyer
		if(delta_x > TOLERANCE_CENTRE){
			angle_x = '+';
		}
		else if(delta_x < -TOLERANCE_CENTRE){
			angle_x = '-';
		}
		else{
			angle_x = '0';
		}

		if(delta_y > TOLERANCE_CENTRE){
			angle_y = 'p';
		}
		else if(delta_y < -TOLERANCE_CENTRE){
			angle_y = 'm';
		}
		else{
			angle_y = 'n';
		}

		if((angle_x != '0') || (angle_y != 'n')){

			//envoie de la commande au servo moteur
			if(write_s(sc, &angle_x, 1)!=-1){
				#ifdef DEBUG
					printf("signal envoyé : instruction pan\n");
				#endif
			}

			else{
				#ifdef DEBUG
					printf("erreur port série\n");
				#endif
			}

			if(write_s(sc, &angle_y, 1)!=-1){
				#ifdef DEBUG
					printf("signal envoyé : instruction tilt\n");
				#endif
			}

			else{
				#ifdef DEBUG
					printf("erreur port série\n");
				#endif
			}

			#ifdef DEBUG
				printf("------------fin message-------------\n");
			#endif
			usleep(((deltaMax - delta) * 100));//pid : delay de signal
			//printf("Delay : %d\n", ((deltaMax - delta) * 10));
			//printf("delta : %d\n", delta);
		}
		else{
			usleep(25000);
		}
	}
	else{
		usleep(25000);
	}
}

void reset(Serial_com* sc)
{

	//envoie de la commande au servo moteur
	if(write_s(sc, "r", 1)!=-1){
		#ifdef DEBUG
		printf("signal envoyé : instruction pan\n");
		#endif
	}
}

int calculdelta(int x, int y, int sizeXScreen, int sizeYScreen){

	int centre_x=sizeXScreen/2;
	int centre_y=sizeYScreen/2;
	//calcul du delta
	int delta_x = x - centre_x;
	int delta_y = y - centre_y;


	int delta = (int)sqrt((delta_x * delta_x) + (delta_y * delta_y));

	return delta;
}



void writeInCsv(int* tabWr, int* tabWu,FILE *f){
	int i=0;
	for(i=0; i < SIZE_MAX_CSV; i++){
		if(tabWr[i]!=-1){
			char col1[10];
			char col2[10];
			char col3[10];

			sprintf(col1, "%d", i);
			sprintf(col2, "%d", tabWr[i]);
			sprintf(col3, "%d", tabWu[i]);
			strcat(col1,",");
			strcat(col1,col2);
			strcat(col1, " ");
			strcat(col1,col3);

			strcat(col1," \n");
			printf("%s nn", col1);

			//strcat(&ligne," 	");
			//valeur=sprintf(&valeur, "%d", tabWr[i]);
			//strcat(&ligne, &valeur);
			//printf("%s", ligne);

			fputs(col1, f);

		}
	}
}

void* launch_follow(void* info_void){

	Info* info = (Info*) info_void;

	Serial_com test;
	char name[] = "test.csv";
	clock_t temps_start, temps;
	double actu = 0;
	int* tabWr = malloc(SIZE_MAX_CSV*sizeof(int));
	int* tabWu = malloc(SIZE_MAX_CSV*sizeof(int));
	int i;
	int timerBegin = 0;

	if(open_s(&test, ACM)!=-1){
		#ifdef DEBUG
		printf("connecté sur le port série\n");
		#endif
	}

	else{
		printf("problème de connection\n");
		pthread_exit(NULL);
	}

	while((*info->isEnd == 0) && (info->sizeX == NULL || info->sizeY == NULL)){
		usleep(25000);
	}

	#ifdef DEBUG
		FILE * f;
		//scanf("%s", name);
		//strcat(name, ".csv");
		printf("ecriture dans ... %s \n", name);

		if((f=fopen(name, "w+"))!=NULL)
		printf("ouverture fichier ok...");
		else
		printf("erreur fichier");

		for(i=0; i < SIZE_MAX_CSV; i++){
			tabWr[i] = -1;
			tabWu[i] = -1;
		}

	#endif

	reset(&test);
	while(*info->isEnd == 0)
	{
		if(*info->reset){
			reset(&test);
			*info->reset = 0;
		}
		else{
			int newX = *info->x;
			int newY = *info->y;
			/**info->x = -1;
			*info->y = -1;*/
			send_instruction(*info->sizeX, *info->sizeY, &test, newX, newY);
			#ifdef DEBUG

				if(timerBegin == 1 || ((*info->x != (*info->sizeX / 2)) && (*info->y != (*info->sizeY / 2)))){
					timerBegin = 1;
					temps =clock() - temps_start;
					actu = (temps / (double) CLOCKS_PER_SEC) * 100;
					printf("temps actuel : %lf \n", (double) actu);

					tabWr[(int) actu] = *info->x;
					tabWu[(int) actu] = *info->y;
					printf("ajout de %d a %d dizaine de milisecondes \n", tabWr[(int) actu], (int) actu);
					if(actu >= SIZE_MAX_CSV){
						*info->isEnd = 1;
					}
				}
				else{
					temps_start = clock();
				}
			#endif
		}

	}
	#ifdef DEBUG
		writeInCsv(tabWr,tabWu,f);
	#endif
	reset(&test);

	//fclose(f);
	close_s(&test) ;

	pthread_exit(NULL);
}
