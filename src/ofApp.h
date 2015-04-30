#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxSwipeable.h"
#include <stdio.h>

class ofApp : public ofBaseApp{

	public:
		static const int LARGE_BUTTON_WIDTH = 180;
		static const int BUTTON_HEIGHT = 110;
		static const int SMALL_BUTTON_WIDTH = 110;
		static const int SMALL_INTERVAL = 10;

		//0 - rapido, - detalhe; 1 - lento, + detalhe 
		//static const int DEFAULT_PERFORMANCE_OPTION = 1;

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
	
		//customization stuff
		//int fast_or_beauty_option; //0 - faster; 1 - slower

		//cenas da barra lateral e do background
		ofxUISuperCanvas *gui1;
		ofxUISuperCanvas *gui2;
		bool hideGUI;
		float red, green, blue, luminance, contrast;

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

		//cenas do swipe das imagens
		ofxSwipeable img_swipe;
		float time;
		ofPoint position;
		int current_position;

		//variaveis de controlo do ecra actual
		bool choose_video_and_range_screen;
		bool play_video_screen;

		bool redraw_frame_flag;
		bool load_range_gui;
		bool load_video;
		bool entered_exited_fullscreen;

		int current_img;
};
