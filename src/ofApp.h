#pragma once

#include "ofxOpenCv.h"
#include <opencv2/opencv.hpp>
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxSwipeable.h"
#include "ofxCvHaarFinder.h"
#include <stdio.h>
#include "Image.h"
#include <math.h>

class ofApp : public ofBaseApp {

	public:
		static const int MAX_ITEMS_PER_PAGE = 15;

		static const int GALLERY_BUTTON_WIDTH = 175;

		static const int LARGE_BUTTON_WIDTH = 110;
		static const int LARGE_BUTTON_HEIGHT = 110;

		static const int SMALL_BUTTON_HEIGHT = 75;
		static const int SMALL_BUTTON_WIDTH = 75;
		static const int SMALL_INTERVAL = 10;

		
		static const int ABOVE = 0;
		static const int BELOW = 1;
		static const int RANGE = 2;

		static const int NONE = 0;
		static const int ANY = 1;
		static const int VERTICAL = 2;
		static const int HORIZONTAL = 3;

		static const std::string RANGE_SLIDER_NAME;

		void setup();
		void update();
		void draw();
		void exit();

		void resetValues();
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void guiEvent(ofxUIEventArgs &e);
		void setGUI1();
		void setGUI2(float duration);
		void setGUI3();
		void setGUI4();
		void setGUI5();
		void setGUI6();

		string getFilterString(bool var);
		//void applyFiltersToFrame();
		void applyFiltersToFrame(ofImage img);
		bool saveFrame();
	
		//cenas da barra lateral e do background
		ofxUISuperCanvas *gui1;
		ofxUISuperCanvas *gui2;
		ofxUISuperCanvas *gui3;
		ofxUISuperCanvas *gui4;
		ofxUISuperCanvas *gui5;
		ofxUISuperCanvas *gui6;
		bool hideGUI;
		vector<string> radio_options;
		vector<string> radio_options2;

		//cenas da barra de range do video
		float range_minimum_percentage;
		float range_maximum_percentage;

		//cenas do video
		ofVideoPlayer movie;
		vector<string> video_paths;
		vector<float> movies_duration;

		//os varios botoes da app hehe
		//ofImage play_button;
		ofImage back_button;
		ofImage save_button;
		ofImage previous_button;
		ofImage next_button;
		ofImage play_pause_button;
		ofImage stop_button;
		ofImage confirm_button;
		ofImage view_gallery_button;

		//cenas do swipe das imagens
		ofxSwipeable img_swipe;
		float time;
		ofPoint position;
		int current_position; 

		//variaveis de controlo do ecra actual
		bool choose_video_and_range_screen;
		bool play_video_screen;
		bool gallery_screen;

		bool video_playing;
		bool redraw_frame_flag;
		bool load_range_gui;
		bool load_video;
		int radio_button_position; 
		int radio_button_position2; 
		
		float top_guis_start_point_width;
		float top_guis_final_point_width;
		float top_guis_width;
		float start_width_play_btn;

		int max_pages;
		int gallery_selected_page;
		int current_img;
		int contador_de_frames;

		vector<ofImage> img_array;
		vector<int> frames; 

		//variaveis p/ calculo dos valores dos filtros
		int mean_luminance; 
		double contrastVal;
		int nr_people;
		int match_object;
		double nr_edges;
		double hue_total;
		double sliders_dominant_color;
		double gabor_value;
		double img_quality;

		//variaveis dos sliders - float
		float red;
		float green;
		float blue;
		float luminance;
		float contrast; 
		float gabor_filter;
		float number_of_people;
		float number_of_edges;
		float number_of_objects;
		float quality_perc;
		
		//toggle da UI da esquerda
		bool quality_filter_enabled;
		bool dominant_color_enabled;
		bool luminance_enabled;
		bool contrast_enabled;
		bool people_enabled;
		bool gabor_enabled;
		bool object_match_enabled;

		bool object_loaded;

		ofImage objFinder; 
		bool can_update_frame;

		vector<ofImage> video_frames;
};
