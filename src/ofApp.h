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
		void setGUI2(float duration, float min, float max);
	
		//cenas da barra lateral e do background
		ofxUISuperCanvas *gui1;
		ofxUISuperCanvas *gui2;
		bool hideGUI;
		float red, green, blue, luminance, contrast;

		//cenas da barra de range do video
		float min_value;
		float max_value;

		//cenas do video
		ofVideoPlayer movie;
		bool isFullScreen;
		vector<string> video_paths;

		//os varios botoes da app hehe
		ofImage play_button;
		ofImage back_button;
		ofImage play_pause_button;
		ofImage stop_button;

		//cenas do swipe das imagens
		ofxSwipeable img_swipe;
		float time;
		ofPoint position;
		int current_position;

		//variaveis de controlo do ecra actual
		bool choose_video_screen;
		bool play_video_screen;
		bool load_video;
		bool entered_exited_fullscreen;
};
