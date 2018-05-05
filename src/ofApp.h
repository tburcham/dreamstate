#pragma once

#include "ofMain.h"
#include "ofxKinectV2.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    ofxPanel panel;
    ofxIntSlider pointCloudSteps;
    ofxIntSlider pointSize;
    ofxIntSlider pointDepth;

    ofxFloatSlider connectionDistance;
    ofxToggle doShader;
    ofxToggle showKinectDebug;
    ofxFloatSlider thickness;
    ofxFloatSlider jitter;
    ofxFloatSlider orbitInc;
    ofxFloatSlider rollInc;
    ofxFloatSlider distance;
    
    
    vector < shared_ptr<ofxKinectV2> > kinects;
    
    vector <ofTexture> texDepth;
    vector <ofTexture> texRGB;
    //vector <vector<ofPoint>> pointCloud;
    
    vector<ofPoint> pointCloud;
    vector<ofPoint> reducePointCloud(vector<ofPoint> pc, int steps);
    vector<ofPoint> reducedPointCloud;
    
    vector <ofMesh> meshes;
    vector <ofVbo> vbos;
    
    ofEasyCam cam;
    
    ofShader shader;
    //bool doShader;
    
    bool bHide;
    
    ofImage texturePatternImg;
    
    ofTexture shaderTexture;
    
    float orbitAngle, rollAngle;

};
