#include "ofApp.h"

#define FRAME_WIDTH 600
#define FRAME_HEIGHT 440
#define FADE_SIZE 50

const std::string ofApp::RANGE_SLIDER_NAME = "MTIME";

//--------------------------------------------------------------
void ofApp::setup(){

	isFullScreen = false;
	choose_video_screen = true;
	play_video_screen = false;
	load_video = false;
	entered_exited_fullscreen = false;

	cout << "SETUP!!\n";

	min_value = 0;
	max_value = 0;

	ofDirectory dir;
	ofVideoPlayer temp_video;

	//load dos botoes da app
	play_button.loadImage("images/youtube_play.png");
	play_button.resize(LARGE_BUTTON_WIDTH, BUTTON_HEIGHT);

	back_button.loadImage("images/back_button.png");
	back_button.resize(SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);

	play_pause_button.loadImage("images/play_pause_button.jpg");
	play_pause_button.resize(SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);

	stop_button.loadImage("images/stop_button.jpg");
	stop_button.resize(SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);
    
    int nFiles = dir.listDir("movies");

	cout << "TAMANHO DA PASTA: " << nFiles << "\n";

	//se houver videos na pasta, carregamos cada um deles
	//e colocamos a frame do meio do video no ecra principal
	if(nFiles > 0) {
		
		vector<ofPixels> frames(dir.numFiles());
		for(int i = 0; i < dir.numFiles(); i++) {
			string fullPath = dir.getPath(i);
			video_paths.push_back(fullPath);

			cout << "CAMINHO DO VIDEO: " << fullPath << "\n";
			temp_video.loadMovie(fullPath);

			//inteiro com o numero que representa o meio do video
			int middleFrame = temp_video.getTotalNumFrames()/2;
			temp_video.setFrame(middleFrame);

			cout << "TAMANHO DO ARRAY 'FRAMES': " << frames.size() << "\n";

			frames[i].setFromPixels(temp_video.getPixels(), 
				temp_video.getWidth(), temp_video.getHeight(),
				OF_IMAGE_COLOR);
		}

		img_swipe.setIndicatorStyle(0.9f,7,0.05f);

		img_swipe.load(frames,FRAME_WIDTH,FRAME_HEIGHT,FADE_SIZE);
		img_swipe.setAnchorPercent(0.5,0.7);
		position.set(ofGetWidth()*0.65,ofGetHeight()*0.5);
	}
    
    time = 0;
	
    red = 200; 
	blue = 200; 
	green = 200;
	luminance = 0; 
	contrast = 0;

    hideGUI = false;

	ofSetVerticalSync(true);
    
	setGUI1();
}

