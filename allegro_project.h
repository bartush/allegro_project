#include <iostream>
#include <algorithm>
#include <cmath>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>


class allegro_project
{
public:
    allegro_project();
    virtual ~allegro_project();
    virtual void init(int display_flags, bool enable_imgui = true);
    virtual void create_display(int w, int h);
    virtual void display_resize(int w, int h);
    virtual void pre_render();
    virtual void render();
    virtual void post_render();
    virtual void imgui_render();
    virtual bool init_fps_timer(double speed_sec = 1.0/24.0);
    virtual void keyboard_event_handler(const ALLEGRO_EVENT& ev);
    virtual void check_input_state();
    virtual void main_loop();
    static const ALLEGRO_FONT* get_system_font();
    static void allegro_check_version();

protected:
    static ALLEGRO_FONT*   m_system_font;
    ALLEGRO_KEYBOARD_STATE m_keyboard_state;
    ALLEGRO_KEYBOARD_STATE m_prev_keyboard_state;
    ALLEGRO_MOUSE_STATE    m_mouse_state;
    ALLEGRO_MOUSE_STATE    m_prev_mouse_state;
    bool                   m_init          = false;
    bool                   m_imgui_enabled = false;
    ALLEGRO_EVENT_QUEUE*   m_event_queue   = nullptr;
    ALLEGRO_DISPLAY*       m_display       = nullptr;
    ALLEGRO_TIMER*         m_fps           = nullptr;
    int                    m_w             = 0;
    int                    m_h             = 0;
};

#ifdef ALLEGRO_PROJECT_OPENGL
class allegro_opengl_project : public allegro_project
{
public:
    virtual void create_display(int w, int h);
    virtual void display_resize(int w, int h);
    virtual void pre_render();
    virtual void render();
    virtual void post_render();
    virtual void imgui_render() override;
    virtual void check_input_state() override;
    virtual void draw_compas();
    virtual void draw_help_message();
    virtual void draw_debug_info();

    struct arcball_state_struct
    {
        double m_x1 = .0;
        double m_y1 = .0;
        double m_x2 = .0;
        double m_y2 = .0;
    };
    struct arcball_angles_struct
    {
        double m_ax = .0;
        double m_ay = .0;
        double m_az = .0;
    };
    arcball_angles_struct get_arcball_angles(const arcball_state_struct &astate);

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

        void debug_info(int x, int y);

        double get_x();
        double get_y();
        double get_z();

        double get_xa()const {return m_xa;}
        double get_ya()const {return m_ya;}
        double get_za()const {return m_za;}

        float get_xa_radians();
        float get_ya_radians();
        float get_za_radians();

        void set_xa(double xa) {m_xa = xa;}
        void set_ya(double ya) {m_ya = ya;}
        void set_za(double za) {m_za = za;}

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
#endif

// utility macros

#define BEGIN_EXCEPTION_CATCH()  try {

#define END_EXCEPTION_CATCH() } catch(const char* ex) \
				  { std::cout << "exception: " << ex << std::endl; }
