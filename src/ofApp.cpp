#include "ofApp.h"

#include "parametersControl.h"

int NUM_PHASORS = 2;
// REMEMBER !! to change also the param in VideoBuffer.cpp line 90 :: parameters->add(paramNumHeaders.set("Num Headers",0,0,XXXXX));
int numCopies = 480;
int numOscillatorBanks = numCopies;
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
        oscillatorBank* ob = new oscillatorBank(numOscillatorBanks,true,i+1,ofPoint(660+300*i,600));
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

    // NODE BASED ONE
    videoGrabberNode.initGrabber(grabberResolution.x, grabberResolution.y);

#ifdef PM_USE_PS3EYE
    //grabber.setExposure(static_cast<uint8_t>( PS3_exposure));
#endif
    ///////////////
    /// PIPELINE
    ///////////////
    
    videoRendererNode.setup();

    
#ifdef PM_USE_HEADER_RENDERER
    
    fx.setup(grabber);
    gradient.setup(fx);
    //vBuffer.setup(gradient, numCopies,true);

    vHeader.setup(vBuffer);
    vHeader.setDelayMs(0.0f);
    vRendererHeader.setup(vHeader);
    videoTrioRender.setup(vHeader);
    
    
#endif
    
#ifdef PM_USE_MULTIX_RENDERER

    vBuffer.setupNodeBased(numCopies,true);
    vBuffer2.setupNodeBased(numCopies,true);
    
    multixFilter.setupNodeBased();
    videoHeaderNode.setupNodeBased();
    fx.setupNodeBased();
    gradient.setupNodeBased();
    
    copiesOverflowBuffer=false;
    
    bool b =true;
    multixFilter.setMinmaxBlend(b);
    videoRendererMultix.setup(multixFilter);
    
#endif

    // PRINT INFOS
    cout << "Buffer : Max Size : " << vBuffer.getMaxSize() << endl;
    cout << "Buffer : FPS : " << vBuffer.getFps() << endl;
    cout << "Buffer : Total Time : " << vBuffer.getTotalTime() << endl;
    cout << "Buffer : Total Frames : " << vBuffer.getTotalFrames() << endl;
    
    ofBackground(30);
    
    soundStream.setup(this, 0, 2, 44100, 512, 4);
    
    /// PM GUI
    //////////
    parametersPlaymodes = new ofParameterGroup();
    parametersPlaymodes->setName("PLAYMODES VJYOURSELF 1");
    
    // MAIN GUI _
    parametersPlaymodes->add(guiTitleMain.set("MAIN_label", " "));
    parametersPlaymodes->add(guiBufferIsRecording.set("Buffer Recording",true));
    // FX GUI
    parametersPlaymodes->add(guiLumaKeyThreshold.set("Luma Key Threshold", 0.0, 0.0, 1.0));
    parametersPlaymodes->add(guiLumaKeySmooth.set("Luma Key Smooth", 0.0, 0.0, 1.0));
    parametersPlaymodes->add(guiGradientWidth.set("Gradient Width", 0.0, 0.0, 1.0));
    
    guiLumaKeyThreshold.addListener(this,&ofApp::changedLumaKeyThreshold);
    guiLumaKeySmooth.addListener(this,&ofApp::changedLumaKeySmooth);
    guiBufferIsRecording.addListener(this, &ofApp::changedBufferIsRecording);
    guiGradientWidth.addListener(this,&ofApp::changedGradientWidth);
    
#ifdef PM_USE_HEADER_RENDERER
    parametersPlaymodes->add(guiHeaderDelay.set("Header", 0.0, 0.0, 1.0));
    guiHeaderDelay.addListener(this,&ofApp::changedHeaderDelay);

#endif
    
#ifdef PM_USE_MULTIX_RENDERER
    parametersPlaymodes->add(guiTitleMultix.set("MULTIX_label", " "));
    parametersPlaymodes->add(guiMultixMinMaxBlend.set("Multix Blend MinMax",true));
    parametersPlaymodes->add(guiMultixOpacityMode.set("Multix Color Mode",0,0,2));
    parametersPlaymodes->add(guiNumCopies.set("Num Copies",1,1,960));
    parametersPlaymodes->add(guiBeatDiv.set("Offset Beat Div",1,1,32));
    parametersPlaymodes->add(guiBeatMult.set("Offset Beat Mult",1,1,32));
    parametersPlaymodes->add(guiMultixLinearDistribution.set("Linear Distribution",true));
    parametersPlaymodes->add(guiMultixValues.set("Multix Values Vect",guiMultixValues));

//    guiNumCopies.addListener(this, &ofApp::changedNumCopies);
//    guiBeatDiv.addListener(this,&ofApp::changedNumCopies);
//    guiBeatMult.addListener(this,&ofApp::changedNumCopies);
//    guiMultixValues.addListener(this, &ofApp::changedMultixValues);
//    guiMultixMinMaxBlend.addListener(this, &ofApp::changedMinMaxBlend);
//    guiMultixOpacityMode.addListener(this,&ofApp::changedMultixOpacityMode);

#endif
    
    parametersControl::getInstance().createGuiFromParams(parametersPlaymodes, ofColor::orange, ofPoint(350,500));


    
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