//--------------------------------------------------------------
//UPDATE E' CHAMADO ANTES DO DRAW!!!!
void ofApp::update(){
	//user carregou no play, carregamos o video e começa a tocar
	//criamos aqui tambem a GUI do range do video, com base na duracao do video
	if(load_video) {
		movie.loadMovie(video_paths[img_swipe.getCurrent()]);
		movie.play();

		float duration = movie.getDuration();
		min_value = duration/4;
		max_value = 3*duration/4;

		cout << "update - DURACAO DA PUTA DO FILME: " << duration << "\n";
		setGUI2(duration, duration/4, 3*duration/4);

		choose_video_screen = false;
		load_video = false;
		play_video_screen = true;
	}

	//estamos no ecra do video e o video esta' a tocar, fazemos update
	else if(play_video_screen) {
		movie.update();
	}

	float t = ofGetElapsedTimef();
    float dt = t - time;
    time = t;
	img_swipe.update(dt);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(red, green, blue, 255);

	//estamos no primeiro ecra - INICIO
	if(choose_video_screen) {
		play_button.draw((ofGetWidth()*0.65)-(LARGE_BUTTON_WIDTH/2), ofGetHeight()*0.7);

		if(isFullScreen)
			img_swipe.draw(ofGetWidth()*0.65, ofGetHeight()*0.5);
		else
			img_swipe.draw(position.x, position.y);
	}
    
	//ofPopStyle();

	//estamos no segundo ecra, o do video
	if(play_video_screen) {
		if(entered_exited_fullscreen) {
			cout << "draw - DURACAO DA PUTA DO FILME: " << movie.getDuration() << "\n";
			cout << "draw - min: " << min_value << "; max: " << max_value << "\n";
			delete gui2;
			setGUI2(movie.getDuration(), min_value, max_value);
			entered_exited_fullscreen = false;
		}

		back_button.draw((ofGetWidth()*0.65)-(1.5*SMALL_BUTTON_WIDTH)-SMALL_INTERVAL, 
			ofGetHeight()*0.62);
		play_pause_button.draw(ofGetWidth()*0.65-(SMALL_BUTTON_WIDTH/2), 
			ofGetHeight()*0.62);
		stop_button.draw(ofGetWidth()*0.65+(SMALL_BUTTON_WIDTH/2)+SMALL_INTERVAL, 
			ofGetHeight()*0.62);
	
		ofSetHexColor(0xFFFFFF);

		movie.draw(ofGetWidth()*0.65-movie.getWidth()/2,
			ofGetHeight()*0.38-movie.getHeight()/2);

		ofSetHexColor(0x000000);
		unsigned char * pixels = movie.getPixels();
		int mean_luminance = 0;
		int i;
		int num_pixels = movie.getWidth()*movie.getHeight();

		// calculate luminance for each rbg pixel
		for (i = 0; i < num_pixels; i+=3){
			//int inverse = 255 - pixels[i];
			mean_luminance += 0.2125*pixels[i] + 0.7154*pixels[i+1] + 0.0721*pixels[i+2];
		}
		mean_luminance /= (i/3);

		/*ofSetHexColor(0x000000);
		ofDrawBitmapString("press c to change",275,290);
		if(frameByframe) ofSetHexColor(0xCCCCCC);
		ofDrawBitmapString("mouse speed position",275,310);
		if(!frameByframe) ofSetHexColor(0xCCCCCC); else ofSetHexColor(0x000000);
		ofDrawBitmapString("keys <- -> frame by frame " ,445,310);
		ofSetHexColor(0x000000);*/

		ofDrawBitmapString("frame: " + ofToString(movie.getCurrentFrame()) + "/"+ofToString(movie.getTotalNumFrames()),275,350);
		ofDrawBitmapString("duration: " + ofToString(movie.getPosition()*movie.getDuration(),2) + "/"+ofToString(movie.getDuration(),2),275,370);
		ofDrawBitmapString("speed: " + ofToString(movie.getSpeed(),2),275,390);
		ofDrawBitmapString("luminance: " + ofToString(mean_luminance), 275, 410);

		if(movie.getIsMovieDone()){
			ofSetHexColor(0xFF0000);
			ofDrawBitmapString("end of movie",250,440);
		}
	}

    ofSetColor(255);

	//linhas para indicar a area onde é possivel fazer swipe com o rato
	//DESACTIVAR ANTES DA ENTREGA!!!
	ofLine(ofGetWidth()*0.4, ofGetHeight()*0.17, ofGetWidth()*0.4, ofGetHeight()*0.6); //vertical esquerda
	ofLine(ofGetWidth()*0.9, ofGetHeight()*0.17, ofGetWidth()*0.9, ofGetHeight()*0.6); //vertical direita
	ofLine(ofGetWidth()*0.38, ofGetHeight()*0.2, ofGetWidth()*0.92, ofGetHeight()*0.2);	//horizontal cima
	ofLine(ofGetWidth()*0.38, ofGetHeight()*0.57, ofGetWidth()*0.92, ofGetHeight()*0.57); //horizontal baixo

}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	int kind = e.getKind();
	cout << "got event from: " << name << endl;

    if(kind == OFX_UI_WIDGET_NUMBERDIALER)
    {
        ofxUINumberDialer *n = (ofxUINumberDialer *) e.widget;
        cout << n->getValue() << endl;
    }
	
    if(name == "SAMPLER")
    {
        ofxUIImageSampler *is = (ofxUIImageSampler *) e.widget;
        ofColor clr = is->getColor();
        red = clr.r;
        blue = clr.b;
        green = clr.g;
    }
	else if(name == "BUTTON")
	{
		ofxUIButton *button = (ofxUIButton *) e.getButton();
		//bdrawGrid = button->getValue();
	}
	else if(name == "TOGGLE")
	{
		ofxUIToggle *toggle = (ofxUIToggle *) e.getToggle();
		//showVideo = toggle->getValue();

		//cout << "SHOW-VIDEO" << showVideo;
		//update();
	}
    else if(name == "RADIO VERTICAL")
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        cout << radio->getName() << " value: " << radio->getValue() << " active name: " << radio->getActiveName() << endl; 
    }
	else if(name == RANGE_SLIDER_NAME)
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
		cout << slider->getName() << " low [%; #] -> [" << slider->getPercentValueLow() <<
			"; " << slider->getScaledValueLow() << "], high [%;#] -> [" <<
			slider->getPercentValueHigh() << "; " << slider->getScaledValueHigh() << "];\n";
		min_value = slider->getScaledValueLow();
		max_value = slider->getScaledValueHigh();
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{
    //gui1->saveSettings("gui1Settings.xml");
    
	delete gui1;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key)
	{            
		case 'f':
			ofToggleFullscreen();
			isFullScreen = !isFullScreen;
			entered_exited_fullscreen = true;
			break;
            
		case 'h':
            gui1->toggleVisible();
			break;

		default:
			break;
	}
}

