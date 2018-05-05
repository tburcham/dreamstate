#include "ofApp.h"

//NOTE: if you are unable to connect to your device on OS X, try unplugging and replugging in the power, while leaving the USB connected.
//ofxKinectV2 will only work if the NUI Sensor shows up in the Superspeed category of the System Profiler in the USB section.

//On OS X if you are not using the example project. Make sure to add OpenCL.framework to the Link Binary With Library Build Phase
//and change the line in Project.xcconfig to OTHER_LDFLAGS = $(OF_CORE_LIBS) $(OF_CORE_FRAMEWORKS) -framework OpenCL

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Uncomment for verbose info from libfreenect2
    //ofSetLogLevel(OF_LOG_VERBOSE);
    
    
    
    ofBackground(0, 0, 0);
    
    //ofEnableNormalizedTexCoords();
    //ofDisableArbTex();
    
    //see how many devices we have.
    ofxKinectV2 tmp;
    vector <ofxKinectV2::KinectDeviceInfo> deviceList = tmp.getDeviceList();
    
    //allocate for this many devices
    kinects.resize(deviceList.size());
    texDepth.resize(kinects.size());
    texRGB.resize(kinects.size());
    meshes.resize(kinects.size());
    //pointCloud.resize(kinects.size());
    vbos.resize(kinects.size());
    
    panel.setup("", "settings.xml", 10, 100);
    
    //Note you don't have to use ofxKinectV2 as a shared pointer, but if you want to have it in a vector ( ie: for multuple ) it needs to be.
    for(int d = 0; d < kinects.size(); d++){
        kinects[d] = shared_ptr <ofxKinectV2> (new ofxKinectV2());
        kinects[d]->open(deviceList[d].serial);
        panel.add(kinects[d]->params);
        
        meshes[d].setMode(OF_PRIMITIVE_LINES);
        //meshes[d].setMode(OF_PRIMITIVE_POINTS);
       /* meshes[d].enableColors();
        meshes[d].enableIndices();
        meshes[d].enableTextures();*/
    }
    
    panel.add(pointCloudSteps.setup("pointCloudSteps", 25, 1, 100));
    panel.add(thickness.setup("thickness", 1, 1, 50));
    panel.add(jitter.setup("jitter", 1, -100, 100));
    panel.add(pointSize.setup("pointSize", 1, 1, 10));
    panel.add(connectionDistance.setup("connectionDistance", 250, 1, 500));
    panel.add(pointDepth.setup("pointDepth", 2000, 1, 5000));
    panel.add(doShader.setup("shader?", false));
    panel.add(showKinectDebug.setup("showKinectDebug?", false));
    panel.add(orbitInc.setup("orbit speed", 0.1, -2, 2));
    panel.add(distance.setup("cam distance", 1000, 0, 10000));
    
    
    
    panel.loadFromFile("settings.xml");
    
    //ofEnableDepthTest();
    //glEnable(GL_POINT_SMOOTH); // use circular points instead of square points
    //ofEnableSmoothing();
    
    cam.setFov(60);
    cam.setNearClip(1);
    cam.setFarClip(20000);
    
    orbitAngle = rollAngle = 0.0f;
    
    shader.load("shaders/vert.glsl", "shaders/frag.glsl", "shaders/geom.glsl");
    shader.bindDefaults();
    ofLog() << "Maximum number of output vertices support is: " << shader.getGeometryMaxOutputCount();
    
    doShader = false;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    glPointSize(pointSize);
    
    orbitAngle += orbitInc;
    
    cam.orbit(orbitAngle, 0, distance);
    //cam.roll(roll);
    
    for(int d = 0; d < kinects.size(); d++){
        kinects[d]->update();
        if( kinects[d]->isFrameNew() ){
            texDepth[d].loadData( kinects[d]->getDepthPixels() );
            texRGB[d].loadData( kinects[d]->getRgbPixels() );
            
            //pointCloud.push_back(kinects[d]->getWorldCoordinates());
            
            pointCloud = kinects[d]->getWorldCoordinates();
            
            
            
            reducedPointCloud = reducePointCloud(pointCloud, pointCloudSteps);
            
            //texDepth[d].
            
            
            //meshes[d].clear();
            
            /*cout << "pc ";
            cout << reducedPointCloud.size();
            cout << pointCloud.size();
            cout << "\n";*/
            
            //meshes[d].addVertices(pc);
            
            // Let's add some lines!
            // float connectionDistance = 50;
            /*int numVerts = meshes[d].getNumVertices();
            for (int a=0; a<numVerts; ++a) {
                ofVec3f verta = meshes[d].getVertex(a);
                for (int b=a+1; b<numVerts; ++b) {
                    ofVec3f vertb = meshes[d].getVertex(b);
                    float distance = verta.distance(vertb);
                    if (distance <= connectionDistance) {
                        // In OF_PRIMITIVE_LINES, every pair of vertices or indices will be
                        // connected to form a line
                        meshes[d].addIndex(a);
                        meshes[d].addIndex(b);
                    }
                }
            }*/
            
            //meshes[d].addTexCoords(texRGB[0].getTextureData().); //(0, 0, texRGB[0].getWidth(), texRGB[0].getHeight());
            
            //cout << meshes[d].getNumVertices();
            //cout << "\n";

        }
    }
    
    /*int width = 1000, height = 1000;
    meshes[0].clear();
    for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
            meshes[0].addVertex(ofPoint(x*20, y*20, 0)); // make a new vertex
            meshes[0].addColor(ofFloatColor(255.0,0, 0.0, 125.0));  // add a color at that vertex
        }
    }*/
    
    //vbos[0].setMesh(meshes[0], GL_STATIC_DRAW);
    
    // what this is basically doing is figuring out based on the way we inserted vertices
    // into our vertex array above, which array indices of the vertex array go together
    // to make triangles. the numbers commented show the indices added in the first run of
    // this loop - notice here that we are re-using indices 1 and 10
    /*for (int y = 0; y < height-1; y++){
        for (int x=0; x < width-1; x++){
            meshes[0].addIndex(x+y*width);         // 0
            meshes[0].addIndex((x+1)+y*width);     // 1
            meshes[0].addIndex(x+(y+1)*width);     // 10
            
            meshes[0].addIndex((x+1)+y*width);     // 1
            meshes[0].addIndex((x+1)+(y+1)*width); // 11
            meshes[0].addIndex(x+(y+1)*width);     // 10
        }
    }*/
    
    /*meshes[0].clear();
    meshes[0] = ofMesh.box(200.0, 200.0, 200.0, 10.0, 10.0, 10.0);*/
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    //ofDrawBitmapString("ofxKinectV2: Work in progress addon.\nBased on the excellent work by the OpenKinect libfreenect2 team\n\n-Requires USB 3.0 port ( superspeed )\n-Requires patched libusb. If you have the libusb from ofxKinect ( v1 ) linked to your project it will prevent superspeed on Kinect V2", 10, 14);
    
    
    
    
    
    cam.begin();
    cam.setTarget(ofVec3f(0.0, 0.0, 0.0));
    //cam.rotate(0.1, cam.getUpDir() );
    //cam.setPosition(ofGetWidth()/2,ofGetHeight()/2, 1000);
    
    
    
    for(int d = 0; d < kinects.size(); d++){
        float dwHD = ofGetWidth()/4;
        float dhHD = ofGetHeight()/4;
        
        float shiftY = 100 + ((10 + texDepth[d].getHeight()) * d);
        
        //texDepth[d].draw(200, shiftY);
        //texRGB[d].draw(210 + texDepth[d].getWidth(), shiftY, dwHD, dhHD);
        
        ofSetColor(255, 255, 255, 128);
        
        //cout << meshes[d].getNumVertices();
        //cout << "\n";
        
        
        
        //shaderTexture.bindAsImage();
        //shaderTexture.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
        
        
        if(doShader) {
            
            shader.begin();
            
            shaderTexture = texRGB[0];
            shaderTexture.bind();
            
            
            
            
            // set thickness of ribbons
            shader.setUniform1f("thickness", thickness);
            shader.setUniform1f("jitterScale", jitter); //jitter * ofMap(snare, 0, 1, 1, 10));
            
            // make light direction slowly rotate
            shader.setUniform3f("lightDir", sin(ofGetElapsedTimef()/20), cos(ofGetElapsedTimef()/20), 0);
            
            shader.setUniform2f("resolution", 100, 100); //ofVec2f(vidPlayer.getWidth(), vidPlayer.getHeight()));
            shader.setUniform2f("mouse", ofVec2f(mouseX, mouseY));
            shader.setUniform1f("time", sin(ofGetElapsedTimef()));
            //shader.setUniformTexture("tex0", texturePatternImg.getTexture(), 1);
            
            
            
            
            
            shader.setUniformTexture("tex0", shaderTexture, 1);
            
            
            //shader.setUniformTexture("tex0", vidGrabber.getTexture(), 1);
            
        }
        
        ofPushMatrix();
        
        
        ofScale(1, -1, -1);
        ofTranslate(dwHD, dhHD, 0); // center the points a bit
        ofEnableDepthTest();
        
        
        /*cout << "rpc ";
        cout << reducedPointCloud.size();
        cout << "\n";*/
        
        if (reducedPointCloud.size() > 0) {
            for (int i = 0; i < reducedPointCloud.size() - 2; i++) {
                
                /*cout << i;
                cout << "\n";*/
                
                float distance = reducedPointCloud[i].distance(reducedPointCloud[i + 1]);
                
                if (distance < connectionDistance) {
                    
                    //texRGB[0].draw();
                    //ofTextureData td = texRGB[0].getTextureData();
                    //texRGB[0].draw(0, 0);
                    //ofSetColor(texRGB[0].getTextureData)
                    //ofPixels p;
                    //exRGB[0].getTextureData()
                    
                    
                    
                    
                    ofDrawLine(reducedPointCloud[i], reducedPointCloud[i + 1]);
                }
                
            }
        }
        
        //meshes[d].draw();
        //vbos[0].drawElements(GL_TRIANGLES, 100);
        
        
        //mesh.drawVertices();
        //meshes[d].draw();
        /*ofMesh m = meshes[d].box(200.0, 200.0, 200.0, 10.0, 10.0, 10.0);
        
        m.drawFaces();
        m.drawVertices();
        m.draw();*/
        
        //shaderTexture.draw(0, 0);
        
        
        
        ofDisableDepthTest();
        ofPopMatrix();
        
        
        //kinect.getColorSource()->getTexture().bind()
        //texDepth[d].bind();
        //mesh.draw();
        //texDepth[d].unbind();
        //kinect.getColorSource()->getTexture().unbind()
        
        //pointCloud[d].
    }
    
    if(doShader) {
        //shaderTexture.unbind();
        shader.end();
        
    }
    
    if (showKinectDebug) {
        texDepth[0].draw(0, 0);
        texRGB[0].draw(0, 0);
    }
    
    cam.end();
    
    if (!bHide) {
         panel.draw();
        
        ofDisableDepthTest();
    } else {
        ofEnableDepthTest();
    }
    
   
}




vector<ofPoint> ofApp::reducePointCloud(vector<ofPoint> pc, int steps){
    
    vector<ofPoint> reducedPc;
    
    for (int i = 0; i < pc.size(); i+=steps) {
        
        if (pc[i].x != 0 && pc[i].y != 0 && pc[i].z != 0 && pc[i].z < pointDepth*1.0) {
        
            reducedPc.push_back(pc[i]);
            
        }
        
    }
    
    return reducedPc;
    
    
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'h'){
        bHide = !bHide;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
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
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
