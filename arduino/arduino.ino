#include <Servo.h>

#define PAS 2


Servo myservo;
Servo myservo1;

int read;
int posx;
int posy;
int incx=0;
int incy=0;

char datax[1];
char datay[3];

void setup() {

	Serial.begin(9600);

	read=0;
	posx=90;

	posy=95;

	/*************initialisation des servo_moteurs**********/
	myservo.attach(6);//servo_moteur pan
	myservo1.attach(9);//servo_moteur tilt

	/************position initiale****************/



	myservo.write(posx);
	myservo1.write(posy);

	//  Serial.write("-80");
	//Serial.write("-20");


	/*********************************************/

}

void loop() {
	int i=0;

	datax[0] = '\0';

	int cont=0;
	if(Serial.available()){
		while(Serial.available())
		{
			datax[cont]=Serial.read();

			//   Serial.println(posx);

			// datax[0]=NULL;
			//datay[0]=Serial.read();



			//posy = posy + atoi(datay);

			//myservo.write(posy);

			//

			cont++;

		}

		if(datax[0] == 'r'){
			incx = 0;
			incy = 0;
			posx = 90;
			posy = 95;
		}
		else{


			if(datax[0] == '-'){
				//posx = posx +1;
				incx=PAS;
				Serial.println("r");

			}

			else if (datax[0] == '+'){

				//posx = posx -1;
				incx=-PAS;
				Serial.println("r");


			}
			else{
				incx = 0;
			}

			if(datax[0] == 'm'){
				//posy = posy +1;
				incy=PAS;
				Serial.println("r");

			}

			else if(datax[0] == 'p'){
				//posy = posy -1;
				incy=-PAS;
				Serial.println("r");

			}
			else{
				incy = 0;
			}


			cont =0;
		}


		posx=posx+incx;
		posy=posy+incy;
	}

	myservo1.write(posx);
	myservo.write(posy);

	//delay(25);

}

