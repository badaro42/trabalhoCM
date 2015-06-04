#include "ofApp.h"

#define FRAME_WIDTH 600
#define FRAME_HEIGHT 440
#define FADE_SIZE 50

const std::string ofApp::RANGE_SLIDER_NAME = "MTIME";

//--------------------------------------------------------------
void ofApp::setup(){

	resetValues();

	dominant_color_enabled = false;
	luminance_enabled = false;
	contrast_enabled = false;
	people_enabled = false;

	findObject = false;
	hasObject = false;
	
	video_playing = false;
	isFullScreen = false;

	choose_video_and_range_screen = true;
	play_video_screen = false;
	gallery_screen = false;

	load_video = false;
	entered_exited_fullscreen = false;
	load_range_gui = true;
	redraw_frame_flag = false;

	ofDirectory dir;
	ofVideoPlayer temp_video;

	back_button.loadImage("images/back_button.png");
	back_button.resize(SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);

	play_pause_button.loadImage("images/play_pause_button.jpg");
	play_pause_button.resize(SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);

	stop_button.loadImage("images/stop_button.jpg");
	stop_button.resize(SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);

	confirm_button.loadImage("images/apply_button.png");
	confirm_button.resize(SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);
	
	view_gallery_button.loadImage("images/view_gallery.jpg");
	view_gallery_button.resize(LARGE_BUTTON_WIDTH, BUTTON_HEIGHT);
    
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

			movies_duration.push_back(temp_video.getDuration());

			//inteiro com o numero que representa o meio do video
			int middleFrame = temp_video.getTotalNumFrames()/2;
			temp_video.setFrame(middleFrame);

			frames[i].setFromPixels(temp_video.getPixels(), 
				temp_video.getWidth(), temp_video.getHeight(),
				OF_IMAGE_COLOR);
		}
		img_swipe.setIndicatorStyle(0.9f,7,0.05f);
		img_swipe.load(frames,FRAME_WIDTH,FRAME_HEIGHT,FADE_SIZE);
		img_swipe.setAnchorPercent(0.5,0.7);
		position.set(ofGetWidth()*0.65,ofGetHeight()*0.5);
	}

	//a duraçao do primeiro filme
	float duration = movies_duration[0];
	range_minimum_percentage = 0.25;
	range_maximum_percentage = 0.75;

    time = 0;
	match_object = 0;
	current_img = 0;	
    red = 200; 
	blue = 200; 
	green = 200;
	luminance = 0; 
	contrast = 0;
    hideGUI = false;

	ofSetVerticalSync(true);

	setGUI1();
	setGUI2(duration);
}

