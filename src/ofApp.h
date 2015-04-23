#pragma once

#include "ofMain.h"
#include "ofxUI.h"

class ofApp : public ofBaseApp{

	public:

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
    
		void drawGrid(float x, float y);
    
		void setGUI1();
	
		ofxUISuperCanvas *gui1;

		bool hideGUI;
	
		float red, green, blue;
		bool bdrawGrid;
		bool bdrawPadding;
	
		void guiEvent(ofxUIEventArgs &e);

		//cenas do video
		ofVideoPlayer fingerMovie;
		bool frameByframe;
};
