#pragma once

#include "ofxOpenCv.h"
#include <opencv2/opencv.hpp>
#include "ofMain.h"
#include "ofxUI.h"
#include "ofxSwipeable.h"
#include "ofxCvHaarFinder.h"
#include <stdio.h>
#include "Image.h"

class ofApp : public ofBaseApp {

	public:
		static const int LARGE_BUTTON_WIDTH = 350;
		static const int BUTTON_HEIGHT = 110;
		static const int SMALL_BUTTON_WIDTH = 110;
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

		void setFrames();
	
		//customization stuff
		//int fast_or_beauty_option; //0 - faster; 1 - slower

		//cenas da barra lateral e do background
		ofxUISuperCanvas *gui1;
		ofxUISuperCanvas *gui2;
		bool hideGUI;
		float red, green, blue, luminance, contrast, number_of_people, dominant_color, number_of_edges, number_of_objects;
		vector<string> radio_options;
		vector<string> radio_options2;

		//cenas da barra de range do video
		float range_minimum_percentage;
		float range_maximum_percentage;

		//cenas do video
		ofVideoPlayer movie;
		bool isFullScreen;
		vector<string> video_paths;
		vector<float> movies_duration;

		//os varios botoes da app hehe
		//ofImage play_button;
		ofImage back_button;
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
		bool entered_exited_fullscreen;
		int radio_button_position; 
		int radio_button_position2; 
		float calcSelectedColor();

		int current_img;

		int contador_de_frames;
		vector<int> frames; 
		int mean_luminance; 
		double contrastVal;
		int nr_people;
		int nr_edges;

		string path; 
		bool findObject;
		bool hasObject; 
		int match_object;
};