//--------------------------------------------------------------
//UPDATE E' CHAMADO ANTES DO DRAW!!!!
void ofApp::update(){	

	int w = ofGetWidth();			  
	int h = ofGetHeight();  

	if(w != 1024 || h != 720) 
		ofSetWindowShape(1024, 720);

	//detecta quando o user muda o video que quer reproduzir!
	if(current_img != img_swipe.getCurrent()) {
		cout << "OLHA, DESTA FORMA PARECE QUE DA HIHI - " << current_img << "\n";

		current_img = img_swipe.getCurrent();
		float duration = movies_duration[current_img];
		delete gui2;
		setGUI2(duration);
	}

	//user escolheu o video e ja definiu o range pretendido, 
	//carregamos o video com os limites do range 
	if(load_video) {
		movie.loadMovie(video_paths[img_swipe.getCurrent()]);
		movie.setFrame(int(range_minimum_percentage*movie.getTotalNumFrames()));

		redraw_frame_flag = true;
		load_video = false;
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

void ofApp::resetValues() {
	contrastVal = 0;
	contador_de_frames = 0;
	mean_luminance = 0;
	nr_people = 0;
	nr_edges = 0;
	match_object = 0;
	hue_total = 0;
}

//--------------------------------------------------------------
void ofApp::draw(){
	if(!dominant_color_enabled)
		ofBackground(red, green, blue, 255);

	//estamos no primeiro ecra, para escolher video e definir o range 
	if(choose_video_and_range_screen) {
		//desenha a cena do swipe com as imagens dos videos
		if(isFullScreen)
			img_swipe.draw(ofGetWidth()*0.65, ofGetHeight()*0.5);
		else
			img_swipe.draw(position.x, position.y);

		//desenha o botao para confirmar o range
		confirm_button.draw((ofGetWidth()*0.9) - SMALL_BUTTON_WIDTH, ofGetHeight()*0.65);

		if(entered_exited_fullscreen) {
			cout << "ENTRAMOS OU SAIMOS DO FULLSCREEN\n";

			delete gui2;
			setGUI2(movies_duration[current_img]);
			entered_exited_fullscreen = false;
		}
	}

	//estamos no segundo ecra, o do video
	else if(play_video_screen) {
		back_button.draw((ofGetWidth()*0.65)-(1.5*SMALL_BUTTON_WIDTH)-SMALL_INTERVAL, 
			ofGetHeight()*0.62);
		play_pause_button.draw(ofGetWidth()*0.65-(SMALL_BUTTON_WIDTH/2), 
			ofGetHeight()*0.62);
		stop_button.draw(ofGetWidth()*0.65+(SMALL_BUTTON_WIDTH/2)+SMALL_INTERVAL, 
			ofGetHeight()*0.62);
		view_gallery_button.draw((ofGetWidth()*0.65)-(1.5*SMALL_BUTTON_WIDTH)-SMALL_INTERVAL, 
			ofGetHeight()*0.8);
	
		ofSetHexColor(0xFFFFFF);

		//fazemos isto para desenhar a primeira frame quando o user escolhe o video
		if(redraw_frame_flag)
			movie.play();
		
		movie.draw(ofGetWidth()*0.65-movie.getWidth()/2,
			ofGetHeight()*0.38-movie.getHeight()/2);

		ofSetHexColor(0x000000);

		if(video_playing)
			setFrames();

		//cout << "Contador de frames:" << contador_de_frames << "\n";
		//cout << "Contador de pessoas" << nr_people << "\n";
		ofDrawBitmapString("range chosen: from " + ofToString(int(range_minimum_percentage*movie.getTotalNumFrames())) + 
			" to " + ofToString(int(range_maximum_percentage*movie.getTotalNumFrames())),275,330);
		ofDrawBitmapString("frame: " + ofToString(movie.getCurrentFrame()) + "/"+ofToString(movie.getTotalNumFrames()),275,350);
		ofDrawBitmapString("duration: " + ofToString(movie.getPosition()*movie.getDuration(),2) + "/"+ofToString(movie.getDuration(),2),275,370);
		ofDrawBitmapString("speed: " + ofToString(movie.getSpeed(),2),275,390);
		ofDrawBitmapString("luminance: " + ofToString(mean_luminance), 275, 410);
		ofDrawBitmapString("caras: " + ofToString(nr_people), 275, 430);
		ofDrawBitmapString("contrast: " + ofToString(contrastVal), 275, 450);
		ofDrawBitmapString("objects: "  + ofToString(match_object), 275, 470);
		ofDrawBitmapString("edges: " + ofToString(nr_edges), 275, 490);
		ofDrawBitmapString("hue_total: " + ofToString(hue_total), 275, 510);
		
		//ofDrawBitmapString("caras: " + ofToString(nr_people), 275, 430);
		//paramos o video para que
		if(redraw_frame_flag) {
			movie.stop();
			redraw_frame_flag = false;
		}

		//chegamos ao ultimo frame escolhido pelo utilizador
		//uma vez que o filme esta em loop, apontamos para o min do range
		if(movie.getCurrentFrame() == (int(range_maximum_percentage*movie.getTotalNumFrames()))) {
			cout << "CHEGAMOS AO FRAME MAXIMO ESCOLHIDO PELO USER!!!\n";
			movie.setFrame(int(range_minimum_percentage*movie.getTotalNumFrames()));
		}
	}

    ofSetColor(255);

	//linhas para indicar a area onde é possivel fazer swipe com o rato
	//DESACTIVAR ANTES DA ENTREGA!!!
	/*ofLine(ofGetWidth()*0.4, ofGetHeight()*0.17, ofGetWidth()*0.4, ofGetHeight()*0.6); //vertical esquerda
	ofLine(ofGetWidth()*0.9, ofGetHeight()*0.17, ofGetWidth()*0.9, ofGetHeight()*0.6); //vertical direita
	ofLine(ofGetWidth()*0.38, ofGetHeight()*0.2, ofGetWidth()*0.92, ofGetHeight()*0.2);	//horizontal cima
	ofLine(ofGetWidth()*0.38, ofGetHeight()*0.57, ofGetWidth()*0.92, ofGetHeight()*0.57); //horizontal baixo*/

	ofLine(ofGetWidth()*0.4, ofGetHeight()*0.17, ofGetWidth()*0.4, ofGetHeight()*0.6); //vertical esquerda
	ofLine(ofGetWidth()*0.9, ofGetHeight()*0.17, ofGetWidth()*0.9, ofGetHeight()*0.6); //vertical direita
	ofLine(ofGetWidth()*0.38, ofGetHeight()*0.2, ofGetWidth()*0.92, ofGetHeight()*0.2);	//horizontal cima
	ofLine(ofGetWidth()*0.38, ofGetHeight()*0.57, ofGetWidth()*0.92, ofGetHeight()*0.57); //horizontal baixo

}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	int kind = e.getKind();
	cout << kind << "\n";
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
	}
	//eventos do grupo de botoes relacionado com a performance
	else if(kind == OFX_UI_WIDGET_RADIO)
    {
        ofxUIRadio *radio = (ofxUIRadio *) e.widget;
        cout << radio->getName() << " value: " << radio->getValue() << " active name: " << radio->getActiveName() << endl; 
	}
	else if(name == RANGE_SLIDER_NAME)
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
		//cout << slider->getName() << " low [%; #] -> [" << slider->getPercentValueLow() <<
		//	"; " << slider->getScaledValueLow() << "], high [%;#] -> [" <<
		//	slider->getPercentValueHigh() << "; " << slider->getScaledValueHigh() << "];\n";

		range_minimum_percentage = slider->getPercentValueLow();
		range_maximum_percentage = slider->getPercentValueHigh();
	}
	else if(name == "People") //slider das pessoas
	{
		int levels = int(e.getSlider()->getScaledValue());
		e.getSlider()->setValue( levels );
	}
	else if(name == "Edges") //slider das pessoas
	{
		int levels = int(e.getSlider()->getScaledValue());
		e.getSlider()->setValue(levels);
	}
	else if(name == "Luminance") //slider da luminancia
	{
		int levels = int(e.getSlider()->getScaledValue());
		e.getSlider()->setValue( levels );
	}
	else if(name == "Contrast") //slider do constraste
	{
		int levels = int(e.getSlider()->getScaledValue());
		e.getSlider()->setValue( levels );
	}
	else if(name == "Above")
		radio_button_position = ABOVE;
	else if(name == "Below")
		radio_button_position = BELOW;
	else if (name == "Neighborhood")
		radio_button_position = RANGE;
	else if(name == "Vertical")
		radio_button_position2 = VERTICAL;
	else if(name == "Any Direction")
		radio_button_position2 = ANY;
	else if(name == "None")
		radio_button_position2 = NONE;
	else if(name == "Horizontal")
		radio_button_position2 = HORIZONTAL;
	else if(name == "Open Object" && e.getButton()->getValue() == 0) {
		cout << "MERDA DOS OBJETOS" << "\n";
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a jpg or png"); 
		//Check if the user opened a file
		if (openFileResult.bSuccess){
			ofLogVerbose("User selected a file");
			path = openFileResult.getPath();
			findObject = true; 
		}else 
			ofLogVerbose("User hit cancel");
	}
	else if(name == "# of Objects"){
		int levels = int(e.getSlider()->getScaledValue());
		e.getSlider()->setValue( levels );
	}
	else if(name == "Edges"){
		int levels = int(e.getSlider()->getScaledValue());
		e.getSlider()->setValue( levels );
	}
	else if(name == "Luminance filter") {
		int val = e.getToggle()->getValue();
		if(val == 1)
			luminance_enabled = true;
		else
			luminance_enabled = false;
	}
	else if(name == "Contrast filter") {
		int val = e.getToggle()->getValue();
		if(val == 1)
			contrast_enabled = true;
		else
			contrast_enabled = false;
	}
	else if(name == "People filter") {
		int val = e.getToggle()->getValue();
		if(val == 1)
			people_enabled = true;
		else
			people_enabled = false;
	}
	else if(name == "Dominant color filter") {
		int val = e.getToggle()->getValue();
		if(val == 1)
			dominant_color_enabled = true;
		else
			dominant_color_enabled = false;
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{    
	delete gui1;

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key)
	{            
		/*case 'f':
			ofToggleFullscreen();
			isFullScreen = !isFullScreen;
			entered_exited_fullscreen = true;
			break;*/
            
		case 'h':
            gui1->toggleVisible();
			break;

		default:
			break;
	}
}

