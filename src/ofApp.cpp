#include "ofApp.h"

//NOTE: if you are unable to connect to your device on OS X, try unplugging and replugging in the power, while leaving the USB connected.
//ofxKinectV2 will only work if the NUI Sensor shows up in the Superspeed category of the System Profiler in the USB section.

//On OS X if you are not using the example project. Make sure to add OpenCL.framework to the Link Binary With Library Build Phase
//and change the line in Project.xcconfig to OTHER_LDFLAGS = $(OF_CORE_LIBS) $(OF_CORE_FRAMEWORKS) -framework OpenCL

//--------------------------------------------------------------
void ofApp::setup(){
    
    //Uncomment for verbose info from libfreenect2
    //ofSetLogLevel(OF_LOG_VERBOSE);
    
    ofLog() << "GL Version" << glGetString( GL_VERSION );
    
    ofSoundStreamSetup(0, 1, this, 44100, beat.getBufferSize(), 4);
    
    
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
    
    ofLog() << "number of kinects: " << kinects.size() << endl;
    
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
    panel.add(doShader.setup("shader?", true));
    panel.add(showKinectDebug.setup("showKinectDebug?", false));
    panel.add(orbitInc.setup("orbit speed", 0.1, -2, 2));
    panel.add(distance.setup("cam distance", 1000, 0, 10000));
    panel.add(blurAmnt.setup("blurAmnt", 100, -500, 500));
    
    panel.add(xDistortRadius.setup("xDistortRadius", 1, -2.50, 2.50));
    panel.add(yDistortRadius.setup("yDistortRadius", 1, -2.50, 2.50));
    panel.add(zDistortRadius.setup("zDistortRadius", 1, -2.50, 2.50));
    
    
    
    
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
    
    
    //cam.roll(roll);
    
    for(int d = 0; d < kinects.size(); d++){
        kinects[d]->update();
        if( kinects[d]->isFrameNew() ){
            texDepth[d].loadData( kinects[d]->getRawDepthPixels() );
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
    
    
    
    beat.update(ofGetElapsedTimeMillis());
    
    auto duration = 200.f;
    //auto endTime = initTime + duration;
    auto now = ofGetElapsedTimef();
    
    float kick = beat.kick();
    float snare = beat.snare();
    float hihat = beat.hihat();
    
    float radius = ofMap(snare, 0, 1, 1, xDistortRadius);
    float yRadius = ofMap(kick, 0, 1, 1, yDistortRadius);
    float zRadius = ofMap(hihat, 0, 1, 1, zDistortRadius);
    
    for (int y = 0; y < reducedPointCloud.size(); y++) {
        reducedPointCloud[y].x *= radius;
        reducedPointCloud[y].y *= yRadius;
        reducedPointCloud[y].z *= zRadius;
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
    orbitAngle += orbitInc;
    
    cam.orbit(orbitAngle, 1, distance);
    cam.setTarget(ofVec3f(0.0, 0.0, 0.0));
    //cam.rotate(0.1, cam.getUpDir() );
    //cam.setPosition(ofGetWidth()/2,ofGetHeight()/2, 1000);
    
    
    
    for(int d = 0; d < kinects.size(); d++){
        float dwHD = ofGetWidth();
        float dhHD = ofGetHeight();
        
        float shiftY = 100 + ((10 + texDepth[d].getHeight()) * d);
        
        //texDepth[d].draw(200, shiftY);
        //texRGB[d].draw(210 + texDepth[d].getWidth(), shiftY, dwHD, dhHD);
        
        ofSetColor(255, 255, 255, 128);
        
        //cout << meshes[d].getNumVertices();
        //cout << "\n";
        
        cout << "Depth ";
        cout << texDepth[d].getHeight();
        cout << " ";
        cout << texDepth[d].getWidth();
        cout << "\n";
        cout << "RGB ";
        cout << texRGB[d].getHeight();
        cout << " ";
        cout << texRGB[d].getWidth();
        cout << "\n";
        
        cout << texDepth[d].getHeight() / texRGB[d].getHeight();
        cout << "\n";
        cout << texDepth[d].getWidth() / texRGB[d].getWidth();
        cout << "\n";
        
        /*dwHD = dwHD / (texDepth[d].getWidth() / texRGB[d].getWidth());
         dhHD = dhHD / (texDepth[d].getHeight() / texRGB[d].getHeight());*/
        
        //shaderTexture.bindAsImage();
        //shaderTexture.allocate(ofGetWidth(),ofGetHeight(),GL_RGB);
        
        //ofFbo fbo;
        /*fbo.allocate(texRGB[d].getWidth(), texRGB[d].getHeight(), GL_RGB);
         fbo.begin();
         texRGB[0].draw(0, 0, fbo.getWidth(), fbo.getHeight());
         fbo.end();*/
        
        
        if(doShader) {
            
            shader.begin();
            
            shaderTexture = texRGB[0];
            //shaderTexture.bind();
            
            
            
            
            // set thickness of ribbons
            shader.setUniform1f("thickness", thickness);
            shader.setUniform1f("jitterScale", jitter); //jitter * ofMap(snare, 0, 1, 1, 10));
            
            // make light direction slowly rotate
            shader.setUniform3f("lightDir", sin(ofGetElapsedTimef()/20), cos(ofGetElapsedTimef()/20), 0);
            
            shader.setUniform2f("resolution", 100, 100); //ofVec2f(vidPlayer.getWidth(), vidPlayer.getHeight()));
            shader.setUniform2f("mouse", ofVec2f(mouseX, mouseY));
            shader.setUniform1f("time", sin(ofGetElapsedTimef()));
            
            shader.setUniform1f("blurAmnt", blurAmnt);
            //shader.setUniformTexture("tex0", texturePatternImg.getTexture(), 1);
            
            
            //fbo.draw(0, 0);
            
            
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
        
        //ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES);
        mesh.clear();
        
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
                    
                    
                    
                    
                    //ofDrawLine(reducedPointCloud[i], reducedPointCloud[i + 1]);
                    
                    ofPoint pc1 = reducedPointCloud[i];
                    //ofPoint pc2 = reducedPointCloud[i + 1];
                    
                    
                    mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
                    
                    mesh.addVertex( pc1 );
                    mesh.addTexCoord( shaderTexture.getCoordFromPoint(pc1.x + (dwHD / 2), pc1.y + (dhHD / 2)) );
                    
                    /*mesh.addVertex( pc2 );
                     mesh.addTexCoord( shaderTexture.getCoordFromPoint(pc2.x + (dwHD / 2), pc2.y + (dhHD / 2)) );*/
                    
                    
                    /*mesh.addVertex( ofPoint(x,y+h) );
                     mesh.addTexCoord( myTexture.getCoordFromPercent(0,1) );
                     
                     mesh.addVertex( ofPoint(x+w,y+h) );
                     mesh.addTexCoord( myTexture.getCoordFromPercent(1,1) );*/
                    
                    //mesh.draw();
                    
                    /* myTexture.bind();
                     mesh.draw();
                     myTexture.unbind();*/
                }
                
            }
        }
        
        mesh.draw();
        
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
        
        //fbo.draw(0, 0);
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

void ofApp::audioReceived(float* input, int bufferSize, int nChannels) {
    beat.audioReceived(input, bufferSize, nChannels);
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'h'){
        bHide = !bHide;
    }
    if(key == 's') {
        
        string ts = ofGetTimestampString();
        
        string fname = "dreamstate_" + ts + ".ply";
        mesh.save(fname);
        
        img.grabScreen(0, 0 , ofGetWidth(), ofGetHeight());
        img.save("dreamstate_" + ts + ".png");
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
