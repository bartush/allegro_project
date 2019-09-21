#include <iostream>
#include <algorithm>
#include <cmath>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>


class allegro_project
{
public:
    allegro_project();
    virtual ~allegro_project();
    virtual void init(int display_flags);
    virtual void create_display(int w, int h);
    virtual void display_resize(int w, int h);
    virtual void pre_render();
    virtual void render();
    virtual void post_render();
    virtual bool init_fps_timer(double speed_sec = 1.0/24.0);
    virtual void keyboard_event_handler(const ALLEGRO_EVENT& ev);
    virtual void check_input_state();
    virtual void main_loop();
    const ALLEGRO_FONT* get_system_font();

protected:
    ALLEGRO_KEYBOARD_STATE m_keyboard_state;
    ALLEGRO_MOUSE_STATE    m_mouse_state;
    ALLEGRO_MOUSE_STATE    m_prev_mouse_state;
    bool                   m_init          = false;
    ALLEGRO_EVENT_QUEUE*   m_event_queue   = nullptr;
    ALLEGRO_DISPLAY*       m_display       = nullptr;
    ALLEGRO_TIMER*         m_fps           = nullptr;
    ALLEGRO_FONT*          m_system_font   = nullptr;
    int                    m_w             = 0;
    int                    m_h             = 0;
};

class allegro_opengl_project : public allegro_project
{
public:
    virtual void create_display(int w, int h);
    virtual void display_resize(int w, int h);
    virtual void pre_render();
    virtual void render();
    virtual void post_render();

    virtual void check_input_state() override;
    virtual void draw_compas();

    struct arcball_angles
    {
        double arc_x = .0;
        double arc_y = .0;
        double arc_z = .0;
    };
    arcball_angles get_arcball_angles(double screen_x1, double screen_y1, double screen_x2, double screen_y2);

    class camera_frame
    {
    public:
        void init_projection(double fov = 45, double znear = 1, double zfar = 10, double aspect = 1);
        void reset();
        void reset_projection();
        void scale(double dxs, double dxy, double dxz, bool absolute = false);
        void rotate(double dxa, double dya, double dza, bool absolute = false);
        void translate(double dx, double dy, double dz, bool absolute = false);
        //void apply();
        void update();

        double get_x();
        double get_y();
        double get_z();

        double get_xa();
        double get_ya();
        double get_za();

        float get_xa_radians();
        float get_ya_radians();
        float get_za_radians();


    protected:
        bool m_init = false;

        double m_x  = 0;
        double m_y  = 0;
        double m_z  = 0;
        double m_xa = 0;
        double m_ya = 0;
        double m_za = 0;
        double m_xs = 1;
        double m_ys = 1;
        double m_zs = 1;
        bool m_changed_translation = false;
        bool m_changed_rotation    = false;
        bool m_changed_scale       = false;

        double m_fov    = 45;
        double m_znear  = 0;
        double m_zfar   = 10;
        double m_aspect = 1;
    };

protected:
    camera_frame m_camera;

    virtual void enable_global_lighting();
    virtual void disable_global_lighting();
};

// utility macros

#define BEGIN_EXCEPTION_CATCH()  try {

#define END_EXCEPTION_CATCH() } catch(const char* ex) \
				  { std::cout << "exception: " << ex << std::endl; }
