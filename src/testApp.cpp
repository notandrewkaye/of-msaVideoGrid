#include "testApp.h"
#include "ofxXmlSettings.h"


int numX;
int numY;
bool bDisplayNames;
bool bMute;


class VideoCell {
public:
    ofRectangle rect;
    ofVideoPlayer video;
    string name;
    
    void draw() {
        ofPushStyle();
        ofSetColor(255);
        video.update();
        video.draw(rect);
        ofSetLineWidth(1);
        ofSetColor(0, 100, 0);
        //ofLine(rect.getLeft(), rect.getBottom(), rect.getLeft() + rect.getWidth() * video.getPosition(), rect.getBottom());
        if(bDisplayNames) {
            ofSetColor(0, 255, 0);
            ofDrawBitmapString(name, rect.getLeft() + 10, rect.getTop() + 15);
        }
        ofPopStyle();
    }
};


vector<VideoCell*> videoCells;
VideoCell *bigVideo = NULL;

//--------------------------------------------------------------
void testApp::setup() {
    
    
    ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	
	// 0 output channels,
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	soundStream.listDevices();
	
	//if you want to set a different device id
	//soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;
    
	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    
    
    if (XML.loadFile("mapSettings.xml")) {
        ofDrawBitmapString("mapSettings.xml successfully loaded!!", 200, 600 );
        
    } else {
        ofDrawBitmapString("Unable to load: mapSettings.xml ", 200, 600 );
    }
    
    ofSetColor(127, 63, 255);
    ofFill();
    
    numVerticies = XML.getNumTags("VERTEX:PT");
    
    if (numVerticies > 0) {
        
        XML.pushTag("VERTEX", numVerticies-1);
        nPts = XML.getNumTags("PT");
        
        if (nPts > 0) {
            
            numToRead = numPts;
            
            for (int i=0; i < nPts; i++) {
                
                
                int x = XML.getValue("PT:X",0,i);
                
                int y = XML.getValue("PT:Y",0,i);
                
                pts[i].x = x;
                pts[i].y = y;
            }
        }
        
        XML.popTag();
        
    }
    
    
    
    ofxXmlSettings xml;
    xml.loadFile("settings.xml");
    numX = xml.getValue("numX", 4);
    numY = xml.getValue("numY", 4);
    bDisplayNames = xml.getValue("bDisplayNames", true);
    bMute = xml.getValue("bMute", true);
    
    ofSetVerticalSync(true);
    ofBackground(0);
    ofDirectory dir;
    dir.listDir("videos");
    for(int i=0; i<dir.size(); i++) {
        VideoCell *videoCell = new VideoCell();
        bool bLoaded = videoCell->video.loadMovie(dir.getPath(i));
        if(bLoaded) {
            videoCell->video.setVolume(0);
            videoCell->video.play();
            videoCell->name = ofFilePath::getBaseName(dir.getName(i));
            videoCells.push_back(videoCell);
        } else {
            delete videoCell;
        }
        ofLogNotice() << "loading file " << i << " " << dir.getPath(i) << (bLoaded ? " OK" : " FAIL");
    }
}


//--------------------------------------------------------------
int indexFor(int x, int y) {
    int i = floor(ofMap(x, 0, ofGetWidth(), 0, numX));
    int j = floor(ofMap(y, 0, ofGetHeight(), 0, numY));
    int index = i + j*numX;
    return index < videoCells.size() ? index : -1;
}


//--------------------------------------------------------------
void activate() {
    int index = ofRandom(videoCells.size());
    //int index = ofRandom(16);
    //if(index >=0) {
    if(bigVideo) {
        //delete bigVideo;
        //bigVideo = NULL;
        for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.stop(); // stop all videos
        ofDirectory dir;
        //for full screen video
        dir.listDir("videos");
        bigVideo->video.loadMovie(dir.getPath(index));
        bigVideo->video.play();
        bigVideo->video.setVolume(0);
        
        // for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.stop();
        //
        
        //            for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.play();
        //            bigVideo = new VideoCell();
        //            ofDirectory dir;
        //            //for full screen video
        //            dir.listDir("videos");
        //            bigVideo->video.loadMovie(dir.getPath(index));
        //            bigVideo->video.play();
        //            bigVideo->video.setVolume(0);
        //        }
        //    }
        
    } else {
        bigVideo = new VideoCell();
        ofDirectory dir;
        //for full screen video
        dir.listDir("videos");
        bigVideo->video.loadMovie(dir.getPath(index));
        bigVideo->video.play();
        bigVideo->video.setVolume(0);
    }
    
}

//--------------------------------------------------------------
void testApp::update() {
    
    for (int i = 0; i < MAX_N_PTS; i++) {
        tVertex[i].set(pts[i].x,pts[i].y);
    }
    
    //lets scale the vol up to a 0-1 range
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
    //    if(bigVideo) bigVideo->update();
    //    else for(int i=0; i<videoCells.size(); i++) videoCells[i]->update();
}

