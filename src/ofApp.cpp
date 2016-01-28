
#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(24);
    ofEnableAlphaBlending();
    ofNoFill();
    
    displayMode = true;
    displayInterval = 4;
    lineCount = 0;
    worldRadius = 300;
    
    frames = 0;
    frameLim = 900;
    
    JSONRecorder["hotels"] = Json::arrayValue;
    
    cout << "beginning input" << endl;
    
    ofBuffer buffer;
    buffer = ofBufferFromFile("hotelsbase.txt");
    
    cout << "input in ofBuffer, size:" << buffer.size() << endl;
    
    if(buffer.size()) {
        
        cout << "buffer not empty" << endl;
        
        int i = 0;
        
        int lim = 500000;
        
        for (ofBuffer::Line it = buffer.getLines().begin(), end = buffer.getLines().end();
             (it != end) && (i < lim); ++it) {
            
            if ((i % 1000) == 0) {
                cout << "parsed " << i << " lines" << endl;
            }
            
            string line = *it;
            
            ofStringReplace(line, ",", "-");
            // print out the line
            //cout << line << endl;
            
            if (i % displayInterval == 0){
                processLine(line);
            }
            
            i += 1;
        }
        
        cout << "set parsed: " << lim << " actual hotel dataset: " << hotels.size() << endl;
        
        if (!displayMode){
            cout << "saving to output.csv" << endl;
            csvRecorder.saveFile( ofToDataPath( "output.csv" ));
        
            if (JSONRecorder.save("output.json", true)){
                cout << "JSON save sucsessful" << endl;
            } else {
                cout << "JSON save failed" << endl;
            }
            if (JSONRecorder.save("output-short.json", false)){
                cout << "collapsed JSON save sucsessful" << endl;
            } else {
                cout << "collapsed JSON save failed" << endl;
            }
        }
    }
    
}



//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    if (displayMode) {
        
        ofPushMatrix();
        
        //translate so that the center of the screen is 0,0
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        ofSetColor(255, 255, 255, 20);
        
        //draw a translucent wireframe sphere (ofNoFill() is on)
        ofPushMatrix();
        //add an extra spin at the rate of 1 degree per frame
        //ofRotate(ofGetFrameNum(), 0, 1, 0);
        ofRotate(mouseX, 0, 1, 0);
        ofRotateX(ofMap(mouseY,0, ofGetHeight(), -90, 90, true));
        //ofDrawSphere(0, 0, 0, worldRadius);
        ofPopMatrix();
        
        ofSetColor(255);
        for(unsigned int i = 0; i < hotels.size(); i++){
            
            //three rotations
            //two to represent the latitude and lontitude of the city
            //a third so that it spins along with the spinning sphere
            ofQuaternion latRot, longRot, spinQuatX, spinQuatY;
            latRot.makeRotate(hotels[i].latitude, 1, 0, 0);
            longRot.makeRotate(hotels[i].longitude, 0, 1, 0);
            //spinQuatX.makeRotate(mouseX, 0, 1, 0);
            //spinQuatY.makeRotate(ofMap(mouseY,0, ofGetHeight(), -90, 90, true), 1, 0, 0);
            spinQuatX.makeRotate(ofGetFrameNum() * 0.5, 0, 1, 0);
            //spinQuatY.makeRotate(sin(ofGetElapsedTimef() * 1.0)  * 25, 1, 0, 0);
            spinQuatY.makeRotate(sin((float)(ofGetFrameNum() * 2 / 3) * TWO_PI / 360.0)  * 25, 1, 0, 0);
        
            //our starting point is 0,0, on the surface of our sphere, this is where the meridian and equator meet
            ofVec3f center = ofVec3f(0,0,300);
            //multiplying a quat with another quat combines their rotations into one quat
            //multiplying a quat to a vector applies the quat's rotation to that vector
            //so to to generate our point on the sphere, multiply all of our quaternions together then multiple the centery by the combined rotation
            ofVec3f worldPoint = latRot * longRot * spinQuatX * spinQuatY * center;
            int col = ofMap(worldPoint.z, (worldRadius * -1), worldRadius, 0, 255, true);
            
            //cout << worldPoint <<` endl;
            
            //draw it and label it
            //ofDrawLine(ofVec3f(0,0,0), worldPoint);
            ofSetColor(255,255,255,col);
            ofDrawCircle(worldPoint, 1);
            
            //set the bitmap text mode billboard so the points show up correctly in 3d
            //ofDrawBitmapString(hotels[i].name, worldPoint );
        }
        
        ofPopMatrix();
        if (frames < frameLim) {
            cout << "saving frame " << frames << endl;
            img.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
            
            string fileName = "split-recording/"+ ofToString(frames % 3) + "snapshot_" + ofToString(10000+frames)+".png";
            img.save(fileName);
            frames += 1;
        }
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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

//--------------------------------------------------------------
void ofApp::processLine(string line){
    
    vector<string> parts = ofSplitString(line, "~", false, true);
    //printf ("number of elems in line: %lu\n", parts.size());
    
    string name;
    float latitude;
    float longitude;
    
    if (displayMode){
        if ( lineCount != 0){
            for(int i = 0; i < parts.size(); i++){
                if( i == 1) {
                    name = parts[i];
                } else if ( i == 12 ) {
                    latitude = ofToFloat(parts[i]);
                } else if ( i == 13 ) {
                    longitude = ofToFloat(parts[i]);
                }
            }
            
            Hotel new_hotel = {name, latitude, longitude};
            hotels.push_back( new_hotel );
        }
    } else {
        if (lineCount == 0) {
            //header case
        
            for(int i = 0; i < parts.size(); i++){
                csvRecorder.setString(lineCount, i, parts[i]);
            }
        
            dataTypes = parts;
        
        } else {
            //general case
        
            Json::Value elem;
            
            for(int i = 0; i < parts.size(); i++){
                if (i == 9) {
                    //cout << ofSplitString(parts[i], ",", false, true)[0] << endl;
                
                    csvRecorder.setString(lineCount, i, ofSplitString(parts[i], ",", false, true)[0]);
                } else {
                    csvRecorder.setString(lineCount, i, parts[i]);
                }
                elem[dataTypes[i]] = parts[i];
            }
            JSONRecorder["hotels"].append(elem);
        
        }
    }
    
    lineCount += 1;
}
