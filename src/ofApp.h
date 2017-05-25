#pragma once

#include "ofMain.h"
#include "ofxPlaymodes.h"

class ofApp : public ofBaseApp{

	public:
    
    void setup();
    void update();
    void draw();

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

    /// GRABBER
    float						grabFPS;
    ofxPm::VideoGrabber         grabber;
    ofVec2f						grabberResolution;
    float						grabberAspectRatio;

    /// BUFFER
    ofxPm::VideoBuffer			vBuffer;
    ofxPm::VideoRate			vRate;

    bool                        isRecording;

    /// HEADER
    ofxPm::VideoHeader          vHeader;

    /// RENDERER
    ofxPm::BasicVideoRenderer		vRendererGrabber,vRendererBuffer,vRendererHeader;


    /// GENERAL
    
};