void ofApp::setGUI1()
{
    vector<string> names;
	names.push_back("RAD1");
	names.push_back("RAD2");
	names.push_back("RAD3");
	
	gui1 = new ofxUISuperCanvas("CONTROLOS!", 5, 5, 250, 1500);
    gui1->addSpacer();
    gui1->addLabel("Press 'h' to hide interface", OFX_UI_FONT_SMALL);
    
    gui1->addSpacer();
	gui1->addLabel("Main Color:");
	gui1->addSlider("RED", 0.0, 255.0, &red)->setTriggerType(OFX_UI_TRIGGER_ALL);
	gui1->addSlider("GREEN", 0.0, 255.0, &green)->setTriggerType(OFX_UI_TRIGGER_BEGIN|OFX_UI_TRIGGER_CHANGE|OFX_UI_TRIGGER_END);
	gui1->addSlider("BLUE", 0.0, 255.0, &blue)->setTriggerType(OFX_UI_TRIGGER_BEGIN|OFX_UI_TRIGGER_CHANGE);
    
   /* gui1->addSpacer();
    gui1->addLabel("V SLIDERS");
	gui1->addSlider("0", 0.0, 255.0, 150, 17, 160);
	gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	gui1->addSlider("1", 0.0, 255.0, 150, 17, 160);
	gui1->addSlider("2", 0.0, 255.0, 150, 17, 160);
	gui1->addSlider("3", 0.0, 255.0, 150, 17, 160);
	gui1->addSlider("4", 0.0, 255.0, 150, 17, 160);
	gui1->addSlider("5", 0.0, 255.0, 150, 17, 160);
	gui1->addSlider("6", 0.0, 255.0, 150, 17, 160);
	gui1->addSlider("7", 0.0, 255.0, 150, 17, 160);
	gui1->addSlider("8", 0.0, 255.0, 150, 17, 160);
	gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui1->addSpacer();
	gui1->addRadio("RADIO HORIZONTAL", names, OFX_UI_ORIENTATION_HORIZONTAL);
	gui1->addRadio("RADIO VERTICAL", names, OFX_UI_ORIENTATION_VERTICAL);
    
    gui1->addSpacer();
    gui1->setWidgetFontSize(OFX_UI_FONT_SMALL);
	gui1->addButton("BUTTON", false);
	gui1->addToggle("TOGGLE", false);*/
    
	gui1->addSpacer();
	gui1->addSlider("Luminance", 0.0, 100.0, &luminance)->setTriggerType(OFX_UI_TRIGGER_ALL);
	gui1->addSlider("Contrast", 0.0, 100.0, &contrast)->setTriggerType(OFX_UI_TRIGGER_BEGIN|OFX_UI_TRIGGER_CHANGE|OFX_UI_TRIGGER_END);

    gui1->addSpacer();
    gui1->addLabel("RANGE SLIDER");
	gui1->addRangeSlider("RSLIDER", 0.0, 255.0, 50.0, 100.0);
    
    string textString = "This widget is a text area widget. Use this when you need to display a paragraph of text. It takes care of formatting the text to fit the block.";
    gui1->addSpacer();
    
    gui1->addTextArea("textarea", textString, OFX_UI_FONT_SMALL);
    
    gui1->autoSizeToFitWidgets();
	ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
}

