//
//  ofxLines.cpp
//  example
//
//  Created by Lukasz Karluk on 14/09/2015.
//
//

#include "ofxPoly.h"

//--------------------------------------------------------------
void ofxPolyGrow(ofPolyline & poly, const ofPolyline & polySource, float amount) {
    
    poly.clear();
    poly.setClosed(polySource.isClosed());
    
    if(polySource.size() < 2) {
        poly = polySource;
        return;
    }
    
    const vector<ofVec3f> & points = polySource.getVertices();
    int numOfPoints = points.size();

    bool bClosed = true;
    bClosed = bClosed && (polySource.isClosed() == true);
    bClosed = bClosed && (numOfPoints >= 3);
    
    for(int i=0; i<numOfPoints; i++) {
        
        bool bEndings = false;
        bEndings = bEndings || (i == 0);
        bEndings = bEndings || (i == numOfPoints-1);
        bEndings = bEndings && (bClosed == false);
        
        if(bEndings == true) {

            const ofVec3f & p0 = points[i];
            ofVec3f n0 = polySource.getNormalAtIndex(i);
            ofVec3f point = p0 + (n0 * amount);
            poly.addVertex(point);

            continue;
        }
        
        int i0 = i-1;
        if(i0 < 0) {
            i0 += numOfPoints;
        }
        
        const ofVec3f & p0 = points[i0];
        const ofVec3f & p1 = points[i];
        ofVec3f n0 = ofVec2f(p0 - p1).getPerpendicular();
        ofVec3f n1 = polySource.getNormalAtIndex(i);
        
        float angle = ofVec2f(n0).angle(ofVec2f(n1));
        float length = amount / cos(angle * DEG_TO_RAD);
        
        ofVec3f point = p1 + (n1 * length);
        poly.addVertex(point);
    }
}

//--------------------------------------------------------------
void ofxPolyGrowAlongNormals(ofPolyline & poly, const ofPolyline & polySource, float normalLength) {
    vector<float> thicknesses;
    thicknesses.insert(thicknesses.begin(), polySource.size(), normalLength);
    ofxPolyGrowAlongNormals(poly, polySource, thicknesses);
}

void ofxPolyGrowAlongNormals(ofPolyline & poly, const ofPolyline & polySource, const vector<float> & normalLengths) {

    poly = polySource;

    if(poly.size() < 2) {
        return;
    }
    
    vector<ofVec3f> & points = poly.getVertices();
    int numOfPoints = points.size();
    
    for(int i=0; i<numOfPoints; i++) {
        
        float normalLength = 0.0;
        if(i < normalLengths.size()) {
            normalLength = normalLengths[i];
        }
        
        ofVec3f & point = points[i];
        ofVec3f normal = poly.getNormalAtIndex(i);
        point += (normal * normalLength);
    }
}

//--------------------------------------------------------------
void ofxPolyToMesh(ofMesh & mesh, const ofPolyline & polySource, float normalLength) {
    
    float normalLength0 = -normalLength;
    float normalLength1 = normalLength;
    
    ofPolyline poly0, poly1;
    ofxPolyGrowAlongNormals(poly0, polySource, normalLength0);
    ofxPolyGrowAlongNormals(poly1, polySource, normalLength1);
    
    ofxPolyToMesh(mesh, poly0, poly1);
}

void ofxPolyToMesh(ofMesh & mesh, const ofPolyline & polySource, const vector<float> & normalLengths) {

    vector<float> normalLengths0;
    vector<float> normalLengths1;
    
    for(int i=0; i<normalLengths.size(); i++) {
        float normalLength = normalLengths[i];
        float normalLength0 = -normalLength;
        float normalLength1 = normalLength;
        
        normalLengths0.push_back(normalLength0);
        normalLengths1.push_back(normalLength1);
    }
    
    ofPolyline poly0, poly1;
    ofxPolyGrowAlongNormals(poly0, polySource, normalLengths0);
    ofxPolyGrowAlongNormals(poly1, polySource, normalLengths1);
    
    ofxPolyToMesh(mesh, poly0, poly1);
}

void ofxPolyToMesh(ofMesh & mesh, const ofPolyline & poly0, const ofPolyline & poly1) {
    
    mesh.clear();
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    int numOfPoints = MIN(poly0.size(), poly1.size());
    int numOfCycles = numOfPoints;
    if(poly0.isClosed() == true) {
        numOfCycles += 1;
    }
    
    for(int i=0; i<numOfCycles; i++) {
        int j = i % numOfPoints;
        const ofVec3f & p0 = poly0.getVertices()[j];
        const ofVec3f & p1 = poly1.getVertices()[j];
        
        mesh.addVertex(p0);
        mesh.addVertex(p1);
    }
}

//--------------------------------------------------------------
void ofxPolyDrawNormals(const ofPolyline & poly, float normalLength) {

    const vector<ofVec3f> & points = poly.getVertices();
    
    for(int i=0; i<points.size(); i++) {
        const ofVec3f & point = points[i];
        ofVec3f normal = poly.getNormalAtIndex(i);

        ofDrawLine(point, point + (normal * normalLength));
    }
}

//--------------------------------------------------------------
void ofxPolySave(const ofPolyline & poly, string xmlPath) {
    ofXml xml;
    ofXml data = xml.appendChild("poly");
    data.appendChild("closed").set(poly.isClosed());
    for(auto & point : poly.getVertices()) {
        ofXml p = data.appendChild("point");
        p.setAttribute("x", point.x);
        p.setAttribute("y", point.y);
    }
    xml.save(xmlPath);
}

//--------------------------------------------------------------
bool ofxPolyLoad(ofPolyline & poly, string xmlPath) {
    ofXml xml;
    if(xml.load(xmlPath)) {
        auto data = xml.getChild("poly");
        bool bClosed = data.getChild("closed").getBoolValue();
        
        poly.clear();
        for(auto & p : data.getChildren("point")) {        
            float x = p.getAttribute("x").getFloatValue();
            float y = p.getAttribute("y").getFloatValue();
            poly.addVertex(x, y);
        }
        poly.setClosed(bClosed);
        return true;
    } else {
        return false;
    }
}
    

