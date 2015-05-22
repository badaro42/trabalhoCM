#include "Auxiliar.h"

Auxiliar::Auxiliar()
{

}

float Auxiliar::calcColor(float green, float red, float blue){

	float max, min; 
	red /= 255;
	green /= 255;
	blue /= 255;
	float delta; 
	max = std::max(red, std::max(green, blue));
	min = std::min(red, std::min(green, blue));
	delta = max - min;

	float hue_temp, hue;
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
	return hue;
}

void Auxiliar::setPixels(unsigned char * p){
	pixels = p;
}

void Auxiliar::setWidth(int width){
	width = width;
}

void Auxiliar::setNrPixels(int nr_pixels){
	num_pixels = nr_pixels;
}

float Auxiliar::calculateContrast(int i){

			if(i > (width*3-1) && i < (num_pixels - width*3)){
			//tenho parte de cima e de baixo
			if(i % (int) width-3 != 0 && ((i % (int) width*3)+3 != 9)){

				// tenho esquerdo e direito
			}else if(i % (int) width-3 != 0 && !((i % (int) width*3)+3 != 9)){
				// tenho esquerdo
			}else if(!(i % (int) width-3 != 0) && (i % (int) width*3)+3 != 9){
				// tenho direito
			}
		}else if(!(i > (width*3-1)) && i < (num_pixels - width*3)){
			//tenho parte de cima e de baixo
			if(i % (int) width-3 != 0 && ((i % (int) width*3)+3 != 9)){

				// tenho esquerdo e direito
			}else if(i % (int) width-3 != 0 && !((i % (int) width*3)+3 != 9)){
				// tenho esquerdo
			}else if(!(i % (int) width-3 != 0) && (i % (int) width*3)+3 != 9){
				// tenho direito
			}
		}else if(i > (width*3-1) && !(i < (num_pixels - width*3))){
			//tenho parte de cima e de baixo
			if(i % (int) width-3 != 0 && ((i % (int) width*3)+3 != 9)){

				// tenho esquerdo e direito
			}else if(i % (int) width-3 != 0 && !((i % (int) width*3)+3 != 9)){
				// tenho esquerdo
			}else if(!(i % (int) width-3 != 0) && (i % (int) width*3)+3 != 9){
				// tenho direito
			}
		}
	return 0;
}