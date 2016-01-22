//
//  定数クラス
//

#pragma once

//enum VIDEO_MODES {
//    VIDEO_MOVIE = 0,
//    VIDEO_CAMERA = 1
//};

class Const
{
public:
    
    // -------- //
    // mode
    // -------- //
//    const static int VIDEO_MODE = VIDEO_CAMERA;
//    const static int VIDEO_DEVICE_ID = 1;

    
    //----------//
    // Kinect Size
    //----------//
    const static int DEPTH_WIDTH = 512;
    const static int DEPTH_HEIGHT = 424;
    
    const static int COLOR_WIDTH = 1920;
    const static int COLOR_HEIGHT = 1080;
    
    
    
    // -------- //
    // layouts
    // -------- //
    // videoの元の大きさ
    const static int WIDTH = 1920;
    const static int HEIGHT = 1080;
    
    // マージン
    const static int W_MARGIN = 30;
    const static int H_MARGIN = 30;
    constexpr static float DEPTH_SCALE = 1.0;
    constexpr static float COLOR_SCALE = 0.3;

    // -------- //
    // file path
    // -------- //
    const static string QUADWARP_XML;
    const static string GUI_XML;
    
};

// string系はココ
const string Const::QUADWARP_XML = "xml/QuadWarp.xml";
const string Const::GUI_XML = "xml/GuiParamsTest.xml";
