#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofEnableAlphaBlending();
	selectedImage = NULL;

	gui.setup("Menu");
	gui.add(add.setup("Add Image"));
	gui.add(play.setup("Play music to boost creativity"));
	gui.add(stop.setup("Stop Music"));
	gui.add(volume.setup("Volume", 0.5, 0, 1));
	gui.add(speed.setup("Tempo", 1, -3, 3));

	play.addListener(this, &ofApp::playMusic);
	stop.addListener(this, &ofApp::stopMusic);
	add.addListener(this, &ofApp::addImage);

	sound.load("audio.wav");
	sound.setLoop(true);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofBackground(0);
	ofFill();

	for (int i = 0; i < images.size(); i++) {
		images[i]->draw();
	}

	gui.draw();
	sound.setVolume(volume);
	sound.setSpeed(speed);
}

void ofApp::exit() {
	for (int i = 0; i < images.size(); i++) {
		delete images[i];
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch (key) {
	case OF_KEY_DEL:
		deleteImage();
		break;
	case OF_KEY_CONTROL:
		ctrlKeyDown = true;
		break;
	case OF_KEY_UP:
		pushImageUp();
		break;
	case OF_KEY_DOWN:
		pushImageDown();
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case OF_KEY_CONTROL:
		ctrlKeyDown = false;
		break;
	case 's':
	case 'S':
		finalImage.grabScreen(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
		finalImage.saveImage("final.jpg");
		break;
	}
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

	if (ctrlKeyDown) doMouseRotation(x, y);
	else if (scaleMode) { doMouseScale(x, y); }
	else doMouseTransform(x, y);

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	unsigned char res[4];
	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);

	switch (res[0]) {
	case 252:
		//xy scalling
		scaleMode = true;
		lastScallingMouse = ofVec2f(x, y);
		selectedGridPoint = res[1];
		return;
	case 253:
		//y scalling
		scaleMode = true;
		lastScallingMouse = ofVec2f(0, y);
		selectedGridPoint = res[1];
		return;
	case 254:
		//x scalling
		lastScallingMouse = ofVec2f(x, 0);
		scaleMode = true;
		selectedGridPoint = res[1];
		return;
	}

	renderSelection();
	processSelection(x, y);
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	if (scaleMode) scaleMode = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
	Image *imageObj = new Image();
	imageObj->trans.x = dragInfo.position.x;
	imageObj->trans.y = dragInfo.position.y;
	if (imageObj->image.load(dragInfo.files[0]) == true)
		images.push_back(imageObj);
	else {
		cout << "Can't load image: " << dragInfo.files[0] << endl;
		delete imageObj;
	}
}

void ofApp::processSelection(int x, int y) {
	unsigned char res[4];
	GLint viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &res);

	if (selectedImage) {
		selectedImage->isSelected = false;
		selectedImage = NULL;
	}

	if (res[0] > 0 && res[0] <= images.size()) {
		Image *image = images[res[0] - 1];
		image->isSelected = true;
		selectedImage = image;
		
		images.erase(images.begin() + (res[0] - 1));
		images.push_back(image);
	}

}

