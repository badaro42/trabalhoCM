#include "ofApp.h"

#define FRAME_WIDTH 550
#define FRAME_HEIGHT 350
#define FADE_SIZE 150

const std::string ofApp::RANGE_SLIDER_NAME = "MTIME";

//--------------------------------------------------------------
void ofApp::setup(){

	resetValues();
	gallery_selected_page = 0;
	max_pages = 0;
	can_update_frame = true;

	quality_filter_enabled = false;
	dominant_color_enabled = false;
	luminance_enabled = false;
	contrast_enabled = false;
	people_enabled = false;
	gabor_enabled = false;
	object_match_enabled = false;

	object_loaded = false;
	
	video_playing = false;

	choose_video_and_range_screen = true;
	play_video_screen = false;
	gallery_screen = false;

	load_video = false;
	load_range_gui = true;
	redraw_frame_flag = false;

	ofDirectory dir;
	ofVideoPlayer temp_video;

	back_button.loadImage("images/back.png");
	back_button.resize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);

	save_button.loadImage("images/save.png");
	save_button.resize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);

	next_button.loadImage("images/next.png");
	next_button.resize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);

	previous_button.loadImage("images/next.png");
	previous_button.resize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);
	previous_button.rotate90(2); // roda a imaagens 90 graus 2 vezes -> 180

	play_pause_button.loadImage("images/play_pause_button.jpg");
	play_pause_button.resize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);

	stop_button.loadImage("images/stop_button.jpg");
	stop_button.resize(SMALL_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);

	confirm_button.loadImage("images/apply_button.png");
	confirm_button.resize(LARGE_BUTTON_WIDTH, LARGE_BUTTON_HEIGHT);
	
	view_gallery_button.loadImage("images/view_gallery.jpg");
	view_gallery_button.resize(GALLERY_BUTTON_WIDTH, SMALL_BUTTON_HEIGHT);
    
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

			ofImage temp_img;
			temp_img.setFromPixels(temp_video.getPixels(), temp_video.getWidth(), temp_video.getHeight(), OF_IMAGE_COLOR, true);
			temp_img.resize(FRAME_WIDTH, FRAME_HEIGHT);

			frames[i].setFromPixels(temp_img.getPixels(), 
				temp_img.getWidth(), temp_img.getHeight(),
				OF_IMAGE_COLOR);
		}
		img_swipe.setIndicatorStyle(0.9f,7,0.05f);
		img_swipe.load(frames,FRAME_WIDTH,FRAME_HEIGHT,FADE_SIZE);
		img_swipe.setAnchorPercent(0.5,0.45);
		position.set(ofGetWidth()*0.63,ofGetHeight()*0.49);
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

	top_guis_start_point_width = ofGetWidth()*0.30;
	top_guis_final_point_width = ofGetWidth()*0.96;
	top_guis_width = ((top_guis_final_point_width - top_guis_start_point_width)/2) - 20;

	start_width_play_btn = ofGetWidth()*0.34 + 2*SMALL_BUTTON_WIDTH + 2*SMALL_INTERVAL;
	 
	setGUI1(); //barra lateral
	setGUI2(duration); //gui do range do video
	setGUI3(); //gui do topo esquerdo - apenas no ecra do video a tocar
	setGUI4(); //gui do topo direito - apenas no ecra do video a tocar
	setGUI5(); //gui da galeria
	setGUI6(); //gui da pagina inicial - mostra info sobre a app.

	gui3->toggleVisible(); //começa escondida!
	gui4->toggleVisible(); //começa escondida!
	gui5->toggleVisible(); //começa escondida!
}

//--------------------------------------------------------------
//UPDATE E' CHAMADO ANTES DO DRAW!!!!
void ofApp::update(){	

	int w = ofGetWidth();			  
	int h = ofGetHeight();  

	//evita que seja feito resize à janela
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
	else if(play_video_screen && can_update_frame) {
		//cout << movie.getCurrentFrame() << "\n";
		movie.update();
	}

	float t = ofGetElapsedTimef();
    float dt = t - time;
    time = t;
	img_swipe.update(dt);
}

void ofApp::resetValues() {
	contador_de_frames = 0;
	
	sliders_dominant_color = 0;
	contrastVal = 0;
	mean_luminance = 0;
	nr_people = 0;
	nr_edges = 0;
	match_object = 0;
	hue_total = 0;
	gabor_value = 0;
}