void ofApp::setGUI2(float duration, float min, float max)
{
	gui2 = new ofxUISuperCanvas("", ofGetWidth()*0.4, ofGetHeight()*0.8, ofGetWidth()*0.5, 75);   
    gui2->addLabel("Movie Time Interval");
	gui2->addRangeSlider(RANGE_SLIDER_NAME, 0.0, duration, min, max);
       
    //gui2->autoSizeToFitWidgets();
	ofAddListener(gui2->newGUIEvent,this,&ofApp::guiEvent);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//------------------------------------------------------------
//so é possivel fazer swipe dentro da area do video!
void ofApp::mousePressed(int x, int y, int button){
	//swipe dentro do rectangulo das imagens do video - so no primeiro ecra
	if((x >= ofGetWidth()*0.4) && (x <= ofGetWidth()*0.9) &&
		(y >= ofGetHeight()*0.2) && (y <= ofGetHeight()*0.57) &&
		choose_video_screen){
		img_swipe.pressed(ofPoint(x,y)-position);

    }
	//botao de play - so no primeiro ecra!!
	else if((x >= ofGetWidth()*0.65 - LARGE_BUTTON_WIDTH/2) &&
		(x <= ofGetWidth()*0.65 + LARGE_BUTTON_WIDTH/2) &&
		(y >= ofGetHeight()*0.7) &&
		(y <= ofGetHeight()*0.7 + BUTTON_HEIGHT) &&
		choose_video_screen) {
			load_video = true;
			cout << "DENTRO DO BOTAO DE PLAY, POSICAO ACTUAL: " << img_swipe.getCurrent() << "\n";
	}
	//botao de back - no segundo ecra!!
	else if((x >= ofGetWidth()*0.65 - 1.5*SMALL_BUTTON_WIDTH - SMALL_INTERVAL) &&
		(x <= ofGetWidth()*0.65 - SMALL_BUTTON_WIDTH/2 - SMALL_INTERVAL) &&
		(y >= ofGetHeight()*0.62) &&
		(y <= ofGetHeight()*0.62 + BUTTON_HEIGHT) &&
		play_video_screen) {
			play_video_screen = false;
			choose_video_screen = true;
			movie.stop();
			gui2->toggleVisible();
			cout << "botao de pause!!!\n";
	}
	//botao de PLAY/PAUSE - no segundo ecra!!
	else if((x >= ofGetWidth()*0.65 - SMALL_BUTTON_WIDTH/2) &&
		(x <= ofGetWidth()*0.65 + SMALL_BUTTON_WIDTH/2) &&
		(y >= ofGetHeight()*0.62) &&
		(y <= ofGetHeight()*0.62 + BUTTON_HEIGHT) &&
		play_video_screen) {
			if(movie.isPlaying())
				movie.stop();
			else
				movie.play();
			cout << "botao de play/pause kkkk!!!\n";
	}
	//botao de STOP - no segundo ecra!!
	else if((x >= (ofGetWidth()*0.65)+(SMALL_BUTTON_WIDTH/2)+SMALL_INTERVAL) &&
		(x <= (ofGetWidth()*0.65)+(1.5*SMALL_BUTTON_WIDTH)+SMALL_INTERVAL) &&
		(y >= ofGetHeight()*0.62) &&
		(y <= ofGetHeight()*0.62 + BUTTON_HEIGHT) &&
		play_video_screen) {
			movie.firstFrame();
			movie.stop();
			//movie.play;
			cout << "botao de STOP HUEHUHE!!! duration: " << movie.getDuration() << "\n";

	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	img_swipe.released(ofPoint(x,y)-position);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	 //cout << "WINDOWS RESIZED!!!!\n";
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}