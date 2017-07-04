#include "ofApp.h"

#include "parametersControl.h"

int NUM_PHASORS = 2;
int numCopies = 320;
//--------------------------------------------------------------
void ofApp::setup(){

    drawFullScreen = false;
    
    ofSetEscapeQuitsApp(false);

    // GENERATOR GUI related ...
    /////////////////////////////////
    
    parametersControl::getInstance().setup();
    bpmControl::getInstance().setup();
    
    for(int i=0;i<NUM_PHASORS;i++)
    {
        phasorClass* p = new phasorClass(i,ofPoint(660+300*i,0));
        phasors.push_back(p);
        phasors[i]->getParameterGroup()->getFloat("BPM") = 60.0;
        phasors[i]->getParameterGroup()->getFloat("Bounce") = false;
        
        baseOscillator* o = new baseOscillator(i,true,ofPoint(660+300*i,300));
        oscillators.push_back(o);
        
        mapper* m = new mapper(i,ofPoint(660+300*i,600));
        mappers.push_back(m);
        
        //int nOscillators, bool gui, int _bankId, ofPoint pos) : baseIndexer(nOscillators
        oscillatorBank* ob = new oscillatorBank(numCopies,true,0,ofPoint(990+300*i,300));
        oscillatorBanks.push_back(ob);
    }
    
    
    /// GRABBER
    /////////////
    isRecording=true;
    vector<ofVideoDevice> devices = grabber.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }

    grabFPS = 30;
    grabberResolution = ofVec2f(640,480);
    grabberAspectRatio = grabberResolution.x / grabberResolution.y;
    vector<ofVideoDevice> vL = grabber.listDevices();
    for(int i=0;i < vL.size();i++)
    {
        cout << "List Video Devices :: "<< i << " __ " << vL[i].deviceName << endl;
    }
    grabber.setFps(grabFPS);
    //grabber.ofBaseVideoGrabber::setDesiredFrameRate(grabFPS);
    grabber.setDeviceID(1);
    grabber.initGrabber(grabberResolution.x,grabberResolution.y);
    /// PIPELINE
    vBuffer.setup(grabber, 600,true);
    vHeader.setup(vBuffer);
    vHeader.setDelayMs(0.0f);

    vRendererGrabber.setup(grabber);
    vRendererBuffer.setup(vBuffer);
    vRendererHeader.setup(vHeader);

    vMultixRenderer.setup(vBuffer,numCopies);
    vMultixRenderer.setDelayOffset(0.04f);
    vMultixRenderer.setMinmaxBlend(1);
    vMultixRenderer.setTint(ofColor(255,255,255,254));

    
    //to do : si el trec peta? o petava ...
    //sleep(2);
    
    // PRINT INFOS
    cout << "Buffer : Max Size : " << vBuffer.getMaxSize() << endl;
    cout << "Buffer : FPS : " << vBuffer.getFps() << endl;
    cout << "Buffer : Total Time : " << vBuffer.getTotalTime() << endl;
    cout << "Buffer : Total Frames : " << vBuffer.getTotalFrames() << endl;
    
    ofBackground(0);
    
    soundStream.setup(this, 0, 2, 44100, 512, 4);
    
    /// GUI
    //////////
    parametersHeader = new ofParameterGroup();
    parametersHeader->setName("PLAYMODES");
    parametersHeader->add(guiHeaderIsPlaying.set("Loop",false));
    parametersHeader->add(guiHeaderDelay.set("Header", 0.0, 0.0, 1.0));
    parametersHeader->add(guiHeaderIn.set("In", 1.0, 0.0, 1.0));
    parametersHeader->add(guiHeaderOut.set("Out", 0.0, 0.0, 1.0));
    parametersHeader->add(guiMultixValues.set("Multix Values",guiMultixValues));
    
    parametersControl::getInstance().createGuiFromParams(parametersHeader, ofColor::orange);

        
    // LISTENERS FUNCTIONS
    guiHeaderIsPlaying.addListener(this, &ofApp::changedHeaderIsPlaying);
    guiHeaderDelay.addListener(this,&ofApp::changedHeaderDelay);
    guiHeaderIn.addListener(this,&ofApp::changedHeaderIn);
    guiHeaderOut.addListener(this,&ofApp::changedHeaderOut);
    guiMultixValues.addListener(this, &ofApp::changedMultixValues);
    
}
//--------------------------------------------------------------
void ofApp::changedHeaderIsPlaying(bool &b)
{
    cout << "Header Is Playing Changed to " << b <<endl;
//    vHeader.setPlaying(b);
}