//--------------------------------------------------------------
void ofApp::draw(){
	if(!dominant_color_enabled)
		ofBackground(red, green, blue, 255);

	//desenha os quatros cantos à volta do video. apenas possivel nos 2 primeiros ecras
	if(choose_video_and_range_screen || play_video_screen) {
		ofSetColor(0);

		//linhas para indicar a area onde é possivel fazer swipe com o rato
		ofLine(ofGetWidth()*0.34, ofGetHeight()*0.24, ofGetWidth()*0.34, ofGetHeight()*0.24+75); //topo esquerdo vert.
		ofLine(ofGetWidth()*0.34, ofGetHeight()*0.24, ofGetWidth()*0.34+75, ofGetHeight()*0.24); //topo esquerdo horiz.
	
		ofLine(ofGetWidth()*0.34, ofGetHeight()*0.79, ofGetWidth()*0.34+75, ofGetHeight()*0.79); //fundo esquerdo horiz.
		ofLine(ofGetWidth()*0.34, ofGetHeight()*0.79, ofGetWidth()*0.34, ofGetHeight()*0.79-75); //fundo esquerdo vert.
	
		ofLine(ofGetWidth()*0.92, ofGetHeight()*0.24, ofGetWidth()*0.92, ofGetHeight()*0.24+75); //topo direito vert.
		ofLine(ofGetWidth()*0.92, ofGetHeight()*0.24, ofGetWidth()*0.92-75, ofGetHeight()*0.24); //topo direito horiz.
	
		ofLine(ofGetWidth()*0.92, ofGetHeight()*0.79, ofGetWidth()*0.92-75, ofGetHeight()*0.79); //fundo direito horiz.
		ofLine(ofGetWidth()*0.92, ofGetHeight()*0.79, ofGetWidth()*0.92, ofGetHeight()*0.79-75); //fundo direito vert.

		if(object_loaded)
			ofDrawBitmapString("[File loaded]", ofGetWidth()*0.12, ofGetHeight()*0.822);
		else
			ofDrawBitmapString("[No file loaded]", ofGetWidth()*0.12, ofGetHeight()*0.822);

		ofSetColor(255);
	}

	//estamos no primeiro ecra, para escolher video e definir o range 
	if(choose_video_and_range_screen) {
		//desenha a cena do swipe com as imagens dos videos
		img_swipe.draw(position.x, position.y);

		//desenha o botao para confirmar o range
		confirm_button.draw((ofGetWidth()*0.92) - LARGE_BUTTON_WIDTH, ofGetHeight()*0.81);
	
		ofSetColor(0);

		ofDrawBitmapString("Start by choosing a video. To do that, swipe left/right on the bounding box.", ofGetWidth()*0.304, ofGetHeight()*0.065);
		ofDrawBitmapString("You can change the filters applied to the video frames at any given moment.", ofGetWidth()*0.304, ofGetHeight()*0.090);
		ofDrawBitmapString("Then, choose the desired range to be processed and confirm it with the", ofGetWidth()*0.304, ofGetHeight()*0.125);
		ofDrawBitmapString("button on the right.", ofGetWidth()*0.304, ofGetHeight()*0.15);
		
		ofSetColor(255);
	}

	//estamos no segundo ecra, o do video
	else if(play_video_screen) {

		back_button.draw(ofGetWidth()*0.34, 
			ofGetHeight()*0.81);
		play_pause_button.draw(start_width_play_btn, 
			ofGetHeight()*0.81);
		stop_button.draw(start_width_play_btn + SMALL_BUTTON_WIDTH + SMALL_INTERVAL, 
			ofGetHeight()*0.81);
		view_gallery_button.draw((ofGetWidth()*0.92)-GALLERY_BUTTON_WIDTH, 
			ofGetHeight()*0.81);
	
		ofSetColor(255);

		//fazemos isto para desenhar a primeira frame quando o user escolhe o video
		if(redraw_frame_flag)
			movie.play();
		
		movie.draw(ofGetWidth()*0.361, ofGetHeight()*0.27, FRAME_WIDTH, FRAME_HEIGHT);

		ofSetHexColor(0);

		if(video_playing && can_update_frame) {
			if(movie.isFrameNew()) {
				//applyFiltersToFrame();
				cout << movie.getCurrentFrame() << "\n";
				ofImage img;
				img.setFromPixels(movie.getPixels(), movie.getWidth(), movie.getHeight(), OF_IMAGE_COLOR);
				video_frames.push_back(img);
			}
		}

		float right_gui_left_column = top_guis_final_point_width - top_guis_width + 4;
		float right_gui_right_column = (top_guis_final_point_width - top_guis_width/2) + 2;

		//strings dos metadados do video
		ofDrawBitmapString("Video path: " + movie.getMoviePath(), ofGetWidth()*0.304, ofGetHeight()*0.065);
		ofDrawBitmapString("Range chosen: " + ofToString(int(range_minimum_percentage*movie.getTotalNumFrames())) + 
			" to " + ofToString(int(range_maximum_percentage*movie.getTotalNumFrames())) + ".",ofGetWidth()*0.304, ofGetHeight()*0.090);
		ofDrawBitmapString("Frame (current/total): " + ofToString(movie.getCurrentFrame()) + "/"+ofToString(movie.getTotalNumFrames()), ofGetWidth()*0.304, ofGetHeight()*0.115);
		ofDrawBitmapString("Duration (current/total): " + ofToString(movie.getPosition()*movie.getDuration(),2) + "/"+ofToString(movie.getDuration(),2), 
			ofGetWidth()*0.304, ofGetHeight()*0.140);
		ofDrawBitmapString("Speed: " + ofToString(movie.getSpeed(),2),ofGetWidth()*0.304, ofGetHeight()*0.165);
		
		bool lol;
		if(radio_button_position2 == NONE)
			lol = false;
		else
			lol = true;

		bool is_active = object_match_enabled && object_loaded;

		//strings dos dos valores dos filtros aplicados
		ofDrawBitmapString(getFilterString(dominant_color_enabled) + 
			" Dominant color", right_gui_left_column, ofGetHeight()*0.065);
		ofDrawBitmapString(getFilterString(luminance_enabled) + 
			" Luminance", right_gui_left_column, ofGetHeight()*0.100);
		ofDrawBitmapString(getFilterString(contrast_enabled) + 
			" Contrast", right_gui_left_column, ofGetHeight()*0.135);
		ofDrawBitmapString(getFilterString(people_enabled) + 
			" People", right_gui_left_column, ofGetHeight()*0.170);
		ofDrawBitmapString(getFilterString(lol) + 
			" Edges", right_gui_right_column, ofGetHeight()*0.065);
		ofDrawBitmapString(getFilterString(gabor_enabled) + 
			" Texture", right_gui_right_column, ofGetHeight()*0.100);
		ofDrawBitmapString(getFilterString(is_active) + 
			" Match", right_gui_right_column, ofGetHeight()*0.135);
		ofDrawBitmapString(getFilterString(quality_filter_enabled) + 
			" Quality", right_gui_right_column, ofGetHeight()*0.170);

		//paramos o video para que nao ande para a frente
		if(redraw_frame_flag) {
			movie.stop();
			redraw_frame_flag = false;
		}

		//chegamos ao ultimo frame escolhido pelo utilizador
		//uma vez que o filme esta em loop, apontamos para o min do range
		if(movie.getCurrentFrame() >= (int(range_maximum_percentage*movie.getTotalNumFrames()))) {
			cout << "CHEGAMOS AO FRAME MAXIMO ESCOLHIDO PELO USER!!!\n";
			movie.setFrame(int(range_minimum_percentage*movie.getTotalNumFrames()));
			movie.stop();

			int i;
			for(i = 0; i < video_frames.size(); i++) {
				applyFiltersToFrame(video_frames[i]);
			}

			cout << video_frames.size() << "\n";
			video_frames.clear();
		}
	}

	//estamos no terceiro ecra, o da galeria, vamos mostrar as imagens
	else if(gallery_screen) {
		ofSetColor(255);

		/*ofLine(ofGetWidth()*0.5, ofGetHeight()*0.79, 
			ofGetWidth()*0.5, ofGetHeight()*0.89); //linha ao centro
		ofLine(ofGetWidth()*0.25, ofGetHeight()*0.79, 
			ofGetWidth()*0.25, ofGetHeight()*0.89); //linha a 25%
		ofLine(ofGetWidth()*0.75, ofGetHeight()*0.79, 
			ofGetWidth()*0.75, ofGetHeight()*0.89);*/ //linha a 75%

		back_button.draw(ofGetWidth()*0.2, 
			ofGetHeight()*0.86);
		previous_button.draw(ofGetWidth()*0.5 - SMALL_BUTTON_HEIGHT - SMALL_INTERVAL/2, 
			ofGetHeight()*0.86);
		next_button.draw(ofGetWidth()*0.5 + SMALL_INTERVAL/2, 
			ofGetHeight()*0.86);
		save_button.draw(ofGetWidth()*0.73, 
			ofGetHeight()*0.86);

		//indica em que elemento começa o ciclo
		int curr_index = MAX_ITEMS_PER_PAGE*(gallery_selected_page-1);

		if(gallery_selected_page == 2)
			int x = curr_index;
		
		int img_width = 150;
		int img_height = 125;
		int init_width = 75;
		int init_height = 165;

		int row_width = 180;
		int max_columns = 4;
		int row = 1;
		int elems_per_row = 5;

		int top;
		int diff = img_array.size() - curr_index;

		//nao dá para encher a pagina toda!
		if(diff < MAX_ITEMS_PER_PAGE)
			top = diff%MAX_ITEMS_PER_PAGE;
		else
			top = MAX_ITEMS_PER_PAGE;

		int i;
		for(i = 0; i < top; i++) {
			row = floor(i/elems_per_row);
			img_array[curr_index].draw(init_width + (i%elems_per_row)*row_width, 
				init_height + img_width*row, img_width, img_height);
			curr_index++;
		}

		ofDrawBitmapString("img_array.size: " + ofToString(img_array.size()), 400, 75);
		ofSetColor(0);
	}
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
	string name = e.getName();
	int kind = e.getKind();
	cout << kind << "\n";
	cout << "got event from: " << name << endl;

	if(name == RANGE_SLIDER_NAME)
	{
		ofxUIRangeSlider *slider = (ofxUIRangeSlider *) e.widget;
		range_minimum_percentage = slider->getPercentValueLow();
		range_maximum_percentage = slider->getPercentValueHigh();
	}
	else if(name == "Red" || name == "Green" || name == "Blue" || name == "# of People" ||
		name == "% of Edges" || name == "Luminance" || name == "# of Objects") //coloca o step do slider de 1 em 1!
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
	else if(name == "All")
		radio_button_position2 = ANY;
	else if(name == "None")
		radio_button_position2 = NONE;
	else if(name == "Horizontal")
		radio_button_position2 = HORIZONTAL;
	else if(name == "Open Object" && e.getButton()->getValue() == 0) {
		ofFileDialogResult openFileResult = ofSystemLoadDialog("Select a jpg or png"); 
		//Check if the user opened a file
		if (openFileResult.bSuccess){
			ofLogVerbose("User selected a file");
			objFinder.loadImage(openFileResult.getPath());
			object_loaded = true; 
		}
		else 
			ofLogVerbose("User hit cancel");
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
	else if(name == "Gabor filter") {
		int val = e.getToggle()->getValue();
		if(val == 1)
			gabor_enabled = true;
		else
			gabor_enabled = false;
	}
	else if(name == "Quality-based filter") {
		int val = e.getToggle()->getValue();
		if(val == 1)
			quality_filter_enabled = true;
		else
			quality_filter_enabled = false;
	}
	else if(name == "Object match filter") {
		int val = e.getToggle()->getValue();
		if(val == 1)
			object_match_enabled = true;
		else
			object_match_enabled = false;
	}
}

//--------------------------------------------------------------
void ofApp::exit()
{    
	delete gui1;
	delete gui2;
	delete gui3;
	delete gui4;
	delete gui5;
	delete gui6;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key)
	{                        
		case 'h':
            gui1->toggleVisible();
			break;

		default:
			break;
	}
}

