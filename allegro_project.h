#include <iostream>
#include <cmath>
#include <algorithm>

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
  virtual void pre_render();
  virtual void render();
  virtual void post_render();
  virtual bool init_fps_timer(double speed_sec = 1.0/24.0);
  virtual void keyboard_event_handler(const ALLEGRO_EVENT& ev);
  virtual void check_keyboard_state();
  virtual void check_mouse_state();
  virtual void main_loop(); 
  const ALLEGRO_FONT* get_system_font();

protected:
  ALLEGRO_KEYBOARD_STATE _keyboard_state;
  ALLEGRO_MOUSE_STATE    _mouse_state;
  ALLEGRO_MOUSE_STATE    _prev_mouse_state;
  bool                   _init          = false;
  ALLEGRO_EVENT_QUEUE*   _event_queue   = nullptr;
  ALLEGRO_DISPLAY*       _display       = nullptr;
  ALLEGRO_TIMER*         _fps           = nullptr;
  ALLEGRO_FONT*          _system_font   = nullptr;
  int                    _w             = 0;
  int                    _h             = 0;
};

class allegro_opengl_project : public allegro_project
{
public:
  virtual void init(int display_flags);
  virtual void pre_render() override;
  virtual void render() override;
  virtual void post_render() override;
  virtual void check_keyboard_state();
  virtual void check_mouse_state();

  class camera_frame
  {
  public:
    void init_projection(double fov = 45, double znear = 1, double zfar = 10, double aspect = 1);
    void reset();
    void reset_projection();
    void scale(double dxs, double dxy, double dxz, bool absolute = false);
    void rotate(double dxa, double dya, double dza, bool absolute = false);
    void translate(double dx, double dy, double dz, bool absolute = false);
    void apply();
    void update();


    double get_x();
    double get_y();
    double get_z();

  protected:
    bool _init = false;

    double _x  = 0;
    double _y  = 0;
    double _z  = 0;  
    double _xa = 0;
    double _ya = 0;
    double _za = 0;
    double _xs = 1;
    double _ys = 1;
    double _zs = 1;
    bool _changed_translation = false;
    bool _changed_rotation    = false;
    bool _changed_scale       = false;
    
    double _fov    = 45;
    double _znear  = 0;
    double _zfar   = 10;
    double _aspect = 1;
  };

protected:
  camera_frame _camera;
  
  virtual void enable_global_lighting();
  virtual void disable_global_lighting();
};

// utility macros
 
#define BEGIN_EXCEPTION_CATCH()  try {

#define END_EXCEPTION_CATCH() } catch(const char* ex) \
				  { std::cout << "exception: " << ex << std::endl; }
