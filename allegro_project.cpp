#include "vv_utils.h"
#include "allegro_project.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"


ALLEGRO_FONT* allegro_project::m_system_font = nullptr;

allegro_project::allegro_project() {}

allegro_project::~allegro_project()
{
    if (m_imgui_enabled)
    {
        ImGui_ImplAllegro5_Shutdown();
        ImGui::DestroyContext();
    }
    if (m_display)
    {
        al_destroy_display(m_display);
        m_display = nullptr;
    }
    if (m_event_queue)
    {
        al_flush_event_queue(m_event_queue);
        al_destroy_event_queue(m_event_queue);
        m_event_queue = nullptr;
    }
    if (m_fps)
    {
        al_stop_timer(m_fps);
        al_destroy_timer(m_fps);
        m_fps = nullptr;
    }
    if (m_system_font)
    {
        al_destroy_font(m_system_font);
        m_system_font = nullptr;
    }
}

void allegro_project::init(int display_flags, bool enable_imgui)
{
    BEGIN_EXCEPTION_CATCH()
    // Basic initialization
    m_imgui_enabled = enable_imgui;
    if (!al_init())
        throw "couldn't init allegro!";

    m_event_queue = al_create_event_queue();
    if (!m_event_queue)
        throw "couldn't start event queue!";
    if (!init_fps_timer())
        throw "couldn't init timer!";
    al_start_timer(m_fps);

    if(!al_install_keyboard())
        throw "couldn't install keyboard!";
    al_register_event_source(m_event_queue, al_get_keyboard_event_source());
    al_get_keyboard_state(&m_keyboard_state);

    if(!al_install_mouse())
        throw "could't install mouse!";
    al_register_event_source(m_event_queue, al_get_mouse_event_source());
    al_get_mouse_state(&m_mouse_state);
    m_prev_mouse_state = m_mouse_state;

    // Set display flags
    al_set_new_display_flags(display_flags);

    // Set depth buffer size GLX_DEPTH_SIZE
    al_set_new_display_option(ALLEGRO_DEPTH_SIZE, 16, ALLEGRO_SUGGEST);

    // Enable antialiasing
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 2, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    // Init Addons
    if (!al_init_primitives_addon())
        throw "couldn't init primitives addon!";

    if (!al_init_image_addon())
        throw "couldn't init image addon!";

    al_init_font_addon();
    al_init_ttf_addon();
    m_system_font = al_load_ttf_font("basis33.ttf", 16, 0);
    if (!m_system_font)
        throw "system font is not initialized!";

    if (m_imgui_enabled)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.Fonts->AddFontFromFileTTF("basis33.ttf", 16, nullptr, io.Fonts->GetGlyphRangesCyrillic());
        ImGui::StyleColorsDark();
    }

    m_init = true;
    END_EXCEPTION_CATCH();
}

void allegro_project::create_display(int w, int h)
{
    BEGIN_EXCEPTION_CATCH()
    if (!m_init)
        throw "Allegro project is not initialized!";
    if (!m_event_queue || m_display)
        throw "event queue is not initialised or display is already created!";

    m_display = al_create_display(w, h);
    if (!m_display)
        throw "couldn't create display!";

    al_register_event_source(m_event_queue, al_get_display_event_source(m_display));
    display_resize(w, h);

    if (m_imgui_enabled)
        ImGui_ImplAllegro5_Init(m_display);

    END_EXCEPTION_CATCH()
}

void allegro_project::pre_render()
{
    if (m_imgui_enabled)
    {
        ImGui_ImplAllegro5_NewFrame();
        ImGui::NewFrame();
    }
}

void allegro_project::render()
{
    if (m_imgui_enabled)
        imgui_render();
    else
        al_clear_to_color(al_map_rgb(0, 148, 204));
}

void allegro_project::post_render()
{
    if (m_imgui_enabled)
    {
        ImGui::End();
        ImGui::Render();
        ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());
    }
}

void allegro_project::imgui_render()
{
    if (!m_imgui_enabled)
        return;
}

