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
	unsigned char * pixels;
	int height;
	int width;  
	int size;
	ofImage obj;
	int nr_edges;

public:
	Image(unsigned char * pix, int t_width, int t_height);	
	void setPixels(unsigned char * p);
	void setWidth(int width);
	void setHeight(int height);
	void setObject(string path);
	int findObject(string path);
	int getWidth();
	int getHeight();
	double getPixel(int x, int y);
	int applyFilter(int i, int j, int type);
	std::vector<int> getVector(int type);
	float calcColor(float green, float red, float blue);
	double calculateContrast(int x, int y); 
	void setobj(string path);
	ofImage getObj();
	int match(ofImage img); 
	int getEdges(int i, int j);
};

#endif