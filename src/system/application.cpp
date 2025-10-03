#include "application.h"
#include "icons_font.h"
#include "browser_akao.h"
#include "browser_ending.h"
#include "browser_field.h"
#include "browser_field_opcodes.h"
#include "psyq/libgpu.h"



std::shared_ptr<ofAppBaseWindow> g_application;
ofFbo g_screen;



void GameRender()
{
    ofTexture texture;
    texture.allocate( 1024, 512, GL_RGBA );

    const u16* src = reinterpret_cast<const u16*>(g_vram.data());

    ofPixels pixels;
    pixels.allocate( 1024, 512, OF_PIXELS_RGBA );

    for( int i = 0; i < 1024 * 512; ++i )
    {
        u16 color = src[i];

        // Извлекаем компоненты цвета из RGB565
        u8 r = ((color >> 11) & 0x1f) << 3;
        u8 g = ((color >> 5) & 0x3f) << 2;
        u8 b = (color & 0x1f) << 3;

        // Устанавливаем RGBA значения
        int index = i * 4;
        pixels[index + 0] = r; // Red
        pixels[index + 1] = g; // Green
        pixels[index + 2] = b; // Blue
        pixels[index + 3] = 255; // Alpha (полностью непрозрачный)
    }

    // Загружаем данные в текстуру
    texture.loadData( pixels );

    g_screen.begin();

    // Рисуем текстуру на конкретных координатах
    //ofEnableAlphaBlending();
    //ofSetColor( 255 );
    int x = 0; // Начальная координата X для размещения текстуры
    int y = 0; // Начальная координата Y для размещения текстуры

    texture.draw( x, y, 1024, 512 );

    g_screen.end();



    if( g_application->getWindowShouldClose() )
    {
        std::exit( EXIT_SUCCESS );
    }

    ofGetMainLoop()->loopOnce();
    ofGetMainLoop()->pollEvents();
}



void Application::setup()
{
    ofSetLogLevel(OF_LOG_VERBOSE);

    gui.setup( nullptr, true, ImGuiConfigFlags_ViewportsEnable );
    // set imGui style
    {
        ImGui::StyleColorsDark();
        ImGuiStyle* style = &ImGui::GetStyle();

        style->WindowRounding = 0.0f;
        style->FrameRounding = 2.0f;
        style->TabRounding = 4.0f;
        style->ScrollbarRounding = 1.6f;
        style->ScrollbarSize = 18.0f;
        style->FramePadding = ImVec2( 3.0f, 3.0f );
        style->WindowBorderSize = 0.1f;

        ImVec4* colors = style->Colors;

        colors[ImGuiCol_Text]                   = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
        colors[ImGuiCol_TextDisabled]           = ImVec4( 0.70f, 0.70f, 0.70f, 1.00f );
        colors[ImGuiCol_WindowBg]               = ImVec4( 0.15f, 0.15f, 0.19f, 0.96f );
//        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//        colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
//        colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
//        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4( 0.35f, 0.35f, 0.35f, 0.54f );
//        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
//        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
//        colors[ImGuiCol_TitleBg]                = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
//        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
//        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
//        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.15f, 0.15f, 0.15f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4( 0.45f, 0.45f, 0.45f, 1.00f );
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4( 0.70f, 0.70f, 0.70f, 1.00f);
//        colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
//        colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
//        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
//        colors[ImGuiCol_Button]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
//        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
//        colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
//        colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
//        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
//        colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator]              = colors[ImGuiCol_Border];
//        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
//        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4( 0.30f, 0.30f, 0.30f, 0.20f );
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4( 0.30f, 0.30f, 0.30f, 0.67f );
        colors[ImGuiCol_ResizeGripActive]       = ImVec4( 0.30f, 0.30f, 0.30f, 0.95f );
        colors[ImGuiCol_InputTextCursor]        = colors[ImGuiCol_Text];
        colors[ImGuiCol_TabHovered]             = colors[ImGuiCol_HeaderHovered];
//        colors[ImGuiCol_Tab]                    = ImLerp(colors[ImGuiCol_Header],       colors[ImGuiCol_TitleBgActive], 0.80f);
//        colors[ImGuiCol_TabSelected]            = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
//        colors[ImGuiCol_TabSelectedOverline]    = colors[ImGuiCol_HeaderActive];
//        colors[ImGuiCol_TabDimmed]              = ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f);
//        colors[ImGuiCol_TabDimmedSelected]      = ImLerp(colors[ImGuiCol_TabSelected],  colors[ImGuiCol_TitleBg], 0.40f);
//        colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.50f, 0.50f, 0.50f, 0.00f);
//        colors[ImGuiCol_DockingPreview]         = colors[ImGuiCol_HeaderActive] * ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
//        colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
//        colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
//        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
//        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
//        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
//        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
//        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);   // Prefer using Alpha=1.0 here
//        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);   // Prefer using Alpha=1.0 here
//        colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//        colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
//        colors[ImGuiCol_TextLink]               = colors[ImGuiCol_HeaderActive];
//        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
//        colors[ImGuiCol_TreeLines]              = colors[ImGuiCol_Border];
//        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
//        colors[ImGuiCol_NavCursor]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
//        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
//        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
//        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }

    // imGui fonts load and settings
    {
        ImGuiIO& io = ImGui::GetIO();
        ImFont* font = io.Fonts->AddFontFromFileTTF( "system/Roboto-Regular.ttf", 16.0f );
        //ImFont* font = io.Fonts->AddFontFromFileTTF( "system/RobotoMono-Regular.ttf", 17.0f );
        if( !font ) ofLogError() << "ERROR: can't load font.";
        io.FontDefault = font;
        ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        if( !io.Fonts->AddFontFromFileTTF( "system/FontAwesome5-Free-Solid.otf", 14.0f, &icons_config, icons_ranges ) )
        {
            ofLogError() << "ERROR: can't load font.";
        }

        ImWchar jp_ranges[] =
        {
            0x0020, 0x00ff, // Basic Latin
            0x3040, 0x309f, // Hiragana
            0x30a0, 0x30ff, // Katakana
            0x4e00, 0x9faf, // Kanji (CJK Unified Ideographs)
            0,
        };
        io.Fonts->AddFontFromFileTTF( "system/NotoSansCJKjp-Regular.otf", 18.0f, &icons_config, jp_ranges );
        io.Fonts->Build();
    }

    backgroundColor = ofColor( 255, 255, 255 );

    g_screen.allocate( 800, 600, GL_RGBA );
    g_screen.begin();
    ofClear( 255, 0, 255, 255 );
    g_screen.end();

    FieldBrowserInitOpcode();
}



void Application::update()
{
}



void Application::draw()
{
    gui.begin(); //required to call this at beginning

    //g_screen.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    g_screen.draw( 0, 0, 640, 480 );

    BrowserField();
    BrowserEnding();
    BrowserAkao();

    if( ImGui::BeginMainMenuBar() )
    {
        if( ImGui::BeginMenu( "Field" ) )
        {
            g_browser_field = true;
            ImGui::EndMenu();
        }

        if( ImGui::BeginMenu( "Ending" ) )
        {
            g_browser_ending = true;
            ImGui::EndMenu();
        }

        if( ImGui::BeginMenu( "Akao" ) )
        {
            g_browser_akao = true;
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    g_screen.begin();
    ofClear( 100, 100, 100, 255 );
    g_screen.end();

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