bool allegro_project::init_fps_timer(double speed_sec)
{
    if (!m_event_queue)
        return false;
    if (m_fps)
        al_destroy_timer(m_fps);
    m_fps = al_create_timer(speed_sec);
    if (!m_fps)
        return false;

    al_register_event_source(m_event_queue, al_get_timer_event_source(m_fps));
    return true;
}

void allegro_project::keyboard_event_handler(const ALLEGRO_EVENT& ev)
{
    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
    {
        throw "ESC pressed!";
        return;
    }
}

void allegro_project::check_input_state()
{
    m_prev_mouse_state = m_mouse_state;
    m_prev_keyboard_state = m_keyboard_state;
    al_get_mouse_state(&m_mouse_state);
    al_get_keyboard_state(&m_keyboard_state);
}

void allegro_project::display_resize(int w, int h)
{
    m_w = w;
    m_h = h;
}

void allegro_project::main_loop()
{
    BEGIN_EXCEPTION_CATCH()
    if (!m_init)
        throw "Allegro openGL project is not initialized!";
    if (!m_event_queue)
        return;
    ALLEGRO_EVENT ev;
    bool drawing_enabled = false;
    while (true)
    {
        al_wait_for_event(m_event_queue, &ev);
        if (m_imgui_enabled)
            ImGui_ImplAllegro5_ProcessEvent(&ev);
        switch (ev.type)
        {
        case ALLEGRO_EVENT_TIMER:
            if (m_display)
                drawing_enabled = true;
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
        {
            return;
            break;
        }
        case ALLEGRO_EVENT_KEY_DOWN:
        {
            keyboard_event_handler(ev);
            break;
        }
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
        {
            al_acknowledge_resize(ev.display.source);
            display_resize(ev.display.width, ev.display.height);
            if (m_imgui_enabled)
            {
                ImGui_ImplAllegro5_InvalidateDeviceObjects();
                ImGui_ImplAllegro5_CreateDeviceObjects();
            }
            break;
        }
        default:
            break;
        }

        if (drawing_enabled && al_event_queue_is_empty(m_event_queue))
        {
            drawing_enabled = false;
            check_input_state();
            pre_render();
            render();
            post_render();
            al_flip_display();
        }
    }
    END_EXCEPTION_CATCH()
}

const ALLEGRO_FONT* allegro_project::get_system_font()
{
    return m_system_font;
}

void allegro_project::allegro_check_version()
{
    uint32_t version = al_get_allegro_version();
    int major = version >> 24;
    int minor = (version >> 16) & 255;
    int revision = (version >> 8) & 255;
    int release = version & 255;
    std::cout << std::endl << "allegro version: " << major << "." << minor << "." << revision << "." << release << std::endl;
}

#ifdef ALLEGRO_PROJECT_OPENGL
// allegro_opengl_project implementation ////////////////////////////////

static bool g_show_demo_window(false);
bool allegro_opengl_project::draw_state_flags::m_shaded    = false;
bool allegro_opengl_project::draw_state_flags::m_wireframe = false;
bool allegro_opengl_project::draw_state_flags::m_compas    = false;
bool allegro_opengl_project::draw_state_flags::m_coord_sys = false;

void allegro_opengl_project::create_display(int w, int h)
{
    allegro_project::create_display(w, h);
    display_resize(w, h);
    m_camera.translate(0, 0, -10);
    //m_camera.rotate(180, 0, 0);
}

void allegro_opengl_project::display_resize(int w, int h)
{
    allegro_project::display_resize(w, h);
    glViewport(0, 0, m_w, m_h);
    m_camera.init_projection(45, 1, 100, static_cast<double>(m_w) / m_h);
}