//--------------------------------------------------------------
void ofApp::changedMultixValues(vector<float> &vf)
{
    //cout << "*" << endl;
//    for(int i=0;i<f.size();i++)
//    {
//        cout << f[i] << "," << endl;
//    }
    
    vMultixRenderer.updateValues(vf);
}
//--------------------------------------------------------------
void ofApp::changedHeaderIn(float &f)
{
    int buffSize = vBuffer.getMaxSize();
    float oneFrameMs = (1.0 / grabFPS) * 1000.0;
    //    cout << "Header Out Changed : MaxSize " << buffSize << " OneFrameMs " << oneFrameMs << endl;
    vHeader.setInMs(ofMap(guiHeaderIn,0.0,1.0,0.0,buffSize*oneFrameMs));
    
}
//--------------------------------------------------------------
void ofApp::changedHeaderOut(float &f)
{
    int buffSize = vBuffer.getMaxSize();
    float oneFrameMs = (1.0 / grabFPS) * 1000.0;
//    cout << "Header Out Changed : MaxSize " << buffSize << " OneFrameMs " << oneFrameMs << endl;
   vHeader.setOutMs(ofMap(guiHeaderOut,0.0,1.0,0.0,buffSize*oneFrameMs));
    
}

//--------------------------------------------------------------
void ofApp::changedHeaderDelay(float &f)
{
    int buffSize = vBuffer.getMaxSize();
    float oneFrameMs = (1.0 / grabFPS) * 1000.0;
    //    cout << "Header Out Changed : MaxSize " << buffSize << " OneFrameMs " << oneFrameMs << endl;
    vHeader.setDelayMs(ofMap(guiHeaderDelay,0.0,1.0,0.0,buffSize*oneFrameMs));
}


//--------------------------------------------------------------
void ofApp::update()
{
    grabber.update();
 
//    phasor->getParameterGroup()->getFloat("Phasor Monitor");
    for(int i=0;i<NUM_PHASORS;i++)
    {
        phasors[i]->getPhasor();
    }
    

}

//--------------------------------------------------------------
void ofApp::draw()
{
    
    ofSetColor(255);
    
    //vRendererGrabber.draw(10,10,160,120);
    //vRendererHeader.draw(10+140+10,10,160,120);
    
    if(drawFullScreen)
    {
        ofPushMatrix();
        ofTranslate(320,240);
        ofRotate(180,0.0,1.0,0.0);
        ofTranslate(-320,-240);
        vMultixRenderer.draw(0,0,1920,1080);
        ofPopMatrix();
    }
    else
    {
        ofPushMatrix();
        ofTranslate(320,240);
        ofRotate(180,0.0,1.0,0.0);
        ofTranslate(-320,-240);
        vMultixRenderer.draw(0,0,640,480);
        ofPopMatrix();
        
    }
    
    //vBuffer.draw();
    //vHeader.draw();
    
    
    float actualFPS = ofGetFrameRate();
    if(actualFPS>=60) ofSetColor(0,255,0);
    else if(actualFPS>=30) ofSetColor(255,128,0);
    else ofSetColor(255,0,0);
    
    ofDrawBitmapString(actualFPS, ofGetWidth()-25, 25);
    
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
        phasors[i]->audioIn(bufferSize);
    }
    
     bpmControl::getInstance().audioIn(input, bufferSize, nChannels);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if(key==' ')
    {
        if(isRecording)
        {
            vBuffer.stop();
        }
        else
        {
            vBuffer.resume();
        }
        isRecording=!isRecording;
    }
    else if(key=='f')
    {
        drawFullScreen=!drawFullScreen;
    }
    else if(key=='s')
    {
    }
    else if(key=='d')
    {
        testTS.update();
        testTS -= tdiff;
    }
    else if(key=='f')
    {
        bool b=true;
        //vHeader.getPhasors()[0]->resetPhasor(b);
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

    if(ofGetKeyPressed(int('d')))
    {
        float factor = (float(ofGetWidth()-x)/float(ofGetWidth())) * vBuffer.getMaxSize();
        vHeader.setDelayMs(33.33f * factor);
        cout << "Setting delay at " << ofToString(33.33f * factor) << endl;
    }
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
