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

    const auto & points = polySource.getVertices();
    size_t numOfPoints = points.size();

    bool bClosed = true;
    bClosed = bClosed && (polySource.isClosed() == true);
    bClosed = bClosed && (numOfPoints >= 3);

    for(size_t i=0; i<numOfPoints; i++) {

        bool bEndings = false;
        bEndings = bEndings || (i == 0);
        bEndings = bEndings || (i == numOfPoints-1);
        bEndings = bEndings && (bClosed == false);

        if(bEndings == true) {

            const auto & p0 = points[i];
            auto n0 = polySource.getNormalAtIndex(static_cast<int>(i));
            auto point = p0 + (n0 * amount);
            poly.addVertex(point);

            continue;
        }

        size_t i0 = i == 0 ? numOfPoints - 1 : i - 1;

        const auto & p0 = points[i0];
        const auto & p1 = points[i];

        auto n0 = ofVec2f(p0 - p1).getPerpendicular();
        auto n1 = polySource.getNormalAtIndex(static_cast<int>(i));

        float angle = ofVec2f(n0).angle(ofVec2f(n1));
        float length = amount / cosf(ofDegToRad(angle));

        auto point = p1 + (n1 * length);
        poly.addVertex(point);
    }
}

//--------------------------------------------------------------
void ofxPolyGrowAlongNormals(ofPolyline & poly, const ofPolyline & polySource, float normalLength) {
    std::vector<float> thicknesses;
    thicknesses.insert(thicknesses.begin(), polySource.size(), normalLength);
    ofxPolyGrowAlongNormals(poly, polySource, thicknesses);
}

void ofxPolyGrowAlongNormals(ofPolyline & poly, const ofPolyline & polySource, const std::vector<float> & normalLengths) {

    poly = polySource;

    if(poly.size() < 2) {
        return;
    }

    auto & points = poly.getVertices();
    size_t numOfPoints = points.size();

    for(size_t i=0; i<numOfPoints; i++) {

        float normalLength = 0.0;
        if(i < normalLengths.size()) {
            normalLength = normalLengths[i];
        }

        auto & point = points[i];
        auto normal = poly.getNormalAtIndex(static_cast<int>(i));
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

void ofxPolyToMesh(ofMesh & mesh, const ofPolyline & polySource, const std::vector<float> & normalLengths) {

    std::vector<float> normalLengths0;
    std::vector<float> normalLengths1;

    for(size_t i=0; i<normalLengths.size(); i++) {
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

    size_t numOfPoints = MIN(poly0.size(), poly1.size());
    size_t numOfCycles = numOfPoints;
    if(poly0.isClosed() == true) {
        numOfCycles += 1;
    }

    for(size_t i=0; i<numOfCycles; i++) {
        size_t j = i % numOfPoints;
        const auto & p0 = poly0.getVertices()[j];
        const auto & p1 = poly1.getVertices()[j];

        mesh.addVertex(p0);
        mesh.addVertex(p1);
    }
}

//--------------------------------------------------------------
void ofxPolyDrawNormals(const ofPolyline & poly, float normalLength) {

    const auto & points = poly.getVertices();

    for(size_t i=0; i<points.size(); i++) {
        const auto & point = points[i];
        auto normal = poly.getNormalAtIndex(static_cast<int>(i));

        ofDrawLine(point, point + (normal * normalLength));
    }
}

//--------------------------------------------------------------
void ofxPolySave(const ofPolyline & poly, std::string xmlPath) {
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
bool ofxPolyLoad(ofPolyline & poly, std::string xmlPath) {
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


