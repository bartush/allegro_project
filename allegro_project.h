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

  void pre_render() override;
  void render() override;
  void post_render() override;
};