void allegro_opengl_project::check_input_state()
{
    allegro_project::check_input_state();

    double rot_scale = 1.0;
    const double zoom_scale = 0.5;
    const double pan_scale = 0.01;

    double dx = m_prev_mouse_state.x - m_mouse_state.x;
    double dy = m_prev_mouse_state.y - m_mouse_state.y;
    double dz = m_prev_mouse_state.z - m_mouse_state.z;

    m_camera.translate(0, 0, -dz * zoom_scale);

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_R))
    {
        m_camera.reset();
        m_camera.translate(0, 0, -10);
        //m_camera.rotate(180, 0, 0);
    }

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_RCTRL) ||
            al_key_down(&m_keyboard_state, ALLEGRO_KEY_LCTRL))
    {
        //show ImGui demo window
        if (!al_key_down(&m_prev_keyboard_state, ALLEGRO_KEY_D) &&
                al_key_down(&m_keyboard_state, ALLEGRO_KEY_D))
            g_show_demo_window = !g_show_demo_window;
    }

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_UP))
        m_camera.apply_rotation(vv_geom::quat::from_axis_angle({1.0, 0.0, 0.0}, -M_PI / 180 * rot_scale));
    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_DOWN))
        m_camera.apply_rotation(vv_geom::quat::from_axis_angle({1.0, 0.0, 0.0}, M_PI / 180 * rot_scale));
    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_LEFT))
        m_camera.apply_rotation(vv_geom::quat::from_axis_angle({0.0, 1.0, 0.0}, -M_PI / 180 * rot_scale));
    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_RIGHT))
        m_camera.apply_rotation(vv_geom::quat::from_axis_angle({0.0, 1.0, 0.0}, M_PI / 180 * rot_scale));

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_RSHIFT) ||
            al_key_down(&m_keyboard_state, ALLEGRO_KEY_LSHIFT))
    {
        if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_UP))
            m_camera.translate(0, +0.2, 0);
        if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_DOWN))
            m_camera.translate(0, -0.2, 0);
        if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_LEFT))
            m_camera.translate(-0.2, 0, 0);
        if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_RIGHT))
            m_camera.translate(+0.2, 0, 0);
        if (al_mouse_button_down(&m_prev_mouse_state, 3) && al_mouse_button_down(&m_mouse_state, 3))
            m_camera.translate(-dx * pan_scale, dy * pan_scale, 0);
    }
    else if (al_mouse_button_down(&m_prev_mouse_state, 3)  && al_mouse_button_down(&m_mouse_state, 3))
    {
        arcball_state_struct astate;
        astate.m_x1 = m_prev_mouse_state.x;
        astate.m_y1 = m_prev_mouse_state.y;
        astate.m_x2 = m_mouse_state.x;
        astate.m_y2 = m_mouse_state.y;

        vv_geom::quat q = get_arcball_quaternion(astate);
        m_camera.apply_rotation(q);
    }

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_MINUS))
        m_camera.translate(0, 0, -0.2);
    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_EQUALS))
        m_camera.translate(0, 0, +0.2);
}

void allegro_opengl_project::pre_render()
{
    allegro_project::pre_render();

    glPushMatrix(); // save 2d world matrix

    //glClearColor(0.0, 0.0, 0.2, 1);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_ALPHA_TEST);

    enable_global_lighting();

    m_camera.update();
}

void allegro_opengl_project::render()
{
    allegro_project::render();
    draw_box();
    draw_coord_system();
}

void allegro_opengl_project::draw_help_message()
{
    const auto text_color = al_map_rgb(0, 100, 100);

    al_draw_textf(m_system_font, text_color, 10, m_h - 45, ALLEGRO_ALIGN_LEFT,
                  "%s", "use arrow keys or middle mouse button to rotate model");
    al_draw_textf(m_system_font, text_color, 10, m_h - 35, ALLEGRO_ALIGN_LEFT,
                  "%s", "hold shift and middle mouse button to pan");
    al_draw_textf(m_system_font, text_color, 10, m_h - 25, ALLEGRO_ALIGN_LEFT,
                  "%s", "\"+/-\" or mouse wheel to zoom in/out");
    al_draw_textf(m_system_font, text_color, 10, m_h - 15, ALLEGRO_ALIGN_LEFT,
                  "%s", "\"r\" to reset");
}

void allegro_opengl_project::draw_debug_info()
{
    m_camera.debug_info(m_w - 15, m_h -40);
}

