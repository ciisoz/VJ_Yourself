#include "ofApp.h"

#include "parametersControl.h"

int NUM_PHASORS = 1;
// REMEMBER !! to change also the param in VideoBuffer.cpp line 90 :: parameters->add(paramNumHeaders.set("Num Headers",0,0,XXXXX));
int numCopies = 480;
int numOscillatorBanks = numCopies;
//--------------------------------------------------------------
void ofApp::setup(){

    ofDisableArbTex();
    drawFullScreen = false;
    grabFPS = 60;
    
    ofSetEscapeQuitsApp(false);
    
    /// GRABBER
    /////////////
    ofxPm::VideoGrabber vg;
    vector<ofVideoDevice> devices = vg.listDevices();
    cout << "-- Available system video grabbers : " << endl;
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }

    grabFPS = 60;
    grabberResolution = ofVec2f(640,480);
    grabberAspectRatio = grabberResolution.x / grabberResolution.y;

    grabberPS3.setFps(grabFPS);
    //grabber.ofBaseVideoGrabber::setDesiredFrameRate(grabFPS);
    grabberPS3.setDeviceID(0);
    grabberPS3.initGrabber(grabberResolution.x,grabberResolution.y);

    // NODE BASED ONE
    videoGrabberNode.initGrabber(grabberResolution.x, grabberResolution.y);

    ///////////////
    /// PIPELINE
    ///////////////
    
    videoRendererNode.setup();

    vBuffer.setupNodeBased(numCopies,true);
    vBuffer2.setupNodeBased(numCopies,true);
    
    multixFilter.setupNodeBased();
    bool b=true;
    multixFilter.setMinmaxBlend(b);
    videoHeaderNode.setupNodeBased();
    fx.setupNodeBased();
    gradient.setupNodeBased();
    feedback.setupNodeBased();
    
    copiesOverflowBuffer=false;

    // GENERATOR GUI related ...
    /////////////////////////////////
    
    parametersControl::getInstance().setup();
    bpmControl::getInstance().setup();
    
    ofAddListener(bpmControl::getInstance().bpmChanged, this, &ofApp::changedBPM);
    
    for(int i=0;i<NUM_PHASORS;i++)
    {
        phasorClass* p = new phasorClass(i+1,ofPoint(660+300*i,0));
        phasors.push_back(p);
        phasors[i]->getParameterGroup()->getFloat("BPM") = 60.0;
        phasors[i]->getParameterGroup()->getFloat("Bounce") = false;
        
        baseOscillator* o = new baseOscillator(i+1,true,ofPoint(660+300*i,300));
        oscillators.push_back(o);
        
        
        //int nOscillators, bool gui, int _bankId, ofPoint pos) : baseIndexer(nOscillators
        oscillatorBank* ob = new oscillatorBank(numOscillatorBanks,true,i+1,ofPoint(660+300*i,600));
        oscillatorBanks.push_back(ob);
        
        mapper* m = new mapper(i+1,ofPoint(960+300,200*i));
        mappers.push_back(m);
    }
    

    // PRINT INFOS
    cout << "Buffer : Max Size : " << vBuffer.getMaxSize() << endl;
    cout << "Buffer : FPS : " << vBuffer.getFps() << endl;
    cout << "Buffer : Total Time : " << vBuffer.getTotalTime() << endl;
    cout << "Buffer : Total Frames : " << vBuffer.getTotalFrames() << endl;
    
    ofBackground(30);
    
    soundStream.setup(this, 0, 2, 44100, 512, 4);
}
////--------------------------------------------------------------
//void ofApp::changedBufferIsRecording(bool &b)
//{
//    cout << "Buffer Recording set to : " << b <<endl;
//
//    if(b)
//    {
//        vBuffer.resume();
//    }
//    else
//    {
//        vBuffer.stop();
//    }
//    
//    
//}

//--------------------------------------------------------------
void ofApp::changedBPM(float &_f)
{
    int i;
#ifdef PM_USE_MULTIX_RENDERER
    changedNumCopies(i);
#endif
}

//--------------------------------------------------------------
void ofApp::update()
{
    grabberPS3.update();
    videoGrabberNode.update();
//    phasor->getParameterGroup()->getFloat("Phasor Monitor");
    for(int i=0;i<NUM_PHASORS;i++)
    {
        phasors[i]->getPhasor();
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    
    if(copiesOverflowBuffer)
    {
        ofBackground(255,0,0);
    }
    else{
        ofBackground(25);
    }

    //DrawScreenArea
    ofSetColor(0);
    ofDrawRectangle(0,0,grabberResolution.x, grabberResolution.y+20);
    
    
    ofSetColor(255);
    videoRendererNode.draw(0,0);
    
    float actualFPS = ofGetFrameRate();
    if(actualFPS>=60) ofSetColor(0,255,0);
    else if(actualFPS>=30) ofSetColor(255,128,0);
    else ofSetColor(255,0,0);
    ofDrawBitmapString(int(actualFPS), 10, 15);
    
//    ofSetColor(255);
//    ofDrawBitmapString("Frame[0] Timestamp : " + ofToString(vBuffer.getFirstFrameTimestamp().raw()),ofGetWidth()-350,50);
//    ofDrawBitmapString("testTS : " + ofToString(testTS.elapsed()),ofGetWidth()-350,75);
//    ofDrawBitmapString("tdiff : " + ofToString(tdiff),ofGetWidth()-350,100);
//    ofDrawBitmapString("StopTS : " + ofToString(tsStop.elapsed()),ofGetWidth()-350,105);
//    ofDrawBitmapString("StopTS R: " + ofToString(tsStop.raw()),ofGetWidth()-350,140);
    
 }

//-----------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels)
{
//    audioRateTrigger(bufferSize);
    
    for(int i=0;i<NUM_PHASORS;i++)
    {
        phasors[i]->audioIn(input,bufferSize,nChannels);
    }
    
     bpmControl::getInstance().audioIn(input, bufferSize, nChannels);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
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

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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

}
//------------------------------------------------------
void ofApp::audioRateTrigger(int bufferSize)
{
//    for(int i = phasors.size()-1 ; i >= 0 ; i--){
//        phasors[i]->audioIn(bufferSize);
//    }
    
    
}
//------------------------------------------------------
void ofApp::drawProgramWindow(ofEventArgs & args)
{
//        
//#ifdef PM_USE_MULTIX_RENDERER
//    ofSetColor(255);
//    ofSetColor(255);
//    ofxPm::VideoFrame vB = (ofxPm::VideoFrame) vBuffer.getVideoFrame(float(1.0));
//    if(!vB.isNull())
//    {
//        if(vB.getTextureRef().isAllocated()) vB.getTextureRef().draw(0,0,640,480);
//    }
//    //videoRendererMultix.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
//    ofSetColor(255,0,0);
//    ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
//    ofSetColor(255);
//#endif
//#ifdef PM_USE_HEADER_RENDERER
//        ofSetColor(255);
//    vRendererHeader.draw(0,0,ofGetWindowWidth(),ofGetWindowHeight());
//#endif
//
//    

}

//--------------------------------------------------------------
void ofApp::setupSecondScreen(){
    ofSetBackgroundColor(0,0,0);
}
