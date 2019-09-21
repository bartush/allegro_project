#include "allegro_project.h"


allegro_project::allegro_project() {}

allegro_project::~allegro_project()
{
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

void allegro_project::init(int display_flags)
{
    BEGIN_EXCEPTION_CATCH()
    // Basic initialization
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

    // Enable antialiasing
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 2, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);

    // Init Addons
    if (!al_init_primitives_addon())
        throw "couldn't init primitives addon!";

    if (!al_init_image_addon())
        throw "couldn't init image addon!";

    al_init_font_addon();
    m_system_font = al_create_builtin_font();
    if (!m_system_font)
        throw "system font is not initialized!";

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
    END_EXCEPTION_CATCH()
}

void allegro_project::pre_render() {}

void allegro_project::render()
{
    al_clear_to_color(al_map_rgb(0,0,255*0.2));
    al_draw_textf(m_system_font, al_map_rgb(0, 255, 0), m_w/2, m_h/2,
                  ALLEGRO_ALIGN_CENTER, "%s", "TEST");
}

void allegro_project::post_render() {}

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

// allegro_opengl_project implementation ////////////////////////////////

void allegro_opengl_project::create_display(int w, int h)
{
    allegro_project::create_display(w, h);
    display_resize(w, h);
    m_camera.translate(0, 0, -10);
    m_camera.rotate(180, 0, 0);
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

    const double zoom_scale = 0.5;
    const double rot_scale = 0.2;
    const double pan_scale = 0.01;

    double dx = m_prev_mouse_state.x - m_mouse_state.x;
    double dy = m_prev_mouse_state.y - m_mouse_state.y;
    double dz = m_prev_mouse_state.z - m_mouse_state.z;

    m_camera.translate(0, 0, -dz * zoom_scale);

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_R))
    {
        m_camera.reset();
        m_camera.translate(0, 0, -10);
        m_camera.rotate(180, 0, 0);
    }

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_UP))
        m_camera.rotate(-1, 0, 0);
    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_DOWN))
        m_camera.rotate(+1, 0, 0);
    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_LEFT))
        m_camera.rotate(0, +1, 0);

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_RIGHT))
        m_camera.rotate(0, -1, 0);

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
        arcball_angles arcball = get_arcball_angles(m_prev_mouse_state.x, m_prev_mouse_state.y,
                                 m_mouse_state.x, m_mouse_state.y);

        bool zero_arcball_angles =
            arcball.arc_x == .0 &&
            arcball.arc_y == .0 &&
            arcball.arc_z == .0;

        if (!zero_arcball_angles)
            m_camera.rotate(arcball.arc_x * rot_scale, arcball.arc_y * rot_scale, arcball.arc_z * rot_scale);

        //_camera.rotate(-dy * rot_scale, dx * rot_scale, 0);
    }

    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_MINUS))
        m_camera.translate(0, 0, -0.2);
    if (al_key_down(&m_keyboard_state, ALLEGRO_KEY_EQUALS))
        m_camera.translate(0, 0, +0.2);
}

void allegro_opengl_project::pre_render()
{
    glPushMatrix(); // save 2d world matrix

    glClearColor(0.0, 0.0, 0.2, 1);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_ALPHA_TEST);

    enable_global_lighting();

    m_camera.update();
}