//GUI DOS CONTROLOS: LADO ESQUERDO DA APP
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
	gui1->addSpacer();
	gui1->addToggle("Dominant color filter", false);
	gui1->addSlider("Red", 0.0, 255.0, &red)->setTriggerType(OFX_UI_TRIGGER_ALL);
	gui1->addSlider("Green", 0.0, 255.0, &green)->setTriggerType(OFX_UI_TRIGGER_ALL);
	gui1->addSlider("Blue", 0.0, 255.0, &blue)->setTriggerType(OFX_UI_TRIGGER_ALL);

	gui1->addSpacer();
	gui1->addToggle("Luminance filter", false);
	ofxUISlider *slider1 = gui1->addSlider("Luminance", 0.0, 255.0, &luminance);
	slider1->setTriggerType(OFX_UI_TRIGGER_ALL);
	
	gui1->addSpacer();
	gui1->addToggle("Contrast filter", false);
	ofxUISlider *slider2 = gui1->addSlider("Contrast", 0.0, 10.0, &contrast);
	slider2->setTriggerType(OFX_UI_TRIGGER_ALL);
	
	gui1->addSpacer();
	gui1->addToggle("People filter", false);
	ofxUISlider *slider3 = gui1->addSlider("# of People", 0.0, 50.0, &number_of_people);
	slider3->setTriggerType(OFX_UI_TRIGGER_ALL);

	gui1->addSpacer();
	ofxUIRadio *radio2  = gui1->addRadio("Edges", radio_options2, OFX_UI_ORIENTATION_HORIZONTAL);
	radio2->activateToggle(radio_options2[0]);
	radio_button_position2 = NONE;   
	gui1->addSlider("% of Edges", 0.0, 100.0, &number_of_edges);

	gui1->addSpacer();
	gui1->addToggle("Gabor filter", false);
	ofxUISlider *slider5 = gui1->addSlider("% of Texture", 0.0, 15.0, &gabor_filter);
	slider5->setTriggerType(OFX_UI_TRIGGER_ALL);

    gui1->addSpacer();
	gui1->addToggle("Object match filter", false);
	gui1->addButton("Open Object", false);
	ofxUISlider *slider4 = gui1->addSlider("# of Objects", 0.0, 20.0, &number_of_objects);
	slider4->setTriggerType(OFX_UI_TRIGGER_ALL);

	gui1->addSpacer();
	gui1->addToggle("Quality-based filter", false);
	gui1->addSpacer();

    gui1->autoSizeToFitWidgets();
	ofAddListener(gui1->newGUIEvent,this,&ofApp::guiEvent);
}

