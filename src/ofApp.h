#pragma once

#include "ofMain.h"
#include "ofxPlaymodes.h"
#include "phasorClass.h"
#include "baseOscillator.h"
#include "mapper.h"
#include "oscillatorBank.h"

//#define PM_USE_SYSTEM_GRABBER
//#define PM_USE_PS3EYE
#define PM_USE_DECKLINK
//#define PM_USE_HEADER_RENDERER
#define PM_USE_MULTIX_RENDERER

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
    ofVec2f						grabberResolution;
    float						grabberAspectRatio;

#ifdef PM_USE_PS3EYE
    ofxPm::VideoGrabberPS3Eye         grabber;

    bool PS3_autoWB;
    bool PS3_autoGain;
    int PS3_exposure;
    int PS3_hue;
#endif

#ifdef PM_USE_SYSTEM_GRABBER
    ofxPm::VideoGrabber               grabber;
#endif

#ifdef PM_USE_DECKLINK
    ofxPm::VideoGrabberDeckLink grabber;
#endif
    
    /// BUFFER
    
    ofxPm::VideoBuffer			vBuffer;
    ofxPm::VideoRate			vRate;

    /// RENDERER
    ofxPm::BasicVideoRenderer	vRendererGrabber,vRendererBuffer;

    /// NPORMAL HEADER
#ifdef PM_USE_HEADER_RENDERER
    ofxPm::VideoHeader              vHeader;
    ofxPm::BasicVideoRenderer   vRendererHeader;
    void                        changedHeaderDelay(float &f);

#endif
    
#ifdef PM_USE_MULTIX_RENDERER
    ofxPm::MultixRenderer       vMultixRenderer;
    ofParameter<vector<float>>  guiMultixValues;
    ofParameter<float>          guiMultixOffset;
    ofParameter<bool>           guiMultixMinMaxBlend;
    void                        changedMultixValues(vector<float> &f);
    void                        changedMultixOffset(float &f);
    void                        changedMinMaxBlend(bool &b);
#endif
    
    /// GENERAL
    
    ofSoundStream soundStream;
    
    /// GUI?
    ofParameterGroup*   parametersPlaymodes;
    ofParameter<bool>   guiBufferIsRecording;
    ofParameter<float>  guiHeaderDelay;
    
    // LISTENERS FUNCTIONS
    void                changedBufferIsRecording(bool &b);
    
    //--------- PHASORS

    vector<phasorClass*> phasors;
    vector<baseOscillator*>  oscillators;
    vector<oscillatorBank*>  oscillatorBanks;
    vector<mapper*> mappers;

    // phasors
    void    audioRateTrigger(int bufferSize);
    
    bool drawFullScreen;

    // PS3EYE

};
