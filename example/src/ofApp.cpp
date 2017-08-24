#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofSetBackgroundColor(ofColor::black);

    bPolyClosed = false;
    polyThickness = 20;
}

//--------------------------------------------------------------
void ofApp::update() {

    poly.setClosed(bPolyClosed);

    ofxPolyGrow(polyOuter, poly, polyThickness);
    ofxPolyGrow(polyInner, poly, -polyThickness);
}

//--------------------------------------------------------------
void ofApp::draw() {

    poly.draw();

    const auto & points = poly.getVertices();
    for(size_t i = 0; i < points.size(); i++) {
        const auto & point = points[i];
        ofDrawCircle(point.x, point.y, 4);
    }

    ofxPolyDrawNormals(poly, polyThickness);

    ofSetColor(ofColor::magenta);
    polyOuter.draw();

    ofSetColor(ofColor::cyan);
    polyInner.draw();

    ofSetColor(ofColor::white);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if(key == 'c' || key == 'C') {
        bPolyClosed ^= true;
    }
    if(key == 'r' || key == 'R') {
        poly.clear();
    }
    if(key == 'l' || key == 'L') {
        if(ofxPolyLoad(poly, "poly.xml")) {
            bPolyClosed = poly.isClosed();
            cout << "poly.xml loaded" << endl;
        } else {
            cout << "poly.xml not found" << endl;
        }
    }
    if(key == 's' || key == 'S') {
        ofxPolySave(poly, "poly.xml");
        cout << "poly.xml saved" << endl;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    poly.addVertex(x, y);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