void ofApp::setGUI1()
{	
	radio_options.push_back("Above");
	radio_options.push_back("Below");
	radio_options.push_back("Neighborhood");

	radio_options2.push_back("None");
	radio_options2.push_back("Vertical");
	radio_options2.push_back("Horizontal");
	radio_options2.push_back("All");

	gui1 = new ofxUISuperCanvas("CONTROLS", 5, 5, 250, 1500);
    gui1->addLabel("Press 'h' to hide interface", OFX_UI_FONT_SMALL);
    
    gui1->addSpacer();
	gui1->addSpacer();

	string textString = "Choose the ranger to be considered when apllying the filters.";    
    gui1->addTextArea("textarea", textString, OFX_UI_FONT_SMALL);

	ofxUIRadio *radio  = gui1->addRadio("Radio Button", radio_options, OFX_UI_ORIENTATION_HORIZONTAL);
	radio->activateToggle(radio_options[0]);
	radio_button_position = 0;
	gui1->addSpacer();

	//gui1->addLabel("Filter by dominant color");
	gui1->addToggle("Dominant color filter", false);
	gui1->addSlider("Red", 0.0, 255.0, &red)->setTriggerType(OFX_UI_TRIGGER_CHANGE);
	gui1->addSlider("Green", 0.0, 255.0, &green)->setTriggerType(OFX_UI_TRIGGER_CHANGE);
	gui1->addSlider("Blue", 0.0, 255.0, &blue)->setTriggerType(OFX_UI_TRIGGER_CHANGE);

	gui1->addSpacer();
	gui1->addToggle("Luminance filter", false);
	ofxUISlider *slider1 = gui1->addSlider("Luminance", 0.0, 100.0, &luminance);
	slider1->setTriggerType(OFX_UI_TRIGGER_ALL);
	
	gui1->addSpacer();
	gui1->addToggle("Contrast filter", false);
	ofxUISlider *slider2 = gui1->addSlider("Contrast", 0.0, 100.0, &contrast);
	slider2->setTriggerType(OFX_UI_TRIGGER_ALL);
	
	gui1->addSpacer();
	gui1->addToggle("People filter", false);
	ofxUISlider *slider3 = gui1->addSlider("People", 0.0, 50.0, &number_of_people);
	slider3->setTriggerType(OFX_UI_TRIGGER_ALL);

	gui1->addSpacer();
	ofxUIRadio *radio2  = gui1->addRadio("Edges", radio_options2, OFX_UI_ORIENTATION_HORIZONTAL);
	radio2->activateToggle(radio_options2[0]);
	radio_button_position2 = NONE;   
	gui1->addSlider("Edges", 0.0, 100.0, &number_of_edges);

    gui1->addSpacer();
	gui1->addButton("Open Object", false);
	ofxUISlider *slider4 = gui1->addSlider("# of Objects", 0.0, 20.0, &number_of_objects);
	slider4->setTriggerType(OFX_UI_TRIGGER_ALL);

	gui1->addSpacer();

	/*gui1->addSpacer();
    gui1->addLabel("VERTICAL SLIDERS"); 
    gui1->addSlider("1", 0.0, 255.0, 100.0, 17, 64);
    gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
	gui1->addSlider("2", 0.0, 255.0, 150.0, 17, 64);
	gui1->addSlider("3", 0.0, 255.0, 200.0, 17, 64);
	gui1->addSlider("4", 0.0, 255.0, 250.0, 17, 64);
	gui1->addSlider("5", 0.0, 255.0, 250.0, 17, 64);
	gui1->addSlider("6", 0.0, 255.0, 250.0, 17, 64);
	gui1->addSlider("7", 0.0, 255.0, 250.0, 17, 64);
	gui1->addSlider("8", 0.0, 255.0, 250.0, 17, 64);
	gui1->addSlider("9", 0.0, 255.0, 250.0, 17, 64);
	gui1->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);*/
    
    gui1->autoSizeToFitWidgets();
	ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
}

