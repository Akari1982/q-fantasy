#include "application.h"
#include "system/akao_debug.h"

#include "ofxImGui.h"

#include <filesystem>
#include "IconsFontAwesome5.h"



ofFbo g_GameVram;



void Application::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);

    gui.setup( nullptr, true, ImGuiConfigFlags_ViewportsEnable );
    ImGui::StyleColorsDark();

    // imGui fonts load and settings
    {
        ImGuiIO& io = ImGui::GetIO();
        //ImFont* font = io.Fonts->AddFontFromFileTTF( "system/Roboto-Regular.ttf", 15.0f );
        ImFont* font = io.Fonts->AddFontFromFileTTF( "system/RobotoMono-Regular.ttf", 17.0f );
        if( !font ) ofLogError() << "ERROR: can't load font.";
        io.FontDefault = font;
        static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        if( !io.Fonts->AddFontFromFileTTF( "system/FontAwesome5-Free-Solid.otf", 14.0f, &icons_config, icons_ranges ) )
        {
            ofLogError() << "ERROR: can't load font.";
        }
        io.Fonts->Build();
    }

    backgroundColor = ofColor( 255, 255, 255 );

    g_GameVram.allocate( 800, 600, GL_RGBA );
    g_GameVram.begin();
    ofClear( 255, 0, 255, 255 );
    g_GameVram.end();
}



void Application::update()
{
}



void Application::draw()
{
    gui.begin(); //required to call this at beginning

    //g_GameVram.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    g_GameVram.draw( 0, 0, 640, 480 );

    AkaoDebug();

    std::string popup_to_open = "";
    if( ImGui::BeginMainMenuBar() )
    {
        if( ImGui::BeginMenu( "AkaoDebug" ) )
        {
            g_akao_debug = true;
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    g_GameVram.begin();
    ofClear( 100, 100, 100, 255 );
    g_GameVram.end();

    gui.end(); //required to call this at end
}



void Application::keyPressed( int key ) {}
void Application::keyReleased( int key ) {}
void Application::mouseMoved( int x, int y ) {}
void Application::mouseDragged( int x, int y, int button ) {}
void Application::mousePressed( int x, int y, int button ) {}
void Application::mouseReleased( int x, int y, int button ) {}
void Application::mouseEntered( int x, int y ) {}
void Application::mouseExited( int x, int y ) {}
void Application::windowResized( int w, int h ) {}
void Application::gotMessage( ofMessage msg ) {}
void Application::dragEvent( ofDragInfo dragInfo ){}
