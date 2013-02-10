#include "testApp.h"


const int numX = 4;
const int numY = 4;
bool bDisplayNames = true;
bool bMute = false;


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
        ofLine(rect.getLeft(), rect.getBottom(), rect.getLeft() + rect.getWidth() * video.getPosition(), rect.getBottom());
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
void testApp::setup(){
    ofSetVerticalSync(true);
    ofBackground(0);
    ofDirectory dir;
    dir.listDir("videos/small");
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
    int index = indexFor(ofGetMouseX(), ofGetMouseY());
    if(index >=0) {
        if(bigVideo) {
            delete bigVideo;
            bigVideo = NULL;
            for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.play();
        } else {
            for(int i=0; i<videoCells.size(); i++) videoCells[i]->video.stop();
            bigVideo = new VideoCell();
            ofDirectory dir;
            dir.listDir("videos/big");
            bigVideo->video.loadMovie(dir.getPath(index));
            bigVideo->video.play();
            bigVideo->video.setVolume(1);
        }
    }
}

//--------------------------------------------------------------
void testApp::update() {
    //    if(bigVideo) bigVideo->update();
    //    else for(int i=0; i<videoCells.size(); i++) videoCells[i]->update();
}

//--------------------------------------------------------------
void testApp::draw() {
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
    else {
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
    //    ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key) {
        case 'f':
            ofToggleFullscreen();
            break;
            
        case 'n':
            bDisplayNames ^= true;
            break;
            
        case ' ':
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
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
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