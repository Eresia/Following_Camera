#ifndef INFORMATION
#define INFORMATION

#define TOLERANCE_CENTRE 10

typedef struct Info Info;
struct Info {
	int* sizeX;
	int* sizeY;
	int* x;
	int* y;
	int* isEnd;
	int* reset;
};

#endif