void allegro_opengl_project::render()
{
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

    v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
    v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
    v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
    v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
    v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
    v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1;

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

void allegro_opengl_project::post_render()
{
    disable_global_lighting();

    glDisable(GL_DEPTH_TEST);
    glPopMatrix(); // come back to 2d allegro world

    draw_compas();

    ALLEGRO_COLOR text_color = al_map_rgb(0, 100, 100);

    al_draw_textf(m_system_font, text_color, 10, m_h - 45, ALLEGRO_ALIGN_LEFT,
                  "%s", "use arrow keys or middle mouse button to rotate model");
    al_draw_textf(m_system_font, text_color, 10, m_h - 35, ALLEGRO_ALIGN_LEFT,
                  "%s", "hold shift and middle mouse button to pan");
    al_draw_textf(m_system_font, text_color, 10, m_h - 25, ALLEGRO_ALIGN_LEFT,
                  "%s", "\"+/-\" or mouse wheel to zoom in/out");
    al_draw_textf(m_system_font, text_color, 10, m_h - 15, ALLEGRO_ALIGN_LEFT,
                  "%s", "\"r\" to reset");
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
    glOrtho(0, m_w, m_h, 0, -compas_size, compas_size);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(m_w - compas_size, compas_size, 0);
    glRotated(m_camera.get_xa(), 1, 0, 0);
    glRotated(-m_camera.get_ya(), 0, 1, 0);
    glRotated(m_camera.get_za(), 0, 0, 1);
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


    //draw axes labels

    ALLEGRO_TRANSFORM TX, TY, TZ, TTX, TTY, TTZ, TRX, TRY, TRZ, TS;
    al_identity_transform(&TX);
    al_identity_transform(&TY);
    al_identity_transform(&TZ);
    al_identity_transform(&TTX);
    al_identity_transform(&TTY);
    al_identity_transform(&TTZ);
    al_identity_transform(&TRX);
    al_identity_transform(&TRY);
    al_identity_transform(&TRZ);
    al_identity_transform(&TS);

    al_rotate_transform_3d(&TRX, 1, 0, 0, m_camera.get_xa_radians());
    al_rotate_transform_3d(&TRY, 0, 1, 0, m_camera.get_ya_radians());
    al_rotate_transform_3d(&TRZ, 0, 0, 1, m_camera.get_za_radians());
    al_translate_transform_3d(&TTX, 1, 0, 0);
    al_translate_transform_3d(&TTY, 0, -1, 0);
    al_translate_transform_3d(&TTZ, 0, 0, -1);
    al_scale_transform_3d(&TS,
                          axis_length + text_offset,
                          axis_length + text_offset,
                          axis_length + text_offset);

    al_compose_transform(&TX, &TTX);
    al_compose_transform(&TX, &TS);
    al_compose_transform(&TX, &TRZ);
    al_compose_transform(&TX, &TRY);
    al_compose_transform(&TX, &TRX);

    al_compose_transform(&TY, &TTY);
    al_compose_transform(&TY, &TS);
    al_compose_transform(&TY, &TRZ);
    al_compose_transform(&TY, &TRY);
    al_compose_transform(&TY, &TRX);

    al_compose_transform(&TZ, &TTZ);
    al_compose_transform(&TZ, &TS);
    al_compose_transform(&TZ, &TRZ);
    al_compose_transform(&TZ, &TRY);
    al_compose_transform(&TZ, &TRX);

    ALLEGRO_VERTEX x_label, y_label, z_label;

    x_label.x = 0;
    x_label.y = 0;
    x_label.z = 0;

    y_label.x = 0;
    y_label.y = 0;
    y_label.z = 0;

    z_label.x = 0;
    z_label.y = 0;
    z_label.z = 0;

    al_transform_coordinates_3d(&TX, &x_label.x, &x_label.y, &x_label.z);
    al_transform_coordinates_3d(&TY, &y_label.x, &y_label.y, &y_label.z);
    al_transform_coordinates_3d(&TZ, &z_label.x, &z_label.y, &z_label.z);


    al_draw_textf(m_system_font,
                  al_map_rgb(100, 0, 100),
                  m_w - compas_size + x_label.x,  // x coord
                  compas_size - x_label.y, // y cord
                  ALLEGRO_ALIGN_LEFT,
                  "%s", "X");

    al_draw_textf(m_system_font,
                  al_map_rgb(100, 100, 0),
                  m_w - compas_size + y_label.x,  // x coord
                  compas_size - y_label.y, // y cord
                  ALLEGRO_ALIGN_LEFT,
                  "%s", "Y");

    al_draw_textf(m_system_font,
                  al_map_rgb(0, 100, 100),
                  m_w - compas_size + z_label.x,  // x coord
                  compas_size - z_label.y, // y cord
                  ALLEGRO_ALIGN_LEFT,
                  "%s", "Z");

    al_draw_textf(m_system_font,
                  al_map_rgb(0, 100, 0),
                  10,  // x coord
                  20, // y cord
                  ALLEGRO_ALIGN_LEFT,
                  "%f", m_camera.get_xa_radians());

    al_draw_textf(m_system_font,
                  al_map_rgb(0, 100, 0),
                  10,  // x coord
                  30, // y cord
                  ALLEGRO_ALIGN_LEFT,
                  "%f", m_camera.get_ya_radians());

    al_draw_textf(m_system_font,
                  al_map_rgb(0, 100, 0),
                  10,  // x coord
                  40, // y cord
                  ALLEGRO_ALIGN_LEFT,
                  "%f", m_camera.get_za_radians());



}

allegro_opengl_project::arcball_angles allegro_opengl_project::get_arcball_angles(double screen_x1,
        double screen_y1,
        double screen_x2,
        double screen_y2)
{
    arcball_angles result;
    if (screen_x1 == screen_x2 && screen_y1 == screen_y2)
        return result;

    //double arcball_radius = std::max(_w/2, _h/2);

    double px1 =  (screen_x1 - m_w/2);
    double py1 =  (screen_y1 - m_h/2);

    double px2 =  (screen_x2 - m_w/2);
    double py2 =  (screen_y2 - m_h/2);

    double r_p1 = sqrt(px1*px1 + py1*py1);
    double r_p2 = sqrt(px2*px2 + py2*py2);


    //double arcball_radius = std::max(r_p1, r_p2);
    double arcball_radius = r_p1;

    //if (r_p1 > arcball_radius) arcball_radius = r_p1;
    //if (r_p2 > arcball_radius) arcball_radius = r_p2;

    double pz1 = sqrt(pow(arcball_radius, 2) - r_p1*r_p1);
    double pz2 = sqrt(pow(arcball_radius, 2) - r_p2*r_p2);

    double norm_px1 = px1/arcball_radius;
    double norm_py1 = py1/arcball_radius;
    double norm_pz1 = pz1/arcball_radius;

    double norm_px2 = px2/arcball_radius;
    double norm_py2 = py2/arcball_radius;
    double norm_pz2 = pz2/arcball_radius;


    result.arc_x = norm_px2 - norm_px1;
    result.arc_y = norm_py2 - norm_py1;
    result.arc_z = norm_pz2 - norm_pz1;

    const double sign_x = result.arc_x >= 0 ? 1. : -1;
    const double sign_y = result.arc_y >= 0 ? 1. : -1;
    const double sign_z = result.arc_z >= 0 ? 1. : -1;

    const double rot_scale = 3.;

    result.arc_x = rot_scale * sign_x * acos(std::min(1.0,norm_py1*norm_py2 + norm_pz1*norm_pz2));
    result.arc_y = rot_scale * sign_y * acos(std::min(1.0,norm_px1*norm_px2 + norm_pz1*norm_pz2));
    result.arc_z = rot_scale * sign_z * acos(std::min(1.0,norm_px1*norm_px2 + norm_py1*norm_py2));


    //if (std::isnan(result.arc_x)) result.arc_x = 0;
    //if (std::isnan(result.arc_y)) result.arc_y = 0;
    //if (std::isnan(result.arc_z)) result.arc_z = 0;

//    std::cout << "radius - " << arcball_radius << ";" <<
//              result.arc_x << ";" << result.arc_y << ";" << result.arc_z << std::endl;

    return result;
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
    m_xa = 0;
    m_ya = 0;
    m_za = 0;
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
void allegro_opengl_project::camera_frame::rotate(double xa, double ya, double za, bool absolute)
{
    m_xa = (absolute ? 0 : m_xa) + xa;
    m_ya = (absolute ? 0 : m_ya) + ya;
    m_za = (absolute ? 0 : m_za) + za;
    if (m_xa >= 360)
        m_xa -= 360;
    if (m_ya >= 360)
        m_ya -= 360;
    if (m_za >= 360)
        m_za -= 360;
    m_changed_rotation = true;
}
void allegro_opengl_project::camera_frame::translate(double x, double y, double z, bool absolute)
{

    m_x = (absolute ? 0 : m_x) + x;
    m_y = (absolute ? 0 : m_y) + y;
    m_z = (absolute ? 0 : m_z) + z;
    m_changed_translation = true;
}

// void allegro_opengl_project::camera_frame::apply()
// {
//   if (!_init) throw "camera is not initialized";
//   reset_projection();
//   if (_changed_translation)
//     glTranslated(_x, _y, _z);
//   if (_changed_rotation)
//     {
//       glRotated(_xa, 1, 0, 0);
//       glRotated(_ya, 0, 1, 0);
//       glRotated(_za, 0, 0, 1);
//     }
//   if (_changed_scale)
//       glScaled(_xs, _ys, _zs);
//   _changed_scale = false;
//   _changed_rotation = false;
//   _changed_translation = false;
// }

void allegro_opengl_project::camera_frame::update()
{
    if (!m_init)
        throw "camera is not initialized";

    reset_projection();

    glTranslated(m_x, m_y, m_z);

    glRotated(m_xa, 1, 0, 0);
    glRotated(m_ya, 0, 1, 0);
    glRotated(m_za, 0, 0, 1);

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

double allegro_opengl_project::camera_frame::get_xa()
{
    return m_xa;
}
double allegro_opengl_project::camera_frame::get_ya()
{
    return m_ya;
}
double allegro_opengl_project::camera_frame::get_za()
{
    return m_za;
}

float allegro_opengl_project::camera_frame::get_xa_radians()
{
    return m_xa * std::acos(-1) / 180;
}
float allegro_opengl_project::camera_frame::get_ya_radians()
{
    return m_ya * std::acos(-1) / 180;
}
float allegro_opengl_project::camera_frame::get_za_radians()
{
    return m_za * std::acos(-1) / 180;
}