void ofApp::changedGradientWidth(float &f)
{
    gradient.setGradientWidth(f);
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
//    float gBPM = parametersControl::getInstance().getGlobalBPM();
//    float BPMfactor = (float(guiBeatMult)/float(guiBeatDiv));
//    float oneBeatMs = (60.0/gBPM)*1000;
//    float oneCopyMs = oneBeatMs / BPMfactor;
//    
//    vector<float> vf;
//    multixFilter.setNumHeaders(guiNumCopies);
//    
//    for(int i=0;i<guiNumCopies;i++)
//    {
//        if(guiMultixLinearDistribution)
//        {
//            // in linear distribution, the copies are spaced equally a time/distance defined by BPM and beatMult/Div
//            vf.push_back(i*oneCopyMs);
//        }
//        else
//        {
//            // non-linear distribution, the copies are distributed from 0 to the time expressed by BPM/Mult/Div.
//            // And inside this period the copies are distributed by Generator
//            // TO DO
//            float outMin,outMax;
//            int oscillatorReIndex = int(ofMap(i,0,float(guiNumCopies),0,float(numOscillatorBanks)));
//            mutex.lock();
//            {
//                vf.push_back(ofMap(guiMultixValues.get()[oscillatorReIndex],0.0,1.0, 0.0, oneCopyMs));
//            }
//            mutex.unlock();
////            cout << "app : " << oscillatorReIndex << " // guiMultiValue : " << guiMultixValues.get()[oscillatorReIndex] << endl;
//        }
//    }
//    multixFilter.updateValuesMs(vf);
//    
//    // Calculating "Overflow" it happens when we're trying to fetch a video frame that is out of bounds in the buffer
//    float oneFrameMs = (1.0 / grabFPS) * 1000.0;
//    float timeInBuffer = vBuffer.getMaxSize()*oneFrameMs;
//    float timeInCopies = oneCopyMs*guiNumCopies;
//    if(timeInCopies > timeInBuffer)
//    {
//        cout << "OVERFLOOOOW!! OneCopyMs : " << oneCopyMs << " // guiNumCopies : " << guiNumCopies << " = " << timeInCopies << " //  > " << timeInBuffer << " // buffer Size : " << vBuffer.getMaxSize() << endl;
//        copiesOverflowBuffer = true;
//    }
//    else
//    {
//        copiesOverflowBuffer = false;
//    }
}

//--------------------------------------------------------------
void ofApp::changedMultixOpacityMode(int &i)
{
    multixFilter.setOpacityMode(i);
}

//--------------------------------------------------------------
void ofApp::changedMultixValues(vector<float> &vf)
{
    int i=guiNumCopies;
    changedNumCopies(i);
}
//--------------------------------------------------------------
void ofApp::changedMinMaxBlend(bool &b)
{
    multixFilter.setMinmaxBlend(b);
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
//    if(videoRendererNode.getLastFrameTexture().isAllocated())
//    {
        videoRendererNode.draw(0,0);
//        
//    }

    //vRendererGrabber.draw(10,10,160,120);
    //vRendererHeader.draw(10+140+10,10,160,120);
    
    
//    if(drawFullScreen)
//    {
//        ofPushMatrix();
//        ofTranslate(960,540);
//        ofRotate(180,0.0,1.0,0.0);
//        ofTranslate(-960,-540);
//#ifdef PM_USE_MULTIX_RENDERER
//        vMultixRenderer.draw(0,0,1920,1080);
//#endif
//#ifdef PM_USE_HEADER_RENDERER
//        vRendererHeader.draw(0,0,1920,1080);
//#endif
//        ofPopMatrix();
//    }
//    else
    {

        ofPushMatrix();
        ofVec2f moveToRotate = ofVec2f(grabberResolution.x,grabberResolution.y);
        ofTranslate(moveToRotate.x/2.0,moveToRotate.y/2.0);
        ofRotate(180,0.0,1.0,0.0);
        ofTranslate(-moveToRotate.x/2.0,-moveToRotate.y/2.0);

#ifdef PM_USE_MULTIX_RENDERER
        ofSetColor(255);
        //videoRendererMultix.draw(0,0,moveToRotate.x,moveToRotate.y);

#endif
#ifdef PM_USE_HEADER_RENDERER
        
        ofSetColor(255);
        vRendererHeader.draw(0,0,640,480);
        //videoTrioRender.draw(0,0,640,480);
#endif

        ofPopMatrix();
        
    }
    
    //vBuffer.draw();
    //vHeader.draw();
    
    
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
    else if(key=='h')
    {
        vector<float> v;
        for(int i=0;i<10;i++)
        {
            v.push_back(50*i);
        }
        multixFilter.updateValuesMs(v);
    }
//    else if(key=='e')
//    {
//        PS3_exposure = PS3_exposure + 5;
//        grabber.setExposure(static_cast<uint8_t>( PS3_exposure));
//        cout << PS3_exposure << endl;
//    }
//    else if(key=='E')
//    {
//        PS3_exposure = PS3_exposure - 5;
//        grabber.setExposure(static_cast<uint8_t> (PS3_exposure));
//        cout << PS3_exposure << endl;
//    }
//    else if(key=='h')
//    {
//        PS3_hue = (PS3_hue + 5)%255;
//        grabber.setHue(static_cast<uint8_t>( PS3_hue));
//    }
//    else if(key=='H')
//    {
//        PS3_hue = (PS3_hue - 5)%255;
//        grabber.setHue(static_cast<uint8_t> (PS3_hue));
//    }
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
