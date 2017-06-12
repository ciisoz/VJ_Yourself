#include "ofApp.h"

#include "parametersControl.h"


//--------------------------------------------------------------
void ofApp::setup(){

    parametersControl::getInstance().setup();

    isRecording=true;
    
    /// GRABBER
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
    grabber.setFps(grabFPS);
    //grabber.ofBaseVideoGrabber::setDesiredFrameRate(grabFPS);
    grabber.setDeviceID(0);
    grabber.initGrabber(grabberResolution.x,grabberResolution.y);
    
    /// PIPELINE
    //vRate.setup(grabber,grabFPS);
    //vBuffer.setup(vRate, 60,true);
    vBuffer.setup(grabber, 120,true);
    vHeader.setup(vBuffer);
    vHeader.setDelayMs(33.33f);

    vRendererGrabber.setup(grabber);
    vRendererBuffer.setup(vBuffer);
    vRendererHeader.setup(vHeader);

    //to do : si el trec peta
    //sleep(2);
    
    // PRINT INFOS
    cout << "Buffer : Max Size : " << vBuffer.getMaxSize() << endl;
    cout << "Buffer : FPS : " << vBuffer.getFps() << endl;
    cout << "Buffer : Total Time : " << vBuffer.getTotalTime() << endl;
    cout << "Buffer : Total Frames : " << vBuffer.getTotalFrames() << endl;
    
    ofBackground(32);
    
    soundStream.setup(this, 0, 2, 44100, 512, 4);
}

//--------------------------------------------------------------
void ofApp::update(){

    if(isRecording) grabber.update();
 
    
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetColor(255);
    
    vRendererBuffer.draw(10,10,160,120);
    vRendererHeader.draw(10,260,640,480);
    
    vBuffer.draw();
    vHeader.draw();
    
    
    float actualFPS = ofGetFrameRate();
    if(actualFPS>=60) ofSetColor(0,255,0);
    else if(actualFPS>=30) ofSetColor(255,128,0);
    else ofSetColor(255,0,0);
    
    ofDrawBitmapString(actualFPS, ofGetWidth()-25, 25);
    
    ofSetColor(255);
    ofDrawBitmapString("Frame[0] Timestamp : " + ofToString(vBuffer.getFirstFrameTimestamp().raw()),ofGetWidth()-350,50);
    ofDrawBitmapString("testTS : " + ofToString(testTS.elapsed()),ofGetWidth()-350,75);
    ofDrawBitmapString("tdiff : " + ofToString(tdiff),ofGetWidth()-350,100);
    //    ofDrawBitmapString("StopTS : " + ofToString(tsStop.elapsed()),ofGetWidth()-350,105);
    //    ofDrawBitmapString("StopTS R: " + ofToString(tsStop.raw()),ofGetWidth()-350,140);
}

//-----------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){
    vHeader.audioRateTrigger(bufferSize);
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
    else if(key=='l')
    {
        if(!vHeader.isPlaying())
        {
            //        vHeader.setInPct(0.25);
            //        vHeader.setOutPct(0.75);
            vHeader.setPlaying(true);
            vHeader.setInMs(500+33.33333333*30);
            vHeader.setOutMs(500);
            vHeader.setLoopMode(3);
            vHeader.setSpeed(1.0);
        }
        else
        {
            vHeader.setPlaying(false);
        }
    }
    else if(key=='s')
    {
        tdiff=testTS.elapsed();
//        tsStop.update();
    }
    else if(key=='d')
    {
        testTS.update();
        testTS -= tdiff;
    }
    else if(key=='f')
    {
        bool b=true;
        vHeader.getPhasors()[0]->resetPhasor(b);
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
