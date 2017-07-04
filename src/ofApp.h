#pragma once

#include "ofMain.h"
#include "ofxPlaymodes.h"
#include "phasorClass.h"
#include "baseOscillator.h"
#include "mapper.h"
#include "oscillatorBank.h"

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
    
    void audioIn(float * input, int bufferSize, int nChannels);
    ofEvent<int> audioRateTriggerEvent;

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
    ofxPm::BasicVideoRenderer	vRendererGrabber,vRendererBuffer,vRendererHeader;
    ofxPm::MultixRenderer       vMultixRenderer;

    /// GENERAL
    Poco::Timestamp             testTS;
    Poco::Timestamp             tsStop;
    Poco::Timestamp::TimeDiff   tdiff;
    
    ofSoundStream soundStream;
    
    /// GUI?
    ofParameterGroup*    parametersHeader;
    ofParameter<float>  guiHeaderDelay;
    ofParameter<float>  guiHeaderIn;
    ofParameter<float>  guiHeaderOut;
    ofParameter<bool>   guiHeaderIsPlaying;
    ofParameter<vector<float>> guiMultixValues;
    
    // LISTENERS FUNCTIONS
    void                changedHeaderIsPlaying(bool &b);
    void                changedHeaderIn(float &f);
    void                changedHeaderOut(float &f);
    void                changedHeaderDelay(float &f);
    void                changedMultixValues(vector<float> &f);
    
    
    //--------- PHASORS
    //vector<phasorClass*> phasors;
    vector<phasorClass*> phasors;
    vector<baseOscillator*>  oscillators;
    vector<oscillatorBank*>  oscillatorBanks;
    vector<mapper*> mappers;

    // phasors
//    vector<phasorClass*> getPhasors(){return phasors;};
    void    audioRateTrigger(int bufferSize);
    
    bool drawFullScreen;



};