void allegro_opengl_project::draw_box()
{
    double len = 1.;

    GLfloat n[6][3] =      // normals for the 6 faces of a cube
    {
        {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
        {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0}
    };

    GLint faces[6][4] =    // vertex indices for the 6 faces of a cube
    {
        {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
        {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3}
    };
    GLfloat v[8][3];

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -len;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = len;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -len;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = len;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = len;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = -len;

    if(draw_state_flags::m_shaded)
    {
        GLfloat red_dif[]= {0.9,  0.0, 0.0, 1.0};
        GLfloat red_amb[]= {0.4,  0.0, 0.0, 1.0};
        GLfloat red_spe[]= {0.0,  0.0, 0.0, 1.0};

        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red_dif);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, red_amb);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, red_spe);
        for (int i = 0; i < 6; i++)
        {
            glBegin(GL_QUADS);
            glNormal3fv(&n[i][0]);
            glVertex3fv(&v[faces[i][0]][0]);
            glVertex3fv(&v[faces[i][1]][0]);
            glVertex3fv(&v[faces[i][2]][0]);
            glVertex3fv(&v[faces[i][3]][0]);
            glEnd();
        }
    }

    if(draw_state_flags::m_wireframe)
    {
        disable_global_lighting();
        glColor3f(0.0, 1.0, 1.0);
        glLineWidth(3);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0, 1.0);
        for (int i = 0; i < 6; i++)
        {
            glBegin(GL_LINE_LOOP);
            glVertex3fv(&v[faces[i][0]][0]);
            glVertex3fv(&v[faces[i][1]][0]);
            glVertex3fv(&v[faces[i][2]][0]);
            glVertex3fv(&v[faces[i][3]][0]);
            glEnd();
        }
    }
}

void allegro_opengl_project::post_render()
{
    disable_global_lighting();

    glDisable(GL_DEPTH_TEST);
    glPopMatrix(); // come back to 2d allegro world

    draw_compas();
    draw_help_message();
    draw_debug_info();
    allegro_project::post_render();
}

void allegro_opengl_project::imgui_render()
{
    allegro_project::imgui_render();
    if (!m_imgui_enabled)
        return;

    static ImVec4 clear_color = ImVec4(0.0f, 0.55f, 0.80f, 1.00f);
    if (g_show_demo_window)
        ImGui::ShowDemoWindow(&g_show_demo_window);

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoSavedSettings;
    window_flags |= ImGuiWindowFlags_NoMove;
    const char* window_name = u8"Вид";
    ImGui::Begin(window_name, nullptr, window_flags);
    ImGui::PushItemWidth(-FLT_MIN);
    ImGui::SetWindowPos(window_name, {0.,0.});

    ImGui::Checkbox("shaded", &draw_state_flags::m_shaded);
    ImGui::Checkbox("wireframe", &draw_state_flags::m_wireframe);
    ImGui::Checkbox("compas", &draw_state_flags::m_compas);
    ImGui::Checkbox("coord system", &draw_state_flags::m_coord_sys);

    auto quat = m_camera.get_quat() ? * m_camera.get_quat() : vv_geom::quat();
    float fx = (float)quat.x;
    float fy = (float)quat.y;
    float fz = (float)quat.z;
    float fw = (float)quat.w;
    ImGui::Text(u8"x"); ImGui::SameLine(); ImGui::SliderFloat("qx", &fx, -1.0f, +1.0f);
    ImGui::Text(u8"y"); ImGui::SameLine(); ImGui::SliderFloat("qy", &fy, -1.0f, +1.0f);
    ImGui::Text(u8"z"); ImGui::SameLine(); ImGui::SliderFloat("qz", &fz, -1.0f, +1.0f);
    ImGui::Text(u8"w"); ImGui::SameLine(); ImGui::SliderFloat("qw", &fw, -1.0f, +1.0f);
    vv_geom::quat new_quat(fw, fx, fy, fz);
    new_quat.normalize();
    //m_camera.apply_rotation(new_quat);

    ImGui::ColorEdit3("bkgnd color", (float *)&clear_color);

    ImGui::PopItemWidth();

    al_clear_to_color(al_map_rgb(clear_color.x * 255,
                                 clear_color.y * 255,
                                 clear_color.z * 255));
}

void allegro_opengl_project::enable_global_lighting()
{
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_position[] = {10.0, 10.0, 10.0, 1.0};
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_diffuse);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void allegro_opengl_project::disable_global_lighting()
{
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
}

