#pragma once

#include "ofMain.h"

// my codes
#include "Const.h"

// addon
#include "ofxMultiKinectV2.h"
#include "ofxBlur.h"
#include "ofxGui.h"
#include "ofxCv.h"
#include "ofxNanoVG.h"

#include "RegistrationWrapper.h"

class ofApp : public ofBaseApp
{
public:
    
    // vars
    ofxMultiKinectV2 _kinect;
    ofTexture _depthTex;
    ofTexture _colorTex;
    ofxBlur _depthBlur;
    ofShader _depthShader;
    ofxCv::ContourFinder _contour;
    ofxNanoVG::Canvas _nanoDepth;
    ofxNanoVG::Canvas _nanoColor;
    ofFbo _cutDepthFbo;
    RegistrationWrapper _rw;
    
    // vars for GUI
    ofxPanel _gui;
    ofParameter <int> _pKinectMinDist;
    ofParameter <int> _pKinectMaxDist;
    ofParameter <float> _pBlur;
    ofParameter <int> _pCvMinArea;
    ofParameter <int> _pCvMaxArea;
    ofParameter <int> _pCvThreshold;
    ofParameter <int> _pCvPersistance;
    ofParameter <int> _pCvMaxDist;


    
    void setup()
    {
        // window
        ofSetVerticalSync(true);
        ofSetFrameRate(60);
        ofSetLogLevel(OF_LOG_NOTICE);
        
        
        // kinect
        _kinect.open(true, true, 0);
        _kinect.start();
        
        // gr
        _rw.setup(_kinect.getProtonect());

        // shader
        _depthShader.load("shaders/depth.vert","shaders/depth.frag");
        
        
        // blur
        _depthBlur.setup(Const::DEPTH_WIDTH, Const::DEPTH_HEIGHT, 10, .2, 4);;
        
        
        // cut depth fbo
        _cutDepthFbo.allocate(Const::DEPTH_WIDTH * Const::DEPTH_SCALE, Const::DEPTH_HEIGHT * Const::DEPTH_SCALE);
        
        
        // nanovg
        _nanoDepth.allocate(Const::DEPTH_WIDTH * Const::DEPTH_SCALE, Const::DEPTH_HEIGHT * Const::DEPTH_SCALE);
        assert(_nanoDepth.loadFont("fonts/GenShinGothic-Heavy.ttf", "sans"));
        _nanoColor.allocate(Const::COLOR_WIDTH * Const::COLOR_SCALE, Const::COLOR_HEIGHT * Const::COLOR_SCALE);

        
        // gui
        _gui.setup("params", Const::GUI_XML, Const::W_MARGIN, Const::DEPTH_HEIGHT * Const::DEPTH_SCALE + Const::H_MARGIN*2 );
        _gui.add( _pKinectMinDist.set("KinectMinDist", 1, 300, 800) );
        _gui.add( _pKinectMaxDist.set("KinectMaxDist", 1000, 300, 1500) );
        _gui.add( _pBlur.set("KinectBlur", 1.0, 0, 4.0) );
        _gui.add( _pCvMinArea.set("cv_min_area", 10.0, 0.0, 50.0) );
        _gui.add( _pCvMaxArea.set("cv_max_area", 10, 0, 400) );
        _gui.add( _pCvThreshold.set("cv_threshold", 10, 0, 255) );
        _gui.add( _pCvPersistance.set("cv_persistance", 10, 0, 100) );
        _gui.add( _pCvMaxDist.set("cv_max_dist", 10, 0, 100) );
        _gui.loadFromFile(Const::GUI_XML);
    }
    
    
    void update()
    {
        //
        _kinect.update();
        if (_kinect.isFrameNew())
        {
            _colorTex.loadData( _kinect.getColorPixelsRef() );
            _depthTex.loadData( _kinect.getDepthPixelsRef() );
        }
        

        // blur setting
        _depthBlur.setScale( _pBlur );
        
        
        // depth texture
        if( _depthTex.isAllocated() )
        {
            _depthBlur.begin();
            _depthShader.begin();
            {
                _depthShader.setUniform1f("KinectMinDist", _pKinectMinDist);
                _depthShader.setUniform1f("KinectMaxDist", _pKinectMaxDist);
                
                _depthTex.draw(0,0,Const::DEPTH_WIDTH,Const::DEPTH_HEIGHT);
            }
            _depthShader.end();
            _depthBlur.end();
        }
        

        
        // contour settings
        _contour.setMinAreaRadius( _pCvMinArea );
        _contour.setMaxAreaRadius( _pCvMaxArea );
        _contour.setThreshold( _pCvThreshold );
        _contour.getTracker().setPersistence( _pCvPersistance );
        _contour.getTracker().setMaximumDistance( _pCvMaxDist );
        
        
        // contour finder
        ofTexture tex = _depthBlur.getTextureReference();
        ofPixels p;
        tex.readToPixels(p);
        ofImage img;
        img.setFromPixels(p, Const::DEPTH_WIDTH, Const::DEPTH_HEIGHT, OF_IMAGE_COLOR_ALPHA);
        _contour.findContours( img );
        
        
        // nanovg
        ofxNanoVG::Canvas& c = _nanoDepth;
        c.begin();
        {
            // color
            ofColor color;
            color.set(255);
            c.strokeColor(color);
            
            // draw contour
            c.translate(0, 0);
            int contourSize = _contour.size();
            for(int polylineId = 0; polylineId < contourSize; polylineId++)
            {
                // color
                c.strokeColor( ofColor::fromHsb(0, 255, 255) );
                
                
                // get polyline
                ofPolyline polyline = _contour.getPolyline(polylineId);
                vector<ofPoint> points = polyline.getVertices();
                ofPoint center = polyline.getCentroid2D();
            
                // draw rectangle
                c.beginPath();
                {
                    c.lineWidth(2);
                    c.rect(polyline.getBoundingBox());
                }
                c.strokePath();
            }
        }
        c.end();
        
        
        // canvas nanovg
        ofxNanoVG::Canvas& c2 = _nanoColor;
        c2.begin();
        {
            // color
            ofColor color;
            color.set(255, 100, 100);
            c2.strokeColor(color);
            c2.translate(0, 0);
            int contourSize = _contour.size();
            for(int polylineId = 0; polylineId < contourSize; polylineId++)
            {
                // color
                c.strokeColor( ofColor::fromHsb(0, 255, 255) );
                
                // get polyline
                ofPolyline polyline = _contour.getPolyline(polylineId);
                ofRectangle rect = polyline.getBoundingBox();
                
                // draw rectangle
                c2.beginPath();
                {
                    c2.lineWidth(2);
                    
                    //
                    float dx = rect.getX();
                    float dy = rect.getY();
                    float dw = rect.getWidth();
                    float dh = rect.getHeight();
                    
                    float z1 = _kinect.getDistanceAt(dx, dy);
                    float z2 = _kinect.getDistanceAt(dx+dw, dy);
                    float z3 = _kinect.getDistanceAt(dx+dw, dy+dh);
                    float z4 = _kinect.getDistanceAt(dx, dy+dh);
                    
                    int zcount = 0;
                    float zsum = 0;
                    if( 0 < z1 ) { zsum += z1; zcount += 1; }
                    if( 0 < z2 ) { zsum += z2; zcount += 1; }
                    if( 0 < z3 ) { zsum += z3; zcount += 1; }
                    if( 0 < z4 ) { zsum += z4; zcount += 1; }
                    float zaverage = zsum / zcount;
   
//                    cout << " - - - - " << endl;
//                    cout << dx << "," << dy << "," << dw << "," << dh << endl;
                    
                    //
                    float cx1, cy1, cx2, cy2, cx3, cy3, cx4, cy4;
                    _rw.depthToColor(dx, dy, zaverage, cx1, cy1);
                    _rw.depthToColor(dx+dw, dy, zaverage, cx2, cy2);
                    _rw.depthToColor(dx+dw, dy+dh, zaverage, cx3, cy3);
                    _rw.depthToColor(dx, dy+dh, zaverage, cx4, cy4);
                    
                    vector<ofVec2f> points;
                    if( !isinf(cx1) && !isinf(cy1) ){ points.push_back(ofVec2f(cx1,cy1)); }
                    if( !isinf(cx2) && !isinf(cy2) ){ points.push_back(ofVec2f(cx2,cy2)); }
                    if( !isinf(cx3) && !isinf(cy3) ){ points.push_back(ofVec2f(cx3,cy3)); }
                    if( !isinf(cx4) && !isinf(cy4) ){ points.push_back(ofVec2f(cx4,cy4)); }
                    
                    int len = points.size();
                    for( int i = 0; i < len; i++ )
                    {
                        if(i == 0 )c2.moveTo(points[i].x * Const::COLOR_SCALE, points[i].y * Const::COLOR_SCALE);
                        else c2.lineTo(points[i].x * Const::COLOR_SCALE, points[i].y * Const::COLOR_SCALE);
                    }
                    c2.closePath();
                    
//                    cout << " - -  " << endl;
//                    cout << cx1 << "," << cx1 << endl;
//                    cout << cx2 << "," << cy2 << endl;
//                    cout << cx3 << "," << cy3 << endl;
//                    cout << cx4 << "," << cy4 << endl;
                }
                c2.strokePath();
            }
        }
        c2.end();
    }
    
