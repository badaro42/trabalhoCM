#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxSwipeable.h"

class ofApp : public ofBaseApp{

	public:
		static const int BUTTON_WIDTH = 180;
		static const int BUTTON_HEIGHT = 110;

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
	
		//cenas da barra lateral e do background
		ofxUISuperCanvas *gui1;
		bool hideGUI;
		float red, green, blue;		

		//cenas do video
		ofVideoPlayer movie;
		bool isFullScreen;
		vector<string> video_paths;

		//os varios botoes da app hehe
		ofImage play_button;
		ofImage back_button;
		ofImage play_pause_button;

		//cenas do swipe das imagens
		ofxSwipeable img_swipe;
		float time;
		ofPoint position;
		int current_position;

		//variaveis de controlo do ecra actual
		bool choose_video_screen;
		bool play_video_screen;
		bool load_video;
		bool back_button_pressed;
};
