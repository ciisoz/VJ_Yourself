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
#define PM_USE_MULTIX_RENDERER

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
    float						grabFPS;
    ofVec2f						grabberResolution;
    float						grabberAspectRatio;

    #ifdef PM_USE_PS3EYE
        ofxPm::VideoGrabberPS3Eye       grabber;
        bool PS3_autoWB;
        bool PS3_autoGain;
        int PS3_exposure;
        int PS3_hue;
    #endif

    #ifdef PM_USE_SYSTEM_GRABBER
        ofxPm::VideoGrabber             grabber;
    #endif

    #ifdef PM_USE_DECKLINK
        ofxPm::VideoGrabberDeckLink     grabber;
    #endif
    
    /// BUFFER
    ofxPm::VideoBuffer              vBuffer;

    // FX LUMA
    ofxPm::VideoTestShaderFX        fx;
    ofParameter<float>              guiLumaKeyThreshold;
    ofParameter<float>              guiLumaKeySmooth;
    void                            changedLumaKeyThreshold(float &f);
    void                            changedLumaKeySmooth(float &f);

    // GRADIENT
    ofxPm::GradientEdgesFilter      gradient;
    ofParameter<float>              guiGradientWidth;
    void                            changedGradientWidth(float &f);
    
#ifdef PM_USE_HEADER_RENDERER
    /// HEADER
    ofxPm::VideoHeader              vHeader;
    ofxPm::VideoRenderer            vRendererHeader;
    
    ofParameter<float>              guiHeaderDelay;
    void                            changedHeaderDelay(float &f);
    ofxPm::VideoTrioRenderer        videoTrioRender;
    
#endif
    
#ifdef PM_USE_MULTIX_RENDERER
    ofParameter<string>         guiTitleMultix;
    ofParameter<vector<float>>  guiMultixValues;
    ofParameter<bool>           guiMultixMinMaxBlend;
    ofParameter<int>            guiBeatMult;
    ofParameter<int>            guiBeatDiv;
    ofParameter<int>            guiNumCopies;
    ofParameter<bool>           guiMultixLinearDistribution;
    ofParameter<int>            guiMultixOpacityMode;

    void                        changedMultixValues(vector<float> &f);
    void                        changedMinMaxBlend(bool &b);
    void                        changedNumCopies(int &i);
    void                        changedMultixOpacityMode(int &i);

    bool                        copiesOverflowBuffer;

    ofxPm::VideoTrioRenderer        videoRendererMultix;
    ofxPm::MultixFilter                multixFilter;
    
    
#endif
    
    /// GENERAL
    ofSoundStream               soundStream;
    bool                        drawFullScreen;
    ofMutex                     mutex;
    void                        setupSecondScreen();
    ofParameter<string>         guiTitleMain;

    /// GUI?
    ofParameterGroup*           parametersPlaymodes;
    ofParameter<bool>           guiBufferIsRecording;
    
    // LISTENERS FUNCTIONS
    void                        changedBufferIsRecording(bool &b);
    void                        changedBPM(float &_f);
    
    // GENERATOR STUFF
    /////////////////////
    vector<phasorClass*>        phasors;
    vector<baseOscillator*>     oscillators;
    vector<oscillatorBank*>     oscillatorBanks;
    vector<mapper*> mappers;
    
    void                        audioRateTrigger(int bufferSize);
    ofEvent<int>                audioRateTriggerEvent;
    
    void                        audioIn(float * input, int bufferSize, int nChannels);
    void                        drawProgramWindow(ofEventArgs & args);


};
