//
//  Created by Tatsuya Kida
//
#pragma once

#include "ofProtonect2.h"
#include "registration.h"

class RegistrationWrapper
{

protected:
    libfreenect2::Freenect2Device::IrCameraParams depthParam;
    libfreenect2::Freenect2Device::ColorCameraParams colorParam;
    libfreenect2::Registration * registration;
    
    
public:
    void setup(ofProtonect2* protonect)
    {
        depthParam = protonect->getIrCameraParams();
        colorParam = protonect->getColorCameraParams();
        registration = new libfreenect2::Registration(&depthParam, &colorParam);
    }
    
    void depthToColor(int dx, int dy, float dz, float &cx, float &cy)
    {
        cout << "dx:" << dx << "/ dy:" << dy << "/ dz:" << dz << endl;
        
        registration->apply(dx, dy, dz, cx, cy);
    }

};