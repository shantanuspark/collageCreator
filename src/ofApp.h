#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "ofMain.h"
#include "ofxGui.h"

class Image {

public:
	ofImage image;
	ofVec2f trans, scale;
	float rot;
	bool isSelected;
	void draw(bool isSelectMode = false, int index = 0);
	void drawSelectionGrid();

	Image();

};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
		void doMouseRotation(int x, int y);
		void doMouseTransform(int x, int y);
		void doMouseScale(int x, int y);
		void deleteImage();
		void pushImageUp();
		void pushImageDown();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void processSelection(int x, int y);
		void renderSelection();
		void playMusic();
		void stopMusic();
		void addImage();
		string wstrtostr(const std::wstring &wstr);

		ofSoundPlayer sound;

		ofxPanel gui;
		ofxButton add;
		ofxButton play;
		ofxButton stop;
		ofxFloatSlider volume;
		ofxFloatSlider speed;

	private:
		Image *selectedImage;
		vector<Image*> images;
		ofVec2f		lastScallingMouse;
		ofVec2f		lastMouse;
		bool		ctrlKeyDown,scaleMode=false;
		ofImage     finalImage;
		ofFile savedToFile;
		int			selectedGridPoint;
};