    void draw()
    {
        // init
        ofClear(100);
        ofSetColor(255);
    
        // depth
        _depthBlur.draw( ofRectangle(Const::W_MARGIN, Const::H_MARGIN, Const::DEPTH_WIDTH * Const::DEPTH_SCALE , Const::DEPTH_HEIGHT * Const::DEPTH_SCALE ) );
        _nanoDepth.draw(Const::W_MARGIN, Const::H_MARGIN);

        // color
        if( _colorTex.isAllocated() )
        {
            _colorTex.draw(Const::W_MARGIN*2 + Const::DEPTH_WIDTH * Const::DEPTH_SCALE,
                           Const::H_MARGIN ,
                           Const::COLOR_WIDTH * Const::COLOR_SCALE,
                           Const::COLOR_HEIGHT * Const::COLOR_SCALE);
            
            _nanoColor.draw(Const::W_MARGIN*2 + Const::DEPTH_WIDTH * Const::DEPTH_SCALE,
                            Const::H_MARGIN ,
                            Const::COLOR_WIDTH * Const::COLOR_SCALE,
                            Const::COLOR_HEIGHT * Const::COLOR_SCALE);
        }
        
        // gui
        _gui.draw();
        
        // settings
        ofSetColor(255);
        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 10, 20);
    }

};