//GUI DO RANGE DO VIDEO: POR BAIXO DO VIDEO, APENAS PRIMEIRO ECRA
void ofApp::setGUI2(float duration)
{
	gui2 = new ofxUISuperCanvas("", ofGetWidth()*0.34, ofGetHeight()*0.81, 
		ofGetWidth()*0.56 - SMALL_INTERVAL - LARGE_BUTTON_WIDTH, LARGE_BUTTON_HEIGHT);   
    gui2->addLabel("Movie Time Interval");

	gui2->addSpacer();
	gui2->addTextArea("textarea", 
		"Choose the desired range. Press the button to confirm.", OFX_UI_FONT_SMALL);
	gui2->addSpacer();
	gui2->addRangeSlider(RANGE_SLIDER_NAME, 0.0, duration, 
		range_minimum_percentage*duration, range_maximum_percentage*duration);
       
	ofAddListener(gui2->newGUIEvent,this,&ofApp::guiEvent);
}

//GUI DA INFO DO VIDEO: POR CIMA DO VIDEO, À ESQUERDA, APENAS ECRA DO VIDEO A TOCAR
void ofApp::setGUI3() {
	gui3 = new ofxUISuperCanvas("Selected movie metadata", 
		top_guis_start_point_width, 5, top_guis_width, ofGetHeight()*0.18);
	gui3->addSpacer();
}

