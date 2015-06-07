#ifndef _IMAGE
#define _IMAGE

#include "ofxOpenCv.h"
#include <opencv2/opencv.hpp>
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxSwipeable.h"
#include "ofxCvHaarFinder.h"
#include <stdio.h>
#include "Image.h"

class Image
{
private:
	unsigned char * pixels_color;
	unsigned char * pixels_gray;
	int height;
	int width;  
	int size;
	int nr_edges;
	int hue_vector[361];

public:

	Image(unsigned char * pix_color, unsigned char * pix_gray, int t_width, int t_height);	
	void setPixels(unsigned char * p);
	void setWidth(int width);
	void setHeight(int height);
	int getWidth();
	int getHeight();
	double getPixel(int x, int y);
	int applyFilter(int i, int j, int type);
	std::vector<int> getVector(int type);
	float calcColorAux(int i, int j);
	float calcColor(float red, float green, float blue);
	float calculateContrast(int x, int y); 
	ofImage getObj();
	int match(ofImage img); 
	int getEdges(int i, int j, int type);
	double calculateTexture();
	float calculateLuminance(int i, int j);
	double calculateQuality();
	int peopleOnFrame(ofPixelsRef pixels);
};

#endif