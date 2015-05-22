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

	float red_topo_esquerdo, blue_topo_esquerdo, green_topo_esquerdo;
	float red_topo_direito, blue_topo_direito, green_topo_direito;
	float red_topo, blue_topo, green_topo;
	float red_esquerdo, blue_esquerdo, green_esquerdo;
	float red_direito, blue_direito, green_direito;
	float red_baixo_esquerdo, blue_baixo_esquerdo, green_baixo_esquerdo;
	float red_baixo_direito, blue_baixo_direito, green_baixo_direito;
	float red_baixo, blue_baixo, green_baixo;
	float red, blue, green;
	float mean_luminance_pixel;
	float mean_luminance_adjances;

	red = pixels[i];
	green = pixels[i+1];
	blue = pixels[i+2];
	mean_luminance_pixel = 0.2125*red + 0.7154*green + 0.0721*blue;

	if(i > (width*3-1) && i < (num_pixels - width*3)){
		//tenho parte de cima e de baixo
		red_baixo = pixels[i+3*width];
		green_baixo = pixels[i+3*width+1];
		blue_baixo = pixels[i+3*width+2];

		red_topo = pixels[i-3*width];
		green_topo = pixels[i-3*width+1];
		blue_topo = pixels[i-3*width+2];

		if(i % (int) width-3 != 0 && ((i % (int) width*3)+3 != 9)){
			// tenho esquerdo e direito
			red_topo_esquerdo = pixels[i-3*width-3];
			blue_topo_esquerdo = pixels[i-3*width-3+1];
			green_topo_esquerdo = pixels[i-3*width-3+2];

			red_baixo_esquerdo = pixels[i+3*width-3];
			blue_baixo_esquerdo = pixels[i+3*width-3+1];
			green_baixo_esquerdo = pixels[i+3*width-3+2];

			red_esquerdo = pixels[i-3];
			blue_esquerdo = pixels[i-3+1];
			green_esquerdo = pixels[i-3+2];

			red_direito = pixels[i+3];
			blue_direito = pixels[i+3+1];
			green_direito = pixels[i+3+2];
		
			red_topo_direito = pixels[i-3*width+3];
			blue_topo_direito = pixels[i-3*width+3+1];
			green_topo_direito = pixels[i-3*width+3+2];

			red_baixo_direito = pixels[i+3*width+3];
			blue_baixo_direito = pixels[i+3*width+3+1];
			green_baixo_direito = pixels[i+3*width+3+2];

			mean_luminance_adjances = (0.2125*red_baixo + 0.7154*red_baixo + 0.0721*blue_baixo
				+ 0.2125*red_topo + 0.7154*green_topo + 0.0721*blue_topo
				+ 0.2125*red_topo_esquerdo + 0.7154*green_topo_esquerdo + 0.0721*blue_topo_esquerdo
				+ 0.2125*red_baixo_esquerdo + 0.7154*green_baixo_esquerdo + 0.0721*blue_baixo_esquerdo
				+ 0.2125*red_topo_direito + 0.7154*green_topo_direito + 0.0721*blue_topo_direito
				+ 0.2125*red_baixo_direito + 0.7154*green_baixo_direito + 0.0721*blue_baixo_direito
				+ 0.2125*red_direito + 0.7154*green_direito + 0.0721*blue_direito
				+ 0.2125*red_esquerdo + 0.7154*green_esquerdo + 0.0721*blue_esquerdo
				)/8;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 

		}else if(i % (int) width-3 != 0 && !((i % (int) width*3)+3 != 9)){
			// tenho esquerdo
			red_topo_esquerdo = pixels[i-3*width-3];
			blue_topo_esquerdo = pixels[i-3*width-3+1];
			green_topo_esquerdo = pixels[i-3*width-3+2];

			red_baixo_esquerdo = pixels[i+3*width-3];
			blue_baixo_esquerdo = pixels[i+3*width-3+1];
			green_baixo_esquerdo = pixels[i+3*width-3+2];

			red_esquerdo = pixels[i-3];
			blue_esquerdo = pixels[i-3+1];
			green_esquerdo = pixels[i-3+2];

			mean_luminance_adjances = (0.2125*red_baixo + 0.7154*red_baixo + 0.0721*blue_baixo
				+ 0.2125*red_topo + 0.7154*green_topo + 0.0721*blue_topo
				+ 0.2125*red_topo_esquerdo + 0.7154*green_topo_esquerdo + 0.0721*blue_topo_esquerdo
				+ 0.2125*red_baixo_esquerdo + 0.7154*green_baixo_esquerdo + 0.0721*blue_baixo_esquerdo
				+ 0.2125*red_esquerdo + 0.7154*green_esquerdo + 0.0721*blue_esquerdo)/5;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 

		}else if(!(i % (int) width-3 != 0) && (i % (int) width*3)+3 != 9){
			// tenho direito
			red_topo_direito = pixels[i-3*width+3];
			blue_topo_direito = pixels[i-3*width+3+1];
			green_topo_direito = pixels[i-3*width+3+2];

			red_baixo_direito = pixels[i+3*width+3];
			blue_baixo_direito = pixels[i+3*width+3+1];
			green_baixo_direito = pixels[i+3*width+3+2];

			red_direito = pixels[i+3];
			blue_direito = pixels[i+3+1];
			green_direito = pixels[i+3+2];

			mean_luminance_adjances = (0.2125*red_baixo + 0.7154*red_baixo + 0.0721*blue_baixo
				+ 0.2125*red_topo + 0.7154*green_topo + 0.0721*blue_topo
				+ 0.2125*red_topo_esquerdo + 0.7154*green_topo_esquerdo + 0.0721*blue_topo_esquerdo
				+ 0.2125*red_baixo_esquerdo + 0.7154*green_baixo_esquerdo + 0.0721*blue_baixo_esquerdo
				+ 0.2125*red_direito + 0.7154*green_direito + 0.0721*blue_direito)/5;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 
		}
	}else if(!(i > (width*3-1)) && i < (num_pixels - width*3)){
		//tenho parte de  baixo

		red_baixo = pixels[i+3*width];
		green_baixo = pixels[i+3*width+1];
		blue_baixo = pixels[i+3*width+2];

		if(i % (int) width-3 != 0 && ((i % (int) width*3)+3 != 9)){
			// tenho esquerdo e direito

			red_baixo_esquerdo = pixels[i+3*width-3];
			blue_baixo_esquerdo = pixels[i+3*width-3+1];
			green_baixo_esquerdo = pixels[i+3*width-3+2];

			red_esquerdo = pixels[i-3];
			blue_esquerdo = pixels[i-3+1];
			green_esquerdo = pixels[i-3+2];

			red_direito = pixels[i+3];
			blue_direito = pixels[i+3+1];
			green_direito = pixels[i+3+2];

			red_baixo_direito = pixels[i+3*width+3];
			blue_baixo_direito = pixels[i+3*width+3+1];
			green_baixo_direito = pixels[i+3*width+3+2];

			mean_luminance_adjances = (0.2125*red_baixo + 0.7154*red_baixo + 0.0721*blue_baixo
				+ 0.2125*red_baixo_esquerdo + 0.7154*green_baixo_esquerdo + 0.0721*blue_baixo_esquerdo
				+ 0.2125*red_baixo_direito + 0.7154*green_baixo_direito + 0.0721*blue_baixo_direito
				+ 0.2125*red_direito + 0.7154*green_direito + 0.0721*blue_direito
				+ 0.2125*red_esquerdo + 0.7154*green_esquerdo + 0.0721*blue_esquerdo
				)/5;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 

		}else if(i % (int) width-3 != 0 && !((i % (int) width*3)+3 != 9)){
			// tenho esquerdo
			red_topo_esquerdo = pixels[i-3*width-3];
			blue_topo_esquerdo = pixels[i-3*width-3+1];
			green_topo_esquerdo = pixels[i-3*width-3+2];

			red_baixo_esquerdo = pixels[i+3*width-3];
			blue_baixo_esquerdo = pixels[i+3*width-3+1];
			green_baixo_esquerdo = pixels[i+3*width-3+2];

			red_esquerdo = pixels[i-3];
			blue_esquerdo = pixels[i-3+1];
			green_esquerdo = pixels[i-3+2];

			mean_luminance_adjances = (0.2125*red_baixo + 0.7154*red_baixo + 0.0721*blue_baixo
				+ 0.2125*red_baixo_esquerdo + 0.7154*green_baixo_esquerdo + 0.0721*blue_baixo_esquerdo
				+ 0.2125*red_esquerdo + 0.7154*green_esquerdo + 0.0721*blue_esquerdo)/3;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 

		}else if(!(i % (int) width-3 != 0) && (i % (int) width*3)+3 != 9){
			// tenho direito
			red_topo_direito = pixels[i-3*width+3];
			blue_topo_direito = pixels[i-3*width+3+1];
			green_topo_direito = pixels[i-3*width+3+2];

			red_baixo_direito = pixels[i+3*width+3];
			blue_baixo_direito = pixels[i+3*width+3+1];
			green_baixo_direito = pixels[i+3*width+3+2];

			red_direito = pixels[i+3];
			blue_direito = pixels[i+3+1];
			green_direito = pixels[i+3+2];

			mean_luminance_adjances = (0.2125*red_baixo + 0.7154*red_baixo + 0.0721*blue_baixo
				+ 0.2125*red_baixo_direito + 0.7154*green_baixo_direito + 0.0721*blue_baixo_direito
				+ 0.2125*red_direito + 0.7154*green_direito + 0.0721*blue_direito)/3;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 
		}
	}else if(i > (width*3-1) && !(i < (num_pixels - width*3))){
		//tenho parte de cima 

		red_topo = pixels[i-3*width];
		green_topo = pixels[i-3*width+1];
		blue_topo = pixels[i-3*width+2];

		if(i % (int) width-3 != 0 && ((i % (int) width*3)+3 != 9)){
			// tenho esquerdo e direito
			red_topo_esquerdo = pixels[i-3*width-3];
			blue_topo_esquerdo = pixels[i-3*width-3+1];
			green_topo_esquerdo = pixels[i-3*width-3+2];

			red_esquerdo = pixels[i-3];
			blue_esquerdo = pixels[i-3+1];
			green_esquerdo = pixels[i-3+2];

			red_direito = pixels[i+3];
			blue_direito = pixels[i+3+1];
			green_direito = pixels[i+3+2];
		
			red_topo_direito = pixels[i-3*width+3];
			blue_topo_direito = pixels[i-3*width+3+1];
			green_topo_direito = pixels[i-3*width+3+2];

			mean_luminance_adjances = (0.2125*red_topo + 0.7154*green_topo + 0.0721*blue_topo
				+ 0.2125*red_topo_esquerdo + 0.7154*green_topo_esquerdo + 0.0721*blue_topo_esquerdo
				+ 0.2125*red_topo_direito + 0.7154*green_topo_direito + 0.0721*blue_topo_direito
				+ 0.2125*red_direito + 0.7154*green_direito + 0.0721*blue_direito
				+ 0.2125*red_esquerdo + 0.7154*green_esquerdo + 0.0721*blue_esquerdo
				)/5;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 

		}else if(i % (int) width-3 != 0 && !((i % (int) width*3)+3 != 9)){
			// tenho esquerdo
			red_topo_esquerdo = pixels[i-3*width-3];
			blue_topo_esquerdo = pixels[i-3*width-3+1];
			green_topo_esquerdo = pixels[i-3*width-3+2];

			red_esquerdo = pixels[i-3];
			blue_esquerdo = pixels[i-3+1];
			green_esquerdo = pixels[i-3+2];

			mean_luminance_adjances = (0.2125*red_topo + 0.7154*green_topo + 0.0721*blue_topo
				+ 0.2125*red_topo_esquerdo + 0.7154*green_topo_esquerdo + 0.0721*blue_topo_esquerdo
				+ 0.2125*red_esquerdo + 0.7154*green_esquerdo + 0.0721*blue_esquerdo
				)/3;

			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 

		}else if(!(i % (int) width-3 != 0) && (i % (int) width*3)+3 != 9){
			// tenho direito
			red_topo_direito = pixels[i-3*width+3];
			blue_topo_direito = pixels[i-3*width+3+1];
			green_topo_direito = pixels[i-3*width+3+2];

			red_direito = pixels[i+3];
			blue_direito = pixels[i+3+1];
			green_direito = pixels[i+3+2];

			mean_luminance_adjances = (0.2125*red_topo + 0.7154*green_topo + 0.0721*blue_topo
				+ 0.2125*red_topo_esquerdo + 0.7154*green_topo_esquerdo + 0.0721*blue_topo_esquerdo
				+ 0.2125*red_direito + 0.7154*green_direito + 0.0721*blue_direito)/3;
			return (mean_luminance_pixel+mean_luminance_adjances)/mean_luminance_adjances; 
		}
	}
	return 0;
}