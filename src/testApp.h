#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#define MAX_N_PTS         16

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void audioIn(float * input, int bufferSize, int nChannels);
	
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    int 	drawCounter;
    
    float smoothedVol;
    float scaledVol;
    
    ofSoundStream soundStream;
    
    
    ofVec3f	pts[MAX_N_PTS];
    int		nPts;
    int numVerticies;
    int numPts;
    int numToRead;
    int pointCount;
    
    ofPoint tVertex[MAX_N_PTS];
    int selectedVertex;
    
    bool bDebugMode;
    bool bSaved;
    
    ofxXmlSettings XML;
    int lastTagNumber;
    
    ofPoint target[100];
    
    int currTarget;
    int ctr;
    
    int rX;
    int rY;
    
    int changeVideo;
    
    int counter = 0;
    
    
    
};
