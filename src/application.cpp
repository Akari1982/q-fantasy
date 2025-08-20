#include "application.h"
#include "system/akao_debug.h"

#include <filesystem>



ofFbo g_GameVram;



void Application::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);

    gui.setup( nullptr, true, ImGuiConfigFlags_ViewportsEnable );

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

    AkaoDebugSequence();

/*
    std::string popup_to_open = "";
    if( ImGui::BeginMainMenuBar() )
    {
        if( ImGui::BeginMenu( "AKAO" ) )
        {
            if( ImGui::BeginListBox( "Akao list" ) )
            {
                std::string path = "./data/FIELD";

                for( const auto& entry : std::filesystem::directory_iterator( path ) )
                {
                    if( entry.is_regular_file() )
                    {
                        auto ext = entry.path().extension().string();

                        // Приведение к верхнему регистру для нечувствительности к регистру
                        for( auto& c : ext ) c = std::toupper( static_cast<unsigned char>( c ) );

                        if( ext == ".DAT" )
                        {
                            bool is_selected = false;

                            if( ImGui::Selectable( entry.path().filename().string().c_str(), &is_selected))
                            {
                            }
                        }
                    }
                }

                ImGui::EndListBox();
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
*/

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
