#include "Image.h"

#include <stdio.h>
#include <tchar.h>
#include <limits>
 
// OpenCV
#include "cv.h"
#include "highgui.h"
 
#define _USE_MATH_DEFINES
#include <math.h>


Image::Image(unsigned char * pix_color, unsigned char * pix_gray, int t_width, int t_height)
{
	pixels_color = pix_color;
	pixels_gray = pix_gray;
	width = t_width;
	height = t_height;
	size = t_width*t_height;
	nr_edges = 0;
	
	for(int i = 0; i < 361; i++)
		hue_vector[i] = 0;
}


//************************** DOMINANT COLOR ***********************************

float Image::calculateLuminance(int i, int j) {
	int pos = i*width+j;
	int r = pixels_color[pos];
    int g = pixels_color[pos+1];
    int b = pixels_color[pos+2];

    return 0.213*r+0.715*g+0.072*b;
}

float Image::calcColor(float red, float green, float blue) {
	float max, min, delta, hue_temp, hue;

	red /= 255;
	green /= 255;
	blue /= 255;
	max = std::max(red, std::max(green, blue));
	min = std::min(red, std::min(green, blue));
	delta = max - min;

	if (red == max) {
		hue_temp = (green - blue) / (delta + 1e-20f);
		if(hue_temp < 0.0)
			hue_temp = hue_temp + 6.0;
	}
	else if (green == max)
		hue_temp = ((blue - red) / (delta + 1e-20f)) + 2;
	else
		hue_temp = ((red - green) / (delta + 1e-20f)) + 4;
	hue = hue_temp * 60;

	int add_hue = int(hue);
	hue_vector[add_hue]++;
	
	return hue;
}

float Image::calcColorAux(int i, int j){
	int pos = i*width+j; 

	float red = pixels_color[pos];
	float green = pixels_color[pos+1];
	float blue = pixels_color[pos+2];
	
	return calcColor(red, green, blue);
}


//************************** CONTRASTE CONTRASTE ***********************************

float Image::calculateContrast(int i, int j){
	double topo_esquerdo = getPixel(i-1,j-1);
	double topo_direito = getPixel(i-1,j+1);
	double topo = getPixel(i-1,j);
	double esquerdo = getPixel(i,j-1);
	double direito = getPixel(i,j+1);
	double baixo_esquerdo = getPixel(i+1,j-1);
	double baixo_direito = getPixel(i+1,j+1);
	double baixo = getPixel(i+1,j);
	double middle_pixel = getPixel(i,j);
	double mean_luminance_adjacentes;

	mean_luminance_adjacentes = 
		(baixo + topo + topo_esquerdo + baixo_esquerdo + 
		esquerdo + direito + topo_direito + baixo_direito)/8;

	// para evitar divisões por zero (ou valores proximos disso)
	if(mean_luminance_adjacentes >= 0 && mean_luminance_adjacentes < 1)
		return mean_luminance_adjacentes;

	return (std::abs(middle_pixel-mean_luminance_adjacentes))/std::abs(mean_luminance_adjacentes); 
}


//********************* IMAGE MATCHING (SIFT/SURF) *****************************

int Image::match(ofImage img){

	bool result = false;
	float neares_neighbor_distance_ratio = 0.7f;

	cv::SurfFeatureDetector detector(400);
	vector<cv::KeyPoint> keypoints1, keypoints2;
	cv::SurfDescriptorExtractor extractor;
	cv::Mat descriptors1, descriptors2;
	
	cv::Mat img1(height, width, CV_8UC3, pixels_color);
	cv::Mat img2(img.getHeight(), img.getWidth(), CV_8UC3, img.getPixels());

	detector.detect(img1, keypoints1);
	detector.detect(img2, keypoints2);
	
	extractor.compute(img1, keypoints1, descriptors1);
	extractor.compute(img2, keypoints2, descriptors2);

	cv::FlannBasedMatcher matcher;
	vector<vector<cv::DMatch>> matches;

	//cv::BruteForceMatcher<cv::L2<float>> matcher;
	//vector<cv::DMatch> matches;
	matcher.knnMatch(descriptors1, descriptors2, matches, 2);

	vector<cv::DMatch> good_matches;
	good_matches.reserve(matches.size());  

	for(size_t i = 0; i < matches.size(); ++i)
	{ 
		if(matches[i].size() < 2)
			continue;
     
		const cv::DMatch &m1 = matches[i][0];
		const cv::DMatch &m2 = matches[i][1];
     
		if(m1.distance <= neares_neighbor_distance_ratio * m2.distance)        
			good_matches.push_back(m1);     
	}

	return good_matches.size();
}

//************************** EDGES EDGES EDGES ***********************************

int Image::getEdges(int i, int j, int type){
	int val = applyFilter(i, j, type);

	//branco ou proximo
	if(val >= 240){
		nr_edges++;
		return 1;
	}
	return 0;
}

//************************** PESSOAS NUMA FRAME ***********************************

int Image::peopleOnFrame(ofPixelsRef pixels) {
	ofxCvHaarFinder haarFinder; 
	haarFinder.setup("HaarFinder/haarcascade_frontalface_default.xml");
	return haarFinder.findHaarObjects(pixels);
}

//************************** GABOR GABOR GABOR ***********************************