void ofApp::setGUI2(float duration)
{
	gui2 = new ofxUISuperCanvas("", ofGetWidth()*0.4, ofGetHeight()*0.65, 
		ofGetWidth()*0.5 - SMALL_INTERVAL - SMALL_BUTTON_WIDTH, BUTTON_HEIGHT);   
    gui2->addLabel("Movie Time Interval");
	gui2->addSpacer();
	gui2->addTextArea("textarea", 
		"Choose the desired range. Press the button to confirm.", OFX_UI_FONT_SMALL);
	gui2->addSpacer();
	gui2->addRangeSlider(RANGE_SLIDER_NAME, 0.0, duration, 
		range_minimum_percentage*duration, range_maximum_percentage*duration);

	//cout << "[range_min; range_max; min_frame; max_frame] -> [" << range_minimum <<
	//	"; " << range_maximum << "; " << min_frame << "; " << max_frame << "]\n";
       
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
		choose_video_and_range_screen)
	{
		img_swipe.pressed(ofPoint(x,y)-position);
    }
	//botao para confirmar - so no primeiro ecra!!
	else if((x >= ofGetWidth()*0.9 - SMALL_BUTTON_WIDTH) &&
		(x <= ofGetWidth()*0.9) &&
		(y >= ofGetHeight()*0.65) &&
		(y <= ofGetHeight()*0.65 + BUTTON_HEIGHT) &&
		choose_video_and_range_screen) 
	{
		gui2->toggleVisible();
		choose_video_and_range_screen = false;
		play_video_screen = true;
		load_video = true;

		cout << "DENTRO DO BOTAO DE PLAY, POSICAO ACTUAL: " << img_swipe.getCurrent() << "\n";
	}
	//botao de back - no segundo ecra!!
	else if((x >= ofGetWidth()*0.65 - 1.5*SMALL_BUTTON_WIDTH - SMALL_INTERVAL) &&
		(x <= ofGetWidth()*0.65 - SMALL_BUTTON_WIDTH/2 - SMALL_INTERVAL) &&
		(y >= ofGetHeight()*0.62) &&
		(y <= ofGetHeight()*0.62 + BUTTON_HEIGHT) &&
		play_video_screen) 
	{
		video_playing = false;
		play_video_screen = false;
		choose_video_and_range_screen = true;
		gui2->toggleVisible();

		resetValues();
		
		cout << "botao de pause!!!\n";
	}
	//botao de PLAY/PAUSE - no segundo ecra!!
	else if((x >= ofGetWidth()*0.65 - SMALL_BUTTON_WIDTH/2) &&
		(x <= ofGetWidth()*0.65 + SMALL_BUTTON_WIDTH/2) &&
		(y >= ofGetHeight()*0.62) &&
		(y <= ofGetHeight()*0.62 + BUTTON_HEIGHT) &&
		play_video_screen) 
	{
		if(movie.isPlaying()) {
			movie.stop();
			video_playing = false;
		}
		else {
			movie.play();
			video_playing = true;
		}

		cout << "botao de play/pause kkkk!!!\n";
	}
	//botao de STOP - no segundo ecra!!
	else if((x >= (ofGetWidth()*0.65) + (SMALL_BUTTON_WIDTH/2) + SMALL_INTERVAL) &&
		(x <= (ofGetWidth()*0.65) + (1.5*SMALL_BUTTON_WIDTH) + SMALL_INTERVAL) &&
		(y >= ofGetHeight()*0.62) &&
		(y <= ofGetHeight()*0.62 + BUTTON_HEIGHT) &&
		play_video_screen) 
	{
		movie.stop();
		movie.setFrame(int(range_minimum_percentage*movie.getTotalNumFrames()));
		redraw_frame_flag = true;
		video_playing = false;
		cout << "botao de STOP HUEHUHE!!! min_frame: " << int(range_minimum_percentage*movie.getTotalNumFrames()) << "\n";
	}

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bool batatas = img_swipe.released(ofPoint(x,y)-position);
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

