#ifndef INFORMATION
#define INFORMATION

#define TOLERANCE_CENTRE 30

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