void ofApp::renderSelection() {
	ofBackground(0);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < images.size(); i++) {
		images[i]->draw(true, i);
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void ofApp::doMouseRotation(int x, int y) {

	// if there are no images stored in the list, just return
	//
	if (!selectedImage) return;

	float rotation;
	ofVec2f mouse = ofVec2f(x, y);
	ofVec2f delta = mouse - lastMouse;
	float dist = delta.length();

	// if mouse is moving from left to right, apply positive rotation
	// otherwise apply negative rotation.
	// 
	if (mouse.x < lastMouse.x)
		rotation = -dist;
	else
		rotation = dist;

	// store value of where the mouse was last for next entry
	// 
	lastMouse = mouse;

	// apply rotation to image (will be rotated to this value on next redraw)
	//
	selectedImage->rot += rotation;
}

void ofApp::doMouseScale(int x, int y) {
	
	//if there are no images stored in the list, just return
	if (!selectedImage) return;

	//scalling along y axis
	if (lastScallingMouse.x == 0) {
		if (selectedGridPoint == 1) {//if cursor is moving away from image
			selectedImage->scale.y = 1 + (y - lastScallingMouse.y)/100;
		}
		else {//if cursor is moving towards the image
			selectedImage->scale.y = 1 + (lastScallingMouse.y - y) / 100;
		}
	} //scalling along x axis
	else if (lastScallingMouse.y == 0) {
		if (selectedGridPoint == 0) {
			selectedImage->scale.x = 1 + (lastScallingMouse.x - x) / 100;
		}
		else {
			selectedImage->scale.x = 1 + (x - lastScallingMouse.x) / 100;
		}
	} //scalling along both x,y axis
	else {
		if (selectedGridPoint == 0) {
			selectedImage->scale.y = 1 + (-y + lastScallingMouse.y) / 100;
			selectedImage->scale.x = 1 + (-x + lastScallingMouse.x) / 100;
		}
		else if (selectedGridPoint == 1) {
			selectedImage->scale.y = 1 + (lastScallingMouse.y - y) / 100;
			selectedImage->scale.x = 1 + (-lastScallingMouse.x + x) / 100;
		}
		else if (selectedGridPoint == 2) {
			selectedImage->scale.y = 1 + (y - lastScallingMouse.y) / 100;
			selectedImage->scale.x = 1 + (-x + lastScallingMouse.x) / 100;
		}
		else {
			selectedImage->scale.y = 1 + (y - lastScallingMouse.y) / 100;
			selectedImage->scale.x = 1 + (x - lastScallingMouse.x) / 100;
		}
	}

}

void ofApp::doMouseTransform(int x, int y) {

	//if there are no images stored in the list, just return
	if (!selectedImage) return;

	selectedImage->trans = ofVec2f(x, y);
}

void ofApp::deleteImage()
{
	if (!selectedImage) {
		return;
	}
	int eraseIndex = 0;

	for (int i = 0; i < images.size(); i++) {
		if (images[i] == selectedImage) {
			eraseIndex = i;
		}
	}
	images.erase(images.begin() + eraseIndex);

	selectedImage->isSelected = false;
	selectedImage = NULL;
}

void ofApp::pushImageUp()
{
	if (!selectedImage)
		return;

	Image* i = images[0];
	images.erase(images.begin());
	images.push_back(i);
}

void ofApp::pushImageDown()
{
	if (!selectedImage)
		return;
	
	Image* i = images[images.size()-1];
	images.pop_back();
	images.insert(images.begin(), i);
}

void ofApp::playMusic()
{
	sound.play();
}

void ofApp::stopMusic()
{
	sound.stop();
}


void ofApp::addImage()
{

	char filename[MAX_PATH];
	wchar_t wtext[MAX_PATH];

	OPENFILENAME ofn;
	Image *imageObj = new Image();
	imageObj->trans.x = 100;
	imageObj->trans.y = 100;

	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = _T("PNG Files\0*.png\0JPG Files\0*.jpg\0");
	mbstowcs(wtext, filename, strlen(filename) + 1);
	ofn.lpstrFile = wtext;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = _T("Select an Image!");
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
	
	if (GetOpenFileName(&ofn))
	{
		if (imageObj->image.load(wstrtostr(ofn.lpstrFile)) == true)
				images.push_back(imageObj);
			else {
				cout << "Can't load image: " << &ofn.lpstrFile << endl;
				delete imageObj;
			}
	}
	else
	{
		delete imageObj;
	}
	
}

string ofApp::wstrtostr(const std::wstring & wstr)
{
	std::string strTo;
	char *szTo = new char[wstr.length() + 1];
	szTo[wstr.size()] = '\0';
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
	strTo = szTo;
	delete[] szTo;
	return strTo;
}


Image::Image()
{
	trans.x = 0;
	trans.y = 0;
	rot = 0;
	scale.x = 1;
	scale.y = 1;
	isSelected = false;
}

void Image::draw(bool isSelectMode, int index)
{
	ofPushMatrix();
	ofTranslate(trans);
	ofScale(scale);
	ofRotate(rot);
	if (isSelectMode) {
		ofFill();
		ofSetColor(index + 1, 0, 0);
		ofDrawRectangle(-image.getWidth() / 2.0, -image.getHeight() / 2.0,
			image.getWidth(), image.getHeight());
	}
	else {
		if (isSelected) {
			ofNoFill();
			ofSetColor(255, 0, 0);
			ofSetLineWidth(3);
			drawSelectionGrid();
		}
		ofSetColor(255, 255, 255, 255);
		image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
	}
	ofPopMatrix();
}

void Image::drawSelectionGrid() {
	ofDrawRectangle(-image.getWidth() / 2.0, -image.getHeight() / 2.0,
		image.getWidth(), image.getHeight());
	ofFill();
	//x scaling
	ofSetColor(254, 0, 0);
	ofDrawCircle(-image.getWidth() / 2.0, 0, 20);
	ofSetColor(254, 1, 0);
	ofDrawCircle(image.getWidth() / 2.0, 0, 20);
	//y scalling
	ofSetColor(253, 0, 0);
	ofDrawCircle(0, -image.getHeight() / 2.0, 20);
	ofSetColor(253, 1, 0);
	ofDrawCircle(0, image.getHeight() / 2.0, 20);
	//xy scalling
	ofSetColor(252, 0, 0);
	ofDrawCircle(-image.getWidth() / 2.0, -image.getHeight() / 2.0, 20);
	ofSetColor(252, 1, 0);
	ofDrawCircle(image.getWidth() / 2.0, -image.getHeight() / 2.0, 20);
	ofSetColor(252, 2, 0);
	ofDrawCircle(-image.getWidth() / 2.0, image.getHeight() / 2.0, 20);
	ofSetColor(252, 3, 0);
	ofDrawCircle(image.getWidth() / 2.0, image.getHeight() / 2.0, 20);
}