//--------------------------------------------------------------
void testApp::draw() {
    
    counter += 1;
    
    if (counter > 1000) {
        counter = 0;
        activate();
    }
    
    if (counter >600) {
        if (scaledVol > 0.49) {
            activate();
        }
    }
    
    cout << counter << endl;
    
    // playing fullscreen video
    
    if (bSaved) {
        
        if (XML.pushTag("VERTEX", lastTagNumber-1)) {
            
            
            for (int i=0; i < nPts; i++) {
                
                int x = pts[i].x;
                XML.setValue("PT:X",x,i);
                int y = pts[i].y;
                XML.setValue("PT:Y",y,i);
            }
            XML.popTag();
        }
        
    }
    
    if(bigVideo) {
        float videoAspect = bigVideo->video.getWidth() / (float)bigVideo->video.getHeight();
        float screenAspect = ofGetWidth() / (float)ofGetHeight();
        int x, y, w, h;
        if(videoAspect > screenAspect) {
            w = ofGetWidth();
            h = w / videoAspect;
            x = 0;
            y = (ofGetHeight() - h)/2;
        } else {
            h = ofGetHeight();
            w = h * videoAspect;
            y = 0;
            x = (ofGetWidth() - w)/2;
        }
        bigVideo->rect.set(x, y, w, h);
        bigVideo->draw();
    }
    
    // playing thumbnails
    
    else {
        // this for statement PAUSES  all thumbnails
        // " " will trigger fullscreen big video
        for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.stop();
        int cellw = ofGetWidth() / numX;
        int cellh = ofGetHeight() / numY;
        
        for(int i=0; i<videoCells.size(); i++) {
            VideoCell *videoCell = videoCells[i];
            int x = (i%numX) * cellw;
            int y = (i/numX) * cellh;
            float videoAspect = videoCell->video.getWidth() / (float)videoCell->video.getHeight();
            float screenAspect = cellw / (float)cellh;
            int w, h;
            if(videoAspect > screenAspect) {
                w = cellw;
                h = w / videoAspect;
            } else {
                h = cellh;
                w = h * videoAspect;
            }
            videoCell->rect.set(x, y, w, h);
            videoCell->draw();
        }
    }
    ofSetColor(255, 255, 255);
    //ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
    
    
    // MASK
    
    ofFill();
    
    for (int i = 0; i < nPts; i+=4){
        ofBeginShape();
        
        ofSetColor(0, 0, 0);
        ofVertex(pts[i].x, pts[i].y);
        ofLine(pts[i].x, pts[i].y, pts[i+1].x, pts[i+1].y);
        
        ofVertex(pts[i+1].x, pts[i+1].y);
        ofLine(pts[i+1].x, pts[i+1].y, pts[i+2].x, pts[i+2].y);
        
        
        ofVertex(pts[i+2].x, pts[i+2].y);
        ofLine(pts[i+2].x, pts[i+2].y, pts[i+3].x, pts[i+3].y);
        
        ofVertex(pts[i+3].x, pts[i+3].y);
        ofLine(pts[i+3].x, pts[i+3].y, pts[i].x, pts[i].y);
        
        ofEndShape();
        
        
    }
    
    if (bDebugMode) {
        
        for (int i=0; i<MAX_N_PTS; i++) {
            ofSetColor(255, 0, 0,63);
            ofCircle(tVertex[i], 10);
            
            if(abs(ofDist(mouseX,mouseY,pts[i].x,pts[i].y)) < 10) {
                ofSetColor(0, 255, 0, 127);
                ofCircle(tVertex[i], 10);
            }
            if(abs(ofDist(mouseX,mouseY,pts[i+1].x,pts[i+1].y)) < 10) {
                ofSetColor(0, 255, 0, 127);
                ofCircle(tVertex[i+1], 10);
            }
            if(abs(ofDist(mouseX,mouseY,pts[i+2].x,pts[i+2].y)) < 10) {
                ofSetColor(0, 255, 0, 127);
                ofCircle(tVertex[i+2], 10);
            }
            if(abs(ofDist(mouseX,mouseY,pts[i+3].x,pts[i+3].y)) < 10) {
                ofSetColor(0, 255, 0, 127);
                ofCircle(tVertex[i+2], 10);
            }
        }
    }
}



//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;
    
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;
        
		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :)
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :)
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
	
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if (key == 'd') {
        bDebugMode = true;
        ofBackground(40,40,40);
        ofSetColor(127, 0, 255,63);
        
    }
    if (key == 'p') {
        bDebugMode = false;
        ofBackground(0, 0, 0);
        ofSetColor(0, 0, 0);
    }
    if (key == 's') {
        XML.saveFile("mapSettings.xml");
        bSaved = true;
    }
    if (key == 'l') {
        XML.loadFile("mapSettings.xml");
    }
    
    switch(key) {
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'n':
            bDisplayNames ^= true;
            break;
            
        case ' ':
            //plays the video fullscreen
            activate();
            break;
            
        case 'm':
            bMute ^= true;
            if(bMute) for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.setVolume(0);
            
            break;
            
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    int index = indexFor(x, y);
    if(!bigVideo && index >=0) {
        for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.setVolume(0);
        if(!bMute) videoCells[index]->video.setVolume(1);
    }
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    mousePressed(x, y, button);
    if (bDebugMode) {
        
        for (int i=0; i<MAX_N_PTS; i++) {
            if(abs(ofDist(mouseX,mouseY,pts[i].x,pts[i].y)) < 10) {
                pts[i].x = x;
                pts[i].y = y;
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    
    if (bDebugMode) {
        
        if (nPts < MAX_N_PTS && abs(ofDist(mouseX,mouseY,pts[nPts-1].x,pts[nPts-1].y)) > 10){
            pts[nPts].x = x;
            pts[nPts].y = y;
            nPts++;
        }
    }
    
    if(bigVideo) {
        bigVideo->video.setPosition(ofNormalize(x, bigVideo->rect.getLeft(), bigVideo->rect.getRight()));
    } else {
        int index = indexFor(x, y);
        if(index >=0) {
            VideoCell *videoCell = videoCells[index];
            videoCell->video.setPosition(ofNormalize(x, videoCell->rect.getLeft(), videoCell->rect.getRight()));
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
    
}