//GUI DOS VALORES DOS FILTROS: POR CIMA DO VIDEO, À DIREITA, APENAS ECRA DO VIDEO A TOCAR
void ofApp::setGUI4() {
	gui4 = new ofxUISuperCanvas("Filters information", 
		top_guis_final_point_width - top_guis_width, 5, 
		top_guis_width, ofGetHeight()*0.18);
	gui4->addSpacer();
}

//GUI DA GALERIA: POR CIMA DAS IMAGENS, APENAS NO ECRA DA GALERIA
void ofApp::setGUI5() {
	gui5 = new ofxUISuperCanvas("Frames that match your criteria", 
		ofGetWidth()*0.2, 5, 
		ofGetWidth()*0.6, ofGetHeight()*0.18);
	gui5->addSpacer();
}

//GUI DA INFO DO VIDEO: POR CIMA DO VIDEO, À ESQUERDA, APENAS ECRA DO VIDEO A TOCAR
void ofApp::setGUI6() {
	gui6 = new ofxUISuperCanvas("How to use this application", 
		top_guis_start_point_width, 5, ofGetWidth()*0.66, ofGetHeight()*0.18);
	gui6->addSpacer();
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

	float temp_stop_btn_width = start_width_play_btn + SMALL_BUTTON_WIDTH + SMALL_INTERVAL;

	//primeiro ecra - escolha do video
	if(choose_video_and_range_screen) {
		//swipe dentro do rectangulo das imagens do video - so no primeiro ecra - TÁ!
		if((x >= ofGetWidth()*0.34) && (x <= ofGetWidth()*0.92) &&
			(y >= ofGetHeight()*0.24) && (y <= ofGetHeight()*0.79))
		{
			img_swipe.pressed(ofPoint(x,y)-position);
			cout << "CENA DO SWIPE!!!!1!!!!" << "\n";
		}
		//botao para confirmar - so no primeiro ecra!! - TÁ!
		else if((x >= ofGetWidth()*0.92 - LARGE_BUTTON_WIDTH) &&
			(x <= ofGetWidth()*0.92) &&
			(y >= ofGetHeight()*0.81) &&
			(y <= ofGetHeight()*0.81 + LARGE_BUTTON_HEIGHT)) 
		{
			gui2->toggleVisible(); //esconde
			gui3->toggleVisible(); //mostra
			gui4->toggleVisible(); //mostra
			gui6->toggleVisible(); //esconde

			choose_video_and_range_screen = false;
			play_video_screen = true;
			load_video = true;

			cout << "DENTRO DO BOTAO DE PLAY, POSICAO ACTUAL: " << img_swipe.getCurrent() << "\n";
		}
	}

	//segundo ecra - controlos do video
	else if(play_video_screen) {
			//botao de back - no segundo ecra!! - TÁ!
		if((x >= ofGetWidth()*0.34) && (x <= ofGetWidth()*0.34 + SMALL_BUTTON_WIDTH) &&
			(y >= ofGetHeight()*0.81) && (y <= ofGetHeight()*0.81 + SMALL_BUTTON_HEIGHT)) 
		{
			video_playing = false;
			play_video_screen = false;
			choose_video_and_range_screen = true;
			
			gui2->toggleVisible(); //mostra
			gui3->toggleVisible(); //esconde
			gui4->toggleVisible(); //esconde
			gui6->toggleVisible(); //mostra

			resetValues();
		
			cout << "botao de BACK!!!\n";
		}
		//botao de PLAY/PAUSE - no segundo ecra!! - TÁ!
		else if((x >= start_width_play_btn) &&
			(x <= start_width_play_btn + SMALL_BUTTON_WIDTH) &&
			(y >= ofGetHeight()*0.81) &&
			(y <= ofGetHeight()*0.81 + SMALL_BUTTON_HEIGHT)) 
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
		//botao de STOP - no segundo ecra!! - TÁ!
		else if((x >= temp_stop_btn_width) &&
			(x <= temp_stop_btn_width + SMALL_BUTTON_WIDTH) &&
			(y >= ofGetHeight()*0.81) &&
			(y <= ofGetHeight()*0.81 + SMALL_BUTTON_HEIGHT)) 
		{
			movie.stop();
			movie.setFrame(int(range_minimum_percentage*movie.getTotalNumFrames()));
			redraw_frame_flag = true;
			video_playing = false;

			img_array.clear();

			cout << "botao de STOP HUEHUHE!!! min_frame: " << int(range_minimum_percentage*movie.getTotalNumFrames()) << "\n";
		}
		//botao para ver a GALERIA - no segundo ecra!! - TÁ!
		else if((x >= ofGetWidth()*0.92 - GALLERY_BUTTON_WIDTH) &&
			(x <= ofGetWidth()*0.92) && (y >= ofGetHeight()*0.81) &&
			(y <= ofGetHeight()*0.81 + SMALL_BUTTON_HEIGHT)) 
		{
			cout << "botão da galeria!!!!!!!!!!!!!!111!!!!" << "\n";
			gallery_screen = true;
			video_playing = false;
			play_video_screen = false;

			max_pages = ceil(double(img_array.size())/double(MAX_ITEMS_PER_PAGE));
			gallery_selected_page = 1;

			gui1->toggleVisible(); //esconde
			gui3->toggleVisible(); //esconde
			gui4->toggleVisible(); //esconde
			gui5->toggleVisible(); //mostra
			resetValues();
		}
	}

	//terceiro ecra - galeria de imagens
	else if(gallery_screen) {
		//botao de back - no terceiro ecra, o da galeria de imagens!! - NÃO TÁ!
		if((x >= ofGetWidth()*0.2) && (x <= ofGetWidth()*0.2 + SMALL_BUTTON_WIDTH) &&
			(y >= ofGetHeight()*0.86) && (y <= ofGetHeight()*0.86 + SMALL_BUTTON_HEIGHT)) 
		{
			gallery_screen = false;
			play_video_screen = true;
			load_video = true;
			choose_video_and_range_screen = false;

			gui1->toggleVisible(); //mostra
			gui3->toggleVisible(); //mostra
			gui4->toggleVisible(); //mostra 
			gui5->toggleVisible(); //esconde
		
			cout << "botao de BACK TERCEIRO ECRA!!!\n";
		}
		//botao pagina anterior - TÁ!
		else if((x >= ofGetWidth()*0.5 - SMALL_BUTTON_WIDTH - SMALL_INTERVAL/2) && 
			(x <= ofGetWidth()*0.5 + SMALL_INTERVAL) &&
			(y >= ofGetHeight()*0.86) && (y <= ofGetHeight()*0.86 + SMALL_BUTTON_HEIGHT)) 
		{		
			if(gallery_selected_page > 1)
				gallery_selected_page--;
			cout << "botao PAGINA ANTERIOR!\n";
		}
		//botao pagina seguinte - TÁ!
		else if((x >= ofGetWidth()*0.5 + SMALL_INTERVAL) && 
			(x <= ofGetWidth()*0.5 + SMALL_BUTTON_WIDTH + SMALL_INTERVAL/2) &&
			(y >= ofGetHeight()*0.86) && (y <= ofGetHeight()*0.86 + SMALL_BUTTON_HEIGHT)) 
		{		
			cout << "IMG_ARRAY.SIZE(): " << img_array.size() << "\n";
			cout << "MAX_ITEMS_PER_PAGE: " << MAX_ITEMS_PER_PAGE << "\n";
			cout << "ceil da cena: " << max_pages << "\n";

			if(gallery_selected_page < max_pages)
				gallery_selected_page++;

			cout << "[curr/max]: [" << gallery_selected_page << "/" << max_pages <<
				"]; botao dPAGINA SEGUINTE!!!\n";
		}
		//botao para gravar imagens para o disco - TÁ!
		else if((x >= ofGetWidth()*0.73) && 
			(x <= ofGetWidth()*0.73 + SMALL_BUTTON_WIDTH) &&
			(y >= ofGetHeight()*0.86) && (y <= ofGetHeight()*0.86 + SMALL_BUTTON_HEIGHT)) 
		{		
			cout << "botao para gravar as imagens pa disco!!!\n";

			int i;
			for(i = 0; i < img_array.size(); i++) {
				ofImage img = img_array[i];
				ofSaveImage(img.getPixelsRef(), "temp/img"+ofToString(i)+".png");
			}
		}
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

void ofApp::applyFiltersToFrame(ofImage img2){

	can_update_frame = false;

	nr_edges = 0;
	match_object = 0;
	contrastVal = 0;
	hue_total = 0;

	int i = 0;
	int j = 0;
	int count = 0;
	float value_max = -1;
	float value_min = 100;

	//DUAS IMAGENS IGUAIS, UMA A CORES OUTRA A P/B
	ofImage image_colorful;
	ofImage image_grayscale;
	image_grayscale.setFromPixels(img2.getPixels(), img2.getWidth(), img2.getHeight(), OF_IMAGE_GRAYSCALE, true);
	image_colorful.setFromPixels(img2.getPixels(), img2.getWidth(), img2.getHeight(), OF_IMAGE_COLOR, true);
	Image img = Image(image_colorful.getPixels(), image_grayscale.getPixels(), image_colorful.getWidth(), image_colorful.getHeight());

	sliders_dominant_color = img.calcColor(red, green, blue);
	
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
				count++; //TODO: QUÉSTA MERDA!!!?????
			}
		}
	}

	//TODO: A CENA DO GABOR ESTÁ A DAR VALORES ENTRE 0.039 E 0.042!!!!!!
	if(gabor_enabled) {
		gabor_value = img.calculateTexture();
		cout << "GABOR (%): " << gabor_value*100 << "\n";
	}

	//normalização dos dados :)
	nr_edges /= i*j;
	nr_edges *= 100;
	contrastVal /= i*j; 
	mean_luminance /= i*j;
	hue_total /= i*j;

	cout << radio_button_position2 << "\n";
	cout << "[edges] sliders - filter: " << number_of_edges << " - " << nr_edges << "\n";

	//PATTERN MATCHING - sift/surf
	if(object_loaded && object_match_enabled){
		match_object = img.match(objFinder);
		cout << "matching: " << match_object << "\n";
	}

	bool result = saveFrame();
	if(result)
		img_array.push_back(img2);

	can_update_frame = true;
}

