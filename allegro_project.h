#include <iostream>
#include <algorithm>
#include <GL/glu.h>

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
  virtual void main_loop(); 
  const ALLEGRO_FONT* get_system_font();

protected:
  ALLEGRO_EVENT_QUEUE* _event_queue   = nullptr;
  ALLEGRO_DISPLAY*     _display       = nullptr;
  ALLEGRO_TIMER*       _fps           = nullptr;
  ALLEGRO_FONT*        _system_font   = nullptr;
  int                  _w             = 0;
  int                  _h             = 0;
};

class allegro_opengl_project : public allegro_project
{
public:
  virtual void pre_render() override;
  virtual void render() override;
  virtual void post_render() override;

  class transform
  {
    friend allegro_opengl_project;
  public:
    void reset();
    void scale(double dxs, double dxy, double dxz);
    void rotate(double dxa, double dya, double dza);
    void translate(double dx, double dy, double dz);
    void apply();

    double get_x();
    double get_y();
    double get_z();
  protected:
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
  };

protected:
  transform camera_transform;
  virtual void enable_global_lighting();
  virtual void disable_global_lighting();
};
