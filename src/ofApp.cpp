#include "ofApp.h"

#include "parametersControl.h"

int NUM_PHASORS = 2;
int numCopies = 60*12;
//--------------------------------------------------------------
void ofApp::setup(){

    ofDisableArbTex();
    drawFullScreen = false;
    grabFPS = 60;
    
#ifdef PM_USE_PS3EYE
    PS3_autoWB=false;
    PS3_autoGain=false;
    PS3_exposure=254;
    PS3_hue = 0;
#endif
    
    ofSetEscapeQuitsApp(false);

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
        oscillatorBank* ob = new oscillatorBank(numCopies,true,i+1,ofPoint(660+300*i,600));
        oscillatorBanks.push_back(ob);

        mapper* m = new mapper(i+1,ofPoint(960+300,200*i));
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
    grabberResolution = ofVec2f(640,480);
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
#ifdef PM_USE_PS3EYE
    grabber.setExposure(static_cast<uint8_t>( PS3_exposure));
#endif
    ///////////////
    /// PIPELINE
    ///////////////
    
    fx.setup(grabber);
    vBuffer.setup(fx, numCopies,true);
    //vRendererGrabber.setup(grabber);
    //vRendererBuffer.setup(vBuffer);

#ifdef PM_USE_HEADER_RENDERER
    vHeader.setup(vBuffer);
    vHeader.setDelayMs(0.0f);
    vRendererHeader.setup(vHeader);
#endif
    
#ifdef PM_USE_MULTIX_RENDERER

    copiesOverflowBuffer=false;

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
    parametersPlaymodes->setName("PLAYMODES VJYOURSELF 1");
    // FX GUI
    parametersPlaymodes->add(guiBufferIsRecording.set("Buffer Recording",true));
    parametersPlaymodes->add(guiLumaKeyThreshold.set("Luma Key Threshold", 0.0, 0.0, 1.0));
    parametersPlaymodes->add(guiLumaKeySmooth.set("Luma Key Smooth", 0.0, 0.0, 1.0));
    
    guiLumaKeyThreshold.addListener(this,&ofApp::changedLumaKeyThreshold);
    guiLumaKeySmooth.addListener(this,&ofApp::changedLumaKeySmooth);
    guiBufferIsRecording.addListener(this, &ofApp::changedBufferIsRecording);

    
#ifdef PM_USE_HEADER_RENDERER
    parametersPlaymodes->add(guiHeaderDelay.set("Header", 0.0, 0.0, 1.0));
    guiHeaderDelay.addListener(this,&ofApp::changedHeaderDelay);

#endif
    
#ifdef PM_USE_MULTIX_RENDERER

    parametersPlaymodes->add(guiMultixMinMaxBlend.set("Multix Blend Min/Max",true));
    parametersPlaymodes->add(guiMultixValues.set("Multix Values",guiMultixValues));
    
    parametersPlaymodes->add(guiLinearDistribution.set("Linear Distribution",true));
    parametersPlaymodes->add(guiNumCopies.set("Num Copies",1,1,vBuffer.getMaxSize()));
    parametersPlaymodes->add(guiBeatDiv.set("Offset Beat Div",1,1,32));
    parametersPlaymodes->add(guiBeatMult.set("Offset Beat Mult",1,1,32));

    guiNumCopies.addListener(this, &ofApp::changedNumCopies);
    guiBeatDiv.addListener(this,&ofApp::changedNumCopies);
    guiBeatMult.addListener(this,&ofApp::changedNumCopies);
    guiMultixValues.addListener(this, &ofApp::changedMultixValues);
    guiMultixMinMaxBlend.addListener(this, &ofApp::changedMinMaxBlend);


#endif
    
    parametersControl::getInstance().createGuiFromParams(parametersPlaymodes, ofColor::orange, ofPoint(350,500));
    parametersControl::getInstance().setSliderPrecision(1,"Multix Offset", 4);


    
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

//--------------------------------------------------------------
void ofApp::changedBPM(float &_f)
{
    int i;
#ifdef PM_USE_MULTIX_RENDERER
    changedNumCopies(i);
#endif
}

//--------------------------------------------------------------
void ofApp::changedLumaKeyThreshold(float &f)
{
    fx.setLumaThreshold(f);
}
//--------------------------------------------------------------
void ofApp::changedLumaKeySmooth(float &f)
{
    fx.setLumaSmooth(f);
}

#ifdef PM_USE_MULTIX_RENDERER
//--------------------------------------------------------------
void ofApp::changedNumCopies(int &_i)
{
    float gBPM = parametersControl::getInstance().getGlobalBPM();
    float BPMfactor = (float(guiBeatMult)/float(guiBeatDiv));
    float oneBeatMs = (60.0/gBPM)*1000;
    float oneCopyMs = oneBeatMs / BPMfactor;
    
    vector<float> vf;
    vMultixRenderer.setNumHeaders(numCopies);
    
    for(int i=0;i<guiNumCopies;i++)
    {
        if(guiLinearDistribution)
        {
            // in linear distribution, the copies are spaced equally a time/distance defined by BPM and beatMult/Div
            vf.push_back(i*oneCopyMs);
        }
        else
        {
            // non-linear distribution, the copies are distributed from 0 to the time expressed by BPM/Mult/Div.
            // And inside this period the copies are distributed by Generator
            // TO DO
        }
    }
    vMultixRenderer.updateValuesMs(vf);
    
    // Calculating "Overflow" it happens when we're trying to fetch a video frame that is out of bounds in the buffer
    float oneFrameMs = (1.0 / grabFPS) * 1000.0;
    float timeInBuffer = vBuffer.getMaxSize()*oneFrameMs;
    float timeInCopies = oneCopyMs*guiNumCopies;
    if(timeInCopies > timeInBuffer)
    {
        cout << "OVERFLOOOOW!! OneCopyMs : " << oneCopyMs << " // guiNumCopies : " << guiNumCopies << " = " << timeInCopies << " //  > " << timeInBuffer << endl;
        copiesOverflowBuffer = true;
    }
    else
    {
        copiesOverflowBuffer = false;
    }
}


//--------------------------------------------------------------
void ofApp::changedMultixValues(vector<float> &vf)
{    
    vMultixRenderer.updateValuesPct(vf);
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
    if(copiesOverflowBuffer)
    {
        ofBackground(255,0,0);
    }
    else{
        ofBackground(25);
    }
#endif
    
    ofSetColor(0);
    ofDrawRectangle(0,0,grabberResolution.x, grabberResolution.y+20);
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
    
    ofDrawBitmapString(int(actualFPS), grabberResolution.x-20, grabberResolution.y+15);
    
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
        cout << PS3_exposure << endl;
    }
    else if(key=='E')
    {
        PS3_exposure = PS3_exposure - 5;
        grabber.setExposure(static_cast<uint8_t> (PS3_exposure));
        cout << PS3_exposure << endl;
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
