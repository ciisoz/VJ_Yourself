#pragma once

#include "ofMain.h"
#include "ofxPlaymodes.h"

#include "phasorClass.h"
#include "baseOscillator.h"
#include "mapper.h"
#include "oscillatorBank.h"

//#define PM_USE_SYSTEM_GRABBER
#define PM_USE_PS3EYE
//#define PM_USE_DECKLINK

//#define PM_USE_HEADER_RENDERER
//#define PM_USE_MULTIX_RENDERER

// XXX : poor performanace at fullHD + 240 copies of multix in Decklink grabbing.
// TO DO : try what if i disconnect the DeckLink color conv. shader in terms of performance at FHd, 60 fps, Decklink capture... if it helps performance, implement a new multix that renders the copies from Decklink without shader to an fbo then draw this fbo with the ColorConversion shader from DeckLink... does it work ?
// TO DO : when drawing color modes in multix ... mode 1 or 2 seems different opacity results ...?À

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
    float                           grabFPS;
    ofVec2f                         grabberResolution;
    float                           grabberAspectRatio;

    ofxPm::VideoGrabberPS3Eye       grabberPS3;
    ofxPm::VideoGrabberNodeBased    videoGrabberNode;

    #ifdef PM_USE_DECKLINK
        ofxPm::VideoGrabberDeckLink grabber;
    #endif
    
    /// BUFFER
    ofxPm::VideoBuffer              vBuffer;
    ofxPm::VideoBuffer              vBuffer2;

    // VIDEHEADER
    ofxPm::VideoHeaderNodeBased     videoHeaderNode;

    // VIDEORENDERER
    ofxPm::VideoRendererNodeBased   videoRendererNode;

    // SHADER FX
    ofxPm::VideoTestShaderFX        fx;
    ofxPm::GradientEdgesFilter      gradient;
    ofxPm::FeedbackFilterNodeBased  feedback;

    // MULTIX FX
    ofxPm::MultixFilter             multixFilter;

    
    bool                            copiesOverflowBuffer;
//    ofxPm::VideoTrioRenderer        videoRendererMultix;
    
    
    /// GENERAL
    ofSoundStream               soundStream;
    void                        audioIn(float * input, int bufferSize, int nChannels);
    void                        drawProgramWindow(ofEventArgs & args);
    bool                        drawFullScreen;
    void                        setupSecondScreen();

    // LISTENERS FUNCTIONS
    void                        changedBPM(float &_f);
//    void                        changedBufferIsRecording(bool &b);
    
    // GENERATOR STUFF
    /////////////////////
    vector<phasorClass*>        phasors;
    vector<baseOscillator*>     oscillators;
    vector<oscillatorBank*>     oscillatorBanks;
    vector<mapper*> mappers;
    
    void                        audioRateTrigger(int bufferSize);
    ofEvent<int>                audioRateTriggerEvent;
};
