#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

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
    // to do : VideoRate peta ?À
    // vRate.setup(grabber,grabFPS);
    vBuffer.setup(grabber, 60,true);
    vHeader.setup(vBuffer);
    vHeader.setDelayMs(33.33f);

    vRendererGrabber.setup(grabber);
    vRendererBuffer.setup(vBuffer);
    vRendererHeader.setup(vHeader);

    //to do : si el trec peta
    sleep(2);
    
    // PRINT INFOS
    cout << "Buffer : Max Size : " << vBuffer.getMaxSize() << endl;
    cout << "Buffer : FPS : " << vBuffer.getFps() << endl;
    cout << "Buffer : Total Time : " << vBuffer.getTotalTime() << endl;
    cout << "Buffer : Total Frames : " << vBuffer.getTotalFrames() << endl;
    
    ofBackground(32);
}

//--------------------------------------------------------------
void ofApp::update(){

    if(isRecording) grabber.update();
 
    
}

//--------------------------------------------------------------
void ofApp::draw()
{
    vRendererBuffer.draw(10,10,320,240);
    vRendererHeader.draw(10,260,640,480);
    
    vBuffer.draw();
    vHeader.draw();
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
            vHeader.setInMs(1500);
            vHeader.setOutMs(500);
            vHeader.setLoopMode(2);
            vHeader.setSpeed(4.0);
        }
        else
        {
            vHeader.setPlaying(false);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

    if(ofGetKeyPressed(int('d')))
    {
        float factor = (float(x)/float(ofGetWidth())) * vBuffer.getMaxSize();
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