void ofApp::setFrames(){

	nr_edges = 0;
	match_object = 0;
	contrastVal = 0;
	hue_total = 0;

	int i = 0;
	int j = 0;
	int count = 0;
	float value_max = -1;
	float value_min = 100;

	unsigned char * pixels = movie.getPixels();

	//DUAS IMAGENS IGUAIS, UMA A CORES OUTRA A P/B
	ofImage image_colorful;
	ofImage image_grayscale;
	image_grayscale.setFromPixels(movie.getPixels(), movie.getWidth(), movie.getHeight(), OF_IMAGE_GRAYSCALE, true);
	image_colorful.setFromPixels(movie.getPixels(), movie.getWidth(), movie.getHeight(), OF_IMAGE_COLOR, true);
	Image img = Image(image_colorful.getPixels(), image_grayscale.getPixels(), image_colorful.getWidth(), image_colorful.getHeight());

	unsigned char * teste1 = image_grayscale.getPixels();
	unsigned char * teste2 = image_colorful.getPixels();

	float selected_color = img.calcColor(red, green, blue);
	
	//apenas calcula de 10 em 10 frames e só se o filtro estiver ativo
	if(movie.getCurrentFrame() % 10 == 0 && people_enabled) {
		ofxCvHaarFinder haarFinder; 
		haarFinder.setup("HaarFinder/haarcascade_frontalface_default.xml");
		nr_people = haarFinder.findHaarObjects(movie.getPixelsRef());
	}

	//percorremos os pixeis todos e realizamos todos os calculos
	for(i = 0; i < movie.getHeight(); i++) {
		for(j = 0; j < movie.getWidth(); j++) {
			if(luminance_enabled) 
				mean_luminance += img.calculateLuminance(i, j);

			if(dominant_color_enabled) 
				hue_total += img.calcColorAux(i, j);

			if(contrast_enabled)
				contrastVal += img.calculateContrast(i, j);

			if(radio_button_position2 != NONE){
				nr_edges += img.getEdges(i, j, radio_button_position2);
				count++;
			}
		}
	}

	//normalização dos dados :)
	nr_edges /= i*j;
	nr_edges *= 100;
	contrastVal /= i*j; 
	mean_luminance /= i*j;
	hue_total /= i*j;

	//PATTERN MATCHING - sift/surf
	if(findObject){
		match_object = img.match(path);
		cout << "matching: " << match_object << "\n";
	}

	//SÓ GUARDA AS FRAMES QUE CUMPRAM O REQUISITO DOS FILTROS!!
	if(radio_button_position == ABOVE) 
	{
		if(mean_luminance >= luminance 
			&& nr_people >= number_of_people
			&& selected_color >= hue_total
			&& contrastVal >= contrast
			&& (match_object >= number_of_objects && findObject))
		{
			contador_de_frames++;
			frames.push_back(movie.getCurrentFrame());
		}
	}
	else if(radio_button_position == BELOW)  
	{
		if(mean_luminance <= luminance 
			&& nr_people <= number_of_people
			&& selected_color <= hue_total
			&& contrastVal <= contrast
			&& (match_object <= number_of_objects && findObject))
		{
			contador_de_frames++;
			frames.push_back(movie.getCurrentFrame());
		}		
	}
	else{
		if((mean_luminance >= luminance-10 || mean_luminance <= luminance+10) 
			&& (nr_people <= number_of_people-5 || nr_people <= number_of_people+5)
			&& (selected_color >= hue_total-10 || selected_color <= hue_total+10)
			&& (contrastVal >= contrast-10 || contrastVal <= contrast+10)
			&& (match_object >= number_of_objects-10 || match_object <= number_of_objects+10 && findObject))
		{
			contador_de_frames++;
			frames.push_back(movie.getCurrentFrame());		
		}
	}
}