cv::Mat mkKernel(int kernel_size, double sigma, double theta, double lambda, double psi) {
    
	int half_ks = (kernel_size-1)/2;
	double theta_aux = theta*CV_PI/180;
	double psi_aux = psi*CV_PI/180;
	double delta = 2.0/(kernel_size-1);
	double lambda_aux = lambda;
	double sigma_aux = sigma/kernel_size;
	double x_theta;
	double y_theta;
	
	cv::Mat kernel(kernel_size, kernel_size, CV_32F);
	for (int y=-half_ks; y<=half_ks; y++) {
        for (int x=-half_ks; x<=half_ks; x++) {
			x_theta = x*delta*cos(theta_aux) + y*delta*sin(theta_aux);
			y_theta = -x*delta*sin(theta_aux) + y*delta*cos(theta_aux);
			kernel.at<float>(half_ks+y, half_ks+x) = 
				(float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma_aux,2)) * 
				cos(2*CV_PI*x_theta/lambda_aux + psi_aux);
		}
    }
    return kernel;
}
 
double Image::calculateTexture(){
    
	int kernel_size=21;
	int pos_lm = 50;
	double sig = 5;
	double lm = 0.5+pos_lm/100.0;
	double ps = 90;
	double th = 0;

	cv::Mat dest;
	cv::Mat src(getHeight(), getWidth(), CV_8UC3, pixels_color);
	cvtColor(src, dest, CV_RGB2GRAY);

	double num = 0;
    double num_total = 0;
    for(; th <= 135; th += 45){
        cv::Mat kernel = mkKernel(kernel_size, sig, th, lm, ps);
        cv::filter2D(src, dest, CV_32F, kernel);
        
	    ofImage imageOut = ofImage();
	    imageOut.setFromPixels((unsigned char *) IplImage(dest).imageData, dest.size().width, dest.size().height, OF_IMAGE_GRAYSCALE);
	        
	    // contar numero de pixels acima de threshold - 240
	    int x, y; 
		int width = imageOut.width;
		int heigth = imageOut.height;
	
	    unsigned char* pixels = imageOut.getPixels();
	    num = 0;
	    for (x = 0; x < width; x++){
			for(y = 0; y < heigth; y++){
				if(pixels[(y*width+x)] >= 240)
					num++;
			}
		}
	    num /= (width*height);
	    num_total += num;
    }
    // 4 imagens geradas
    return num_total/4;
}


//************************** METODOS AUXILIARES ***********************************

void Image::setWidth(int width1){
	width = width1;
}

void Image::setHeight(int height1){
	height = height1;
}

int Image::getHeight() {
	return height;
}

int Image::getWidth() {
	return width;
}

double Image::getPixel(int i, int j) {
	if(i < 0 && j < 0) //o primeiro pixel!
		return pixels_gray[0];
	if(j < 0) // nao temos esquerdo
		return pixels_gray[i*width];
	else if(j > width-1) // nao temos direito
		return pixels_gray[((i+1)*width)-1];
	else if(i < 0) // nao temos topo
		return pixels_gray[j];
	else if(i > height-1) // nao temos baixo
		return pixels_gray[((height-1)*width)+j];
	else
		return pixels_gray[(i*width)+j];
}

//type == 1: EDGES TODAS AS DIREÇOES;
//type == 2: EDGES VERTICAIS;
//type == 3: EDGES HORIZONTAIS;
std::vector<int> Image::getVector(int type) {
	if(type == 1) {
		static const int vec[] = {-1,-1,-1,-1,8,-1,-1,-1,-1};
		std::vector<int> v(vec, vec+sizeof(vec)/sizeof(vec[0]));
		return v;
	}
	else if(type == 2){
		static const int vec[] = {-1,0,1,-1,0,1,-1,0,1};
		std::vector<int> v(vec, vec+sizeof(vec)/sizeof(vec[0]));
		return v;
	}
	else if(type == 3){
		static const int vec[] = {-1,-1,-1,0,0,0,1,1,1};
		std::vector<int> v(vec, vec+sizeof(vec)/sizeof(vec[0]));
		return v;
	}
}

int Image::applyFilter(int i, int j, int type){

	std::vector<int> edges = getVector(type);

	double topo_esquerdo = getPixel(i-1,j-1);
	double topo_direito = getPixel(i-1,j+1);
	double topo = getPixel(i-1,j);
	double esquerdo = getPixel(i,j-1);
	double direito = getPixel(i,j+1);
	double baixo_esquerdo = getPixel(i+1,j-1);
	double baixo_direito = getPixel(i+1,j+1);
	double baixo = getPixel(i+1,j);
	double middle_pixel = getPixel(i,j);

	return (topo_esquerdo*edges[0] + topo*edges[1] + topo_direito*edges[2] + esquerdo*edges[3] + middle_pixel*edges[4]
		+ direito*edges[5] + baixo_esquerdo*edges[6] + baixo*edges[7] + baixo_direito*edges[8]);
}

double Image::calculateQuality(){

	int i;
	int count = 0; //nr de cores diferentes
	for(i = 0; i < 361; i++) {
		if(hue_vector[i] > 0)
			count++;
	}
	double temp_edges = nr_edges;
	temp_edges /= width*height;
	double colors = count/360;

	return colors*0.5 + temp_edges*0.5;
}

