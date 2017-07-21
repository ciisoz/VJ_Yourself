#include "ofApp.h"

#include "parametersControl.h"

int NUM_PHASORS = 2;
int numCopies = 60;
//--------------------------------------------------------------
void ofApp::setup(){

    drawFullScreen = false;
#ifdef PM_USE_PS3EYE
    PS3_autoWB=false;
    PS3_autoGain=false;
    PS3_exposure=120;
    PS3_hue = 0;
#endif
    
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
        
        
        //int nOscillators, bool gui, int _bankId, ofPoint pos) : baseIndexer(nOscillators
        oscillatorBank* ob = new oscillatorBank(numCopies,true,i,ofPoint(660+300*i,600));
        oscillatorBanks.push_back(ob);

        mapper* m = new mapper(i,ofPoint(960+300,200*i));
        mappers.push_back(m);
    }
    
    
    /// GRABBER
    /////////////
    vector<ofVideoDevice> devices = grabber.listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }

    grabFPS = 60;
    grabberResolution = ofVec2f(1920,1080);
    grabberAspectRatio = grabberResolution.x / grabberResolution.y;
    vector<ofVideoDevice> vL = grabber.listDevices();
    for(int i=0;i < vL.size();i++)
    {
        cout << "List Video Devices :: "<< i << " __ " << vL[i].deviceName << endl;
    }
    grabber.setFps(grabFPS);
    //grabber.ofBaseVideoGrabber::setDesiredFrameRate(grabFPS);
    grabber.setDeviceID(0);
    grabber.initGrabber(grabberResolution.x,grabberResolution.y);

    ///////////////
    /// PIPELINE
    ///////////////
    
    vBuffer.setup(grabber, 600,true);
    vRendererGrabber.setup(grabber);
    vRendererBuffer.setup(vBuffer);

#ifdef PM_USE_HEADER_RENDERER
    vHeader.setup(vBuffer);
    vHeader.setDelayMs(0.0f);
    vRendererHeader.setup(vHeader);
#endif
    
#ifdef PM_USE_MULTIX_RENDERER
    vMultixRenderer.setup(vBuffer,numCopies);
    vMultixRenderer.setDelayOffset(0.002f);
    vMultixRenderer.setMinmaxBlend(1);
    vMultixRenderer.setTint(ofColor(255,255,255,254));
#endif

    // PRINT INFOS
    cout << "Buffer : Max Size : " << vBuffer.getMaxSize() << endl;
    cout << "Buffer : FPS : " << vBuffer.getFps() << endl;
    cout << "Buffer : Total Time : " << vBuffer.getTotalTime() << endl;
    cout << "Buffer : Total Frames : " << vBuffer.getTotalFrames() << endl;
    
    ofBackground(0);
    
    soundStream.setup(this, 0, 2, 44100, 512, 4);
    
    /// GUI
    //////////
    parametersPlaymodes = new ofParameterGroup();
    
    parametersPlaymodes->setName("PLAYMODES VJYOURSELF");
    parametersPlaymodes->add(guiBufferIsRecording.set("Buffer Recording",true));
    parametersPlaymodes->add(guiHeaderDelay.set("Header", 0.0, 0.0, 1.0));
#ifdef PM_USE_MULTIX_RENDERER
    parametersPlaymodes->add(guiMultixOffset.set("Multix Offset",0.04,0.0,10.0));
    parametersPlaymodes->add(guiMultixMinMaxBlend.set("Multix Blend Min/Max",true));
    parametersPlaymodes->add(guiMultixValues.set("Multix Values[]",guiMultixValues));
#endif
    
    parametersControl::getInstance().createGuiFromParams(parametersPlaymodes, ofColor::orange, ofPoint(350,500));

    parametersControl::getInstance().setSliderPrecision(1,"Multix Offset", 4);
    // LISTENERS FUNCTIONS
    guiBufferIsRecording.addListener(this, &ofApp::changedBufferIsRecording);

#ifdef PM_USE_HEADER_RENDERER
    guiHeaderDelay.addListener(this,&ofApp::changedHeaderDelay);
#endif
    
#ifdef PM_USE_MULTIX_RENDERER
    guiMultixValues.addListener(this, &ofApp::changedMultixValues);
    guiMultixOffset.addListener(this, &ofApp::changedMultixOffset);
    guiMultixMinMaxBlend.addListener(this, &ofApp::changedMinMaxBlend);
