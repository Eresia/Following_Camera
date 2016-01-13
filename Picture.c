#include "Picture.h"

// Fonction lié au clic. Renvoi la position du pixel et sa couleur.
void mouseEvent(int evt, int x, int y, int flags, void* param) {
	Color_pixel* color = (Color_pixel*) param ;
	int i = 0 ;

	// On récupére la couleur du pixel sur lequel on clique.
	if(evt==CV_EVENT_LBUTTONDOWN) {
		color->r = color->cam->imageData[((x*3)+(y*color->cam->widthStep))+2] ;
		color->g = color->cam->imageData[((x*3)+(y*color->cam->widthStep))+1] ;
		color->b = color->cam->imageData[((x*3)+(y*color->cam->widthStep))] ;
	}
}

int colorTracking (IplImage* cap, Color_pixel color, int i, uchar pixel_blue, uchar pixel_green, uchar pixel_red) {
	// Etudier la couleur pour adapter la tolerence en fonction de R, G ou B.
	/*int toleranceR = 15 ;
	int toleranceG = 26 ;
	int toleranceB = 40 ;*/

	// Si les pixels sont de la couleur on les met en rouge.
	if ((pixel_blue >= (color.b)-TOLERANCE_B) && (pixel_blue <= (color.b)+TOLERANCE_B) && (pixel_green >= (color.g)-TOLERANCE_G)
			&& (pixel_green <= (color.g)+TOLERANCE_G) && (pixel_red >= (color.r)-TOLERANCE_R)	&& (pixel_red <= (color.r)+TOLERANCE_R)){

		cap->imageData[i] = 0 ;
		cap->imageData[i+1] = 0 ;
		cap->imageData[i+2] = 255 ;

		return 1 ;
	}

	return 0 ;
}

Barycenter barycenterCalculation (int *barycenter_x, int *barycenter_y, int size_x, int size_y) {
	Barycenter coordonnees ;
	coordonnees.x = coordonnees.y = 0 ;
	//int value_max = 0 ;
	int i, j = 0 ;
	int nombre_x = 0 ;
	int nombre_y = 0 ;
	int compte_x = 0 ;
	int compte_y = 0 ;

	for (i = 0 ; i < size_x ; i++){
		nombre_x = nombre_x + (i*barycenter_x[i]);
		compte_x = compte_x + barycenter_x[i];
	}

	for (i = 0 ; i < size_y ; i++){
		nombre_y = nombre_y + (i*barycenter_y[i]);
		compte_y = compte_y + barycenter_y[i];
	}

	if ((compte_x != 0) && (compte_y != 0)){
		coordonnees.x = nombre_x/compte_x;
		coordonnees.y = nombre_y/compte_y ;
	}
	else{
		coordonnees.x = size_x/2;
		coordonnees.y = size_y/2;
	}

	return coordonnees ;
}

void* launch_picture(void* info_void) {
	Info* info = (Info*) info_void;
	int nbImage = 0 ;
	float exe_time = 0 ;
	clock_t t1, t2, time_test ;
	t1 = clock() ;
	CvPoint center;


	Color_pixel color ;
	color.r = 0 ;
	color.g = 255 ;
	color.b = 0 ;

	Barycenter barycentre_coordonnees ;
	barycentre_coordonnees.x = barycentre_coordonnees.y = 0 ;

	int *barycenter_x ;
	int *barycenter_y ;

	// Déclaration d'un caractère pour la fermeture des fenêtres
	char key;
	int i, j = 0 ;

	// Déclaration des images
	IplImage* cap ;

	// Déclaration de l'élément de capture à partir de la webcam
	CvCapture *capture ;

	// Déclaration des différents noms de fenêtres
	const char* window_title = "Original Camera" ;
	const char* window_hsv = "Hsv Camera" ;

	// On choisis la source pour l'image (webcam).
	capture = cvCreateCameraCapture (1);

	// Si la capture d'image échoue, on arrête le programme avec un message.
	if (!capture) {
		printf("Ouverture du flux video impossible !\n") ;
		*info->isEnd = 1;
		pthread_exit(NULL);
	}

	// Création de la fenêtre.
	cvNamedWindow (window_title, CV_WINDOW_AUTOSIZE);

	printf("\n") ;

	uchar pixel_blue ;
	uchar pixel_green ;
	uchar pixel_red ;

	// On alloue la mémoire pour nos tableaux de calcul du barycentre
	cap = cvQueryFrame(capture);
	int sizeXCam = cap->width;
	int sizeYCam = cap->height;
	info->sizeX = &sizeXCam;
	info->sizeY = &sizeYCam;
	center.x = *info->sizeX/2;
	center.y = *info->sizeY/2;

	/*barycenter_x = calloc(cap->width, sizeof(int));
	barycenter_y = calloc(cap->height, sizeof(int));*/


	// Tant qu'on appuie pas sur q le on continu la boucle.
	while ((key != 'q') && (key != 'Q') && (*info->isEnd == 0)){
		if(key == 'r'){
			color.r = 0 ;
			color.g = 255 ;
			color.b = 0 ;
			*info->reset = 1;
		}
		barycenter_x = calloc(cap->width, sizeof(int));
		barycenter_y = calloc(cap->height, sizeof(int));


		// On met la capture de la webcam dans l'attribut cap.
		cap = cvQueryFrame(capture);

		// Si on traite en BGR
		color.cam = cap ;


		// On parcours notre image (les pixels).
		for (i = 0 ; i < (cap->width) ; i++) {
			for (j = 0 ; j < cap->height ; j++) {

				// Pixels B,G,R en fonction de la position.
				pixel_blue = (uchar)(cap->imageData[((i*3)+(j*cap->widthStep))]) ;
				pixel_green = (uchar)(cap->imageData[((i*3)+(j*cap->widthStep))+1]) ;
				pixel_red = (uchar)(cap->imageData[((i*3)+(j*cap->widthStep))+2]) ;

				// Tracking des pixels en fonction de la couleur.
				if(colorTracking(cap, color, ((i*3)+(j*cap->widthStep)), pixel_blue, pixel_green, pixel_red)){
					//printf("y = %d, x = %d\n", i/((cap->width)*3), i%((cap->width)*3));
					//printf("x = %d, y = %d\n", i/3, j);
					barycenter_y[j]++ ;

					barycenter_x[i]++ ;
				}
			}
		}

		// On calcul le barycentre.
		barycentre_coordonnees = barycenterCalculation (barycenter_x, barycenter_y, cap->width, cap->height) ;

		/*printf("barycentre x main = %d, barycentre y main = %d\n", barycentre_coordonnees.x,
		barycentre_coordonnees.y);*/
		CvPoint p;
		printf("Barycentre update\n");
		p.x = barycentre_coordonnees.x ;
		p.y = barycentre_coordonnees.y ;
		*info->x = barycentre_coordonnees.x;
		*info->y = barycentre_coordonnees.y;

		cvCircle(cap, p, 5, CV_RGB(0,0,255), -1, 8, 0);
		cvCircle(cap, center, TOLERANCE_CENTRE, CV_RGB(0,255,0), -1, 8, 0);

		// On affiche la webcam normalement.
		cvShowImage(window_title, cap);
		cvSetMouseCallback(window_title, mouseEvent, &color);

		// On réinitialise les barycentres
		free(barycenter_x);
		free(barycenter_y);
		key = cvWaitKey(10);
	}



	cvReleaseCapture(&capture);
	cvDestroyWindow(window_title);

	t2 = clock() ;
	exe_time = (float)t2-t1/CLOCKS_PER_SEC;
	*info->isEnd = 1;
	pthread_exit(NULL);
}