void allegro_opengl_project::draw_compas()
{
    if (!draw_state_flags::m_compas)
        return;
    int compas_size = 60;
    int axis_length = 35;
    int text_offset = 12;

    glPushMatrix();

    //glDisable(GL_DEPTH_TEST);
    //glDisable(GL_CULL_FACE);
    //glDisable(GL_TEXTURE_2D);
    //glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glOrtho(0, m_w, m_h, 0, -compas_size, compas_size);
    glOrtho(0, m_w, 0, m_h, -compas_size, compas_size);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(m_w - compas_size, m_h - compas_size, 0);

    auto quat = m_camera.get_quat() ? * m_camera.get_quat() : vv_geom::quat();
    double rotation_matrix[16];
    quat.to_rotation_matrix(rotation_matrix);
    glMultMatrixd(rotation_matrix);

    glScaled(1, 1, 1);

    glLineWidth(3);

    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);   // x-axis
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(axis_length, 0, 0);
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);  // y-axis
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0, axis_length, 0);
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);  // z-axis
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0, 0, axis_length);
    glEnd();

    glDisable(GL_DEPTH_TEST);
    glPopMatrix();

    // Draw axes labels
    vv_geom::vec3 x_axis = vv_geom::rotate_vector(vv_geom::vec3(axis_length + text_offset, 0, 0), quat);
    vv_geom::vec3 y_axis = vv_geom::rotate_vector(vv_geom::vec3(0, axis_length + text_offset, 0), quat);
    vv_geom::vec3 z_axis = vv_geom::rotate_vector(vv_geom::vec3(0, 0, axis_length + text_offset), quat);

    al_draw_textf(m_system_font,
		  al_map_rgb(100, 0, 100),
		  m_w - compas_size + x_axis.x,  // x coord
		  compas_size - x_axis.y,       // y coord
		  ALLEGRO_ALIGN_LEFT,
		  "%s", "X");

    al_draw_textf(m_system_font,
		  al_map_rgb(100, 100, 0),
		  m_w - compas_size + y_axis.x,  // x coord
		  compas_size - y_axis.y,       // y coord
		  ALLEGRO_ALIGN_LEFT,
		  "%s", "Y");

    al_draw_textf(m_system_font,
		  al_map_rgb(0, 100, 100),
		  m_w - compas_size + z_axis.x,  // x coord
		  compas_size - z_axis.y,       // y coord
		  ALLEGRO_ALIGN_LEFT,
		  "%s", "Z");
}

void allegro_opengl_project::draw_coord_system()
{
    if (!draw_state_flags::m_coord_sys)
        return;

    int axis_length = 1;
    glPushMatrix();
    glScaled(0.3, 0.3, 0.3);
    glLineWidth(3);

    disable_global_lighting();
    glColor3f(1.0, 0.0, 0.0);
    glBegin(GL_LINES);   // x-axis
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(axis_length, 0, 0);
    glEnd();

    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_LINES);  // y-axis
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0, axis_length, 0);
    glEnd();

    glColor3f(0.0, 0.0, 1.0);
    glBegin(GL_LINES);  // z-axis
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0, 0, axis_length);
    glEnd();

    glPopMatrix();
}

vv_geom::quat allegro_opengl_project::get_arcball_quaternion(const arcball_state_struct &astate)
{
 if (astate.m_x1 > m_w || astate.m_x2 > m_w ||
            astate.m_y1 > m_h || astate.m_y2 > m_h)
     return vv_geom::quat();

    if (astate.m_x1 < 0. || astate.m_x2 < 0. ||
            astate.m_y1 < 0. || astate.m_y2 < 0.)
        return vv_geom::quat();

    if (vv_is_zero(astate.m_x1 - astate.m_x2) &&
            vv_is_zero(astate.m_y1 - astate.m_y2))
        return vv_geom::quat();

    double px1 = (astate.m_x1 - m_w/2);
    double py1 = (astate.m_y1 - m_h/2);

    double px2 = (astate.m_x2 - m_w/2);
    double py2 = (astate.m_y2 - m_h/2);

    double r_p1 = sqrt(px1*px1 + py1*py1);
    double r_p2 = sqrt(px2*px2 + py2*py2);

    if (vv_is_zero(r_p1))
        return vv_geom::quat();

    double arcball_radius = 2 * std::max(m_w/2, m_h/2);
    arcball_radius = std::max(arcball_radius, r_p1);
    arcball_radius = std::max(arcball_radius, r_p2);
    arcball_radius = std::max(r_p1, r_p2);

    double pz1 = sqrt(pow(arcball_radius, 2) - r_p1*r_p1);
    double pz2 = sqrt(pow(arcball_radius, 2) - r_p2*r_p2);

    vv_geom::vec3 v1 = {px1, py1, pz1};
    vv_geom::vec3 v2 = {px2, py2, pz2};

    v1.normalize();
    v2.normalize();

    vv_geom::quat q = vv_geom::quat::from_vectors(v2, v1);
    q.normalize();

    return q;
}

//  allegro_opengl_project::transformation implementation ///////////////////////////

void allegro_opengl_project::camera_frame::init_projection(double fov, double znear, double zfar, double aspect)
{
    m_fov = fov;
    m_znear = znear;
    m_zfar = zfar;
    m_aspect = aspect;
    m_init = true;
}

void allegro_opengl_project::camera_frame::reset()
{
    if (!m_init)
        return;
    m_xs = 1;
    m_ys = 1;
    m_zs = 1;
    if (nullptr == m_rotation)
        m_rotation = new vv_geom::quat();
    else
        *m_rotation = vv_geom::quat();
    m_x = 0;
    m_y = 0;
    m_z = 0;
    m_changed_scale = true;
    m_changed_rotation = true;
    m_changed_translation = true;
}

void allegro_opengl_project::camera_frame::reset_projection()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    double w,h;
    const double pi = std::acos(-1);
    h = 2 * m_znear * std::tan(m_fov*pi/(2*180));
    w = h * m_aspect;
    glFrustum(-w/2, w/2, -h/2, h/2, m_znear, m_zfar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void allegro_opengl_project::camera_frame::scale(double xs, double ys, double zs, bool absolute)
{
    m_xs = (absolute ? 0 : m_xs) + xs;
    m_ys = (absolute ? 0 : m_ys) + ys;

    m_zs = (absolute ? 0 : m_zs) + zs;
    m_changed_scale = true;
}

void allegro_opengl_project::camera_frame::translate(double x, double y, double z, bool absolute)
{
    m_x = (absolute ? 0 : m_x) + x;
    m_y = (absolute ? 0 : m_y) + y;
    m_z = (absolute ? 0 : m_z) + z;
    m_changed_translation = true;
}

void allegro_opengl_project::camera_frame::apply_rotation(const vv_geom::quat& q)
{
    if (nullptr == m_rotation)
        m_rotation = new vv_geom::quat();
    *m_rotation = *m_rotation * q;
    m_rotation->normalize();
    m_changed_rotation = true;
}

void allegro_opengl_project::camera_frame::debug_info(int x, int y)
{
    const auto font  = allegro_opengl_project::get_system_font();
    const auto color = al_map_rgb(0, 200, 0);

    double xa = 0, ya = 0, za = 0;
    if (nullptr != m_rotation)
        m_rotation->convert_to_euler(xa, ya, za);
    al_draw_textf(font, color, x, y, ALLEGRO_ALIGN_RIGHT, "%f", xa);
    al_draw_textf(font, color, x, y + 10, ALLEGRO_ALIGN_RIGHT, "%f", ya);
    al_draw_textf(font, color, x, y + 20, ALLEGRO_ALIGN_RIGHT, "%f", za);
}

void allegro_opengl_project::camera_frame::update()
{
    if (!m_init)
        throw "camera is not initialized";

    reset_projection();

    glTranslated(m_x, m_y, m_z);

    double rotation_matrix[16];
    if (nullptr != m_rotation)
    {
        m_rotation->to_rotation_matrix(rotation_matrix);
        glMultMatrixd(rotation_matrix);
    }

    glScaled(m_xs, m_ys, m_zs);
    m_changed_scale = false;
    m_changed_rotation = false;
    m_changed_translation = false;
}

double allegro_opengl_project::camera_frame::get_x()
{
    return m_x;
}

double allegro_opengl_project::camera_frame::get_y()
{
    return m_y;
}

double allegro_opengl_project::camera_frame::get_z()
{
    return m_z;
}

#endif