#endif
    
}
//--------------------------------------------------------------
void ofApp::changedBufferIsRecording(bool &b)
{
    cout << "Buffer Recording set to : " << b <<endl;

    if(b)
    {
        vBuffer.resume();
    }
    else
    {
        vBuffer.stop();
    }

}

#ifdef PM_USE_MULTIX_RENDERER
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
void ofApp::changedMultixOffset(float &f)
{
    vMultixRenderer.setDelayOffset(f);
}
//--------------------------------------------------------------
void ofApp::changedMinMaxBlend(bool &b)
{
    vMultixRenderer.setMinmaxBlend(b);
}

#endif

#ifdef PM_USE_HEADER_RENDERER
//--------------------------------------------------------------
void ofApp::changedHeaderDelay(float &f)
{
    int buffSize = vBuffer.getMaxSize();
    float oneFrameMs = (1.0 / grabFPS) * 1000.0;
    //    cout << "Header Out Changed : MaxSize " << buffSize << " OneFrameMs " << oneFrameMs << endl;
    vHeader.setDelayMs(ofMap(guiHeaderDelay,0.0,1.0,0.0,buffSize*oneFrameMs));
}
#endif

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
    
#ifdef PM_USE_MULTIX_RENDERER
    if(guiMultixMinMaxBlend)
    {
        ofBackground(0);
    }
    else{
        ofBackground(255);
    }
#endif
    
    ofSetColor(255);
    
    //vRendererGrabber.draw(10,10,160,120);
    //vRendererHeader.draw(10+140+10,10,160,120);
    
    
    if(drawFullScreen)
    {
        ofPushMatrix();
        ofTranslate(960,540);
        ofRotate(180,0.0,1.0,0.0);
        ofTranslate(-960,-540);
#ifdef PM_USE_MULTIX_RENDERER
        vMultixRenderer.draw(0,0,1920,1080);
#endif
#ifdef PM_USE_HEADER_RENDERER
        vRendererHeader.draw(0,0,1920,1080);
#endif
        ofPopMatrix();
    }
    else
    {
        ofPushMatrix();
        ofTranslate(320,240);
        ofRotate(180,0.0,1.0,0.0);
        ofTranslate(-320,-240);
#ifdef PM_USE_MULTIX_RENDERER
        vMultixRenderer.draw(0,0,640,480);
#endif
#ifdef PM_USE_HEADER_RENDERER
        vRendererHeader.draw(0,0,640,480);
#endif

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
//    if(key==' ')
//    {
//        if(isRecording)
//        {
//            vBuffer.stop();
//        }
//        else
//        {
//            vBuffer.resume();
//        }
//        isRecording=!isRecording;
//    }
    if (key=='f')
    {
        drawFullScreen=!drawFullScreen;
    }
#ifdef PM_USE_PS3EYE
    else if(key=='g')
    {
        PS3_autoGain=!PS3_autoGain;
        grabber.setAutoGain(PS3_autoGain);
    }
    else if(key=='b')
    {
        PS3_autoWB=!PS3_autoWB;
        grabber.setAutoWhiteBalance(PS3_autoWB);
    }
    else if(key=='e')
    {
        PS3_exposure = PS3_exposure + 5;
        grabber.setExposure(static_cast<uint8_t>( PS3_exposure));
    }
    else if(key=='E')
    {
        PS3_exposure = PS3_exposure - 5;
        grabber.setExposure(static_cast<uint8_t> (PS3_exposure));
    }
    else if(key=='h')
    {
        PS3_hue = (PS3_hue + 5)%255;
        grabber.setHue(static_cast<uint8_t>( PS3_hue));
    }
    else if(key=='H')
    {
        PS3_hue = (PS3_hue - 5)%255;
        grabber.setHue(static_cast<uint8_t> (PS3_hue));
    }
#endif
//    else if(key=='m')
//    {
//        multixMinMax = !multixMinMax;
//        vMultixRenderer.setMinmaxBlend(multixMinMax);
//        if(multixMinMax)
//        {
//            vMultixRenderer.setTint(ofColor(255,255,255,254));
//        }
//        else
//        {
//            vMultixRenderer.setTint(ofColor(255,255,255,254));
//        }
//    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

#ifdef PM_USE_HEADER_RENDERER
    if(ofGetKeyPressed(int('d')))
    {
        float factor = (float(ofGetWidth()-x)/float(ofGetWidth())) * vBuffer.getMaxSize();
        vHeader.setDelayMs(33.33f * factor);
        cout << "Setting delay at " << ofToString(33.33f * factor) << endl;
    }
#endif
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
