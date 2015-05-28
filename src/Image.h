#ifndef _IMAGE
#define _IMAGE

#include <stdio.h>
#include "ofxUI.h"
#include <math.h>

class Image
{
private:
	unsigned char * pixels;
	int height;
	int width;  
	int size;
public:
	Image(unsigned char * pix, int t_width, int t_height);	
	void setPixels(unsigned char * p);
	void setWidth(int width);
	void setHeight(int height);
	int getWidth();
	int getHeight();
	double getPixel(int x, int y);
	int getEdges(int i, int j);

	float calcColor(float green, float red, float blue);
	double calculateContrast(int x, int y); 
};

#endif