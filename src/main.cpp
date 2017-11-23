#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
//	ofSetupOpenGL(1280,1024,OF_WINDOW);			// <-------- setup the GL context
//
//	// this kicks off the running of my app
//	// can be OF_WINDOW or OF_FULLSCREEN
//	// pass in width and height too:
//	ofRunApp(new ofApp());

    /* 
     
    ofGLWindowSettings settings;
    settings.width = 1280;
    settings.height = 1024;
    settings.setGLVersion(4,1);
    ofCreateWindow(settings);
    
    // this kicks off the running of my app
    // can be OF_WINDOW or OF_FULLSCREEN
    // pass in width and height too:
    ofRunApp(new ofApp());
    
    */
    
    


    ofGLFWWindowSettings settings;

    // MAIN WINDOWS
    settings.width = 1280;
    settings.height = 1024;
    settings.setPosition(ofVec2f(320,0));
    settings.setGLVersion(4,1);
    settings.resizable = true;
    shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

    // SECOND WINDOW
    settings.width = 320;
    settings.height = 240;
    settings.setPosition(ofVec2f(0,0));
    settings.resizable = true;
    //settings.setGLVersion(4,1);
    // uncomment next line to share main's OpenGL resources with gui
    settings.shareContextWith = mainWindow;
    shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);

    mainWindow->setVerticalSync(false);
    guiWindow->setVerticalSync(false);

    shared_ptr<ofApp> mainApp(new ofApp);
    ofAddListener(guiWindow->events().draw,mainApp.get(),&ofApp::drawProgramWindow);

    ofRunApp(mainWindow, mainApp);
    ofRunMainLoop();
     
     



    
}