string ofApp::getFilterString(bool var) {
	if(var)
		return "[X]";
	else
		return "[ ]";
}

bool ofApp::saveFrame() {
	bool result = false;

	//o range pretendido é para valores ACIMA do definido nos sliders
	if(radio_button_position == ABOVE) 
	{
		//se filtro ativado, ver se valores estao no alcance
		if(luminance_enabled) { //luminancia
			if(mean_luminance > luminance) //valores estao fora, retorna logo false
				result = true;
			else
				return false;
		}
		if(people_enabled) { //contagem de pessoas
			if(nr_people > number_of_people) 
				result = true;
			else
				return false;
		}
		if(dominant_color_enabled) { //cor dominante - hue
			if(hue_total > sliders_dominant_color)
				result = true;
			else
				return false;
		}
		if(contrast_enabled) { //contraste
			if(contrastVal > contrast) 
				result = true;
			else
				return false;
		}
		if(object_loaded) { //pattern matching - sift
			if(match_object > number_of_objects)
				result = true;
			else
				return false;
		}
		if(gabor_filter) { //texturas - gabor
			if(gabor_value > gabor_filter) 
				result = true;
			else
				return false;
		}
		if(radio_button_position2 != NONE) { //contornos
			if(nr_edges > number_of_edges) 
				result = true;
			else
				return false;
		}
	}
	//o range pretendido é para valores ABAIXO do definido nos sliders
	else if(radio_button_position == BELOW) {
		//se filtro ativado, ver se valores estao no alcance
		if(luminance_enabled) { //luminancia
			if(mean_luminance < luminance) //valores estao fora, retorna logo false
				result = true;
			else
				return false;
		}
		if(people_enabled) { //contagem de pessoas
			if(nr_people < number_of_people) 
				result = true;
			else
				return false;
		}
		if(dominant_color_enabled) { //cor dominante - hue
			if(hue_total < sliders_dominant_color)
				result = true;
			else
				return false;
		}
		if(contrast_enabled) { //contraste
			if(contrastVal < contrast) 
				result = true;
			else
				return false;
		}
		if(object_loaded) { //pattern matching - sift
			if(match_object < number_of_objects) 
				result = true;
			else
				return false;
		}
		if(gabor_filter) { //texturas - gabor
			if(gabor_value < gabor_filter) 
				result = true;
			else
				return false;
		}
		if(radio_button_position2 != NONE) { //contornos
			if(nr_edges < number_of_edges) 
				result = true;
			else
				return false;
		}
	}
	//o range pretendido é para valores À VOLTA do definido nos sliders
	else if(radio_button_position == RANGE) {

		//se filtro ativado, ver se valores estao no alcance
		if(luminance_enabled) { //luminancia -> [-10;10]
			if(mean_luminance >= luminance-10 && mean_luminance <= luminance+10)
				result = true;
			else
				return false;
		}
		if(people_enabled) { //contagem de pessoas -> [-3;3]
			if(nr_people >= number_of_people-3 && nr_people <= number_of_people+3) 
				result = true;
			else
				return false;
		}
		if(dominant_color_enabled) { //cor dominante - hue -> [-20;20]
			if(hue_total >= sliders_dominant_color-20 && hue_total <= sliders_dominant_color+20)
				result = true;
			else
				return false;
		}
		if(contrast_enabled) { //contraste -> [-0.5;0.5]
			if(contrastVal >= contrast-0.5 && contrastVal <= contrast+0.5) 
				result = true;
			else
				return false;
		}
		if(object_loaded) { //pattern matching - sift -> [-5;5]
			if(match_object >= number_of_objects-5 && match_object <= number_of_objects+5) 
				result = true;
			else
				return false;
		}
		if(gabor_filter) { //texturas - gabor -> [-2;2]
			if(gabor_value >= gabor_filter-2 && gabor_value <= gabor_filter+2) 
				result = true;
			else
				return false;
		}
		if(radio_button_position2 != NONE) { //contornos -> [-5;5]
			if(nr_edges >= number_of_edges-5 && nr_edges <= number_of_edges+5) 
				result = true;
			else
				return false;
		}

	}

	// ora bem, guardamos a frame ou nao?
	if(result) {
		cout << "FRAME PASSOU, GUARDAR!\n";
		return true;
	}
	else
		return false;
}
