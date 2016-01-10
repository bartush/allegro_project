#include "allegro_project.h"

allegro_project::allegro_project() {}

allegro_project::~allegro_project() 
{
  if (_display)
    {
      al_destroy_display(_display);
      _display = nullptr;
    }
  if (_event_queue)
    {
      al_flush_event_queue(_event_queue);
      al_destroy_event_queue(_event_queue);
      _event_queue = nullptr;
    }
  if (_fps)
    {
      al_stop_timer(_fps);
      al_destroy_timer(_fps);
      _fps = nullptr;
    }
  if (_system_font)
    {
      al_destroy_font(_system_font);
      _system_font = nullptr;
    }
}

void allegro_project::init(int display_flags)
{
  // Basic initialization
  if (!al_init()) 
    throw "couldn't init allegro!";

  _event_queue = al_create_event_queue();
  if (!_event_queue) 
    throw "couldn't start event queue!";

  if (!init_fps_timer())
    throw "couldn't init timer!";
  al_start_timer(_fps);

  if(!al_install_keyboard()) 
    throw "couldn't install keyboard!";
  al_register_event_source(_event_queue, al_get_keyboard_event_source());
  
	  
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
  _system_font = al_create_builtin_font();
  if (!_system_font) 
    throw "system font is not initialized!";
}

void allegro_project::create_display(int w, int h)
{
  if (!_event_queue || _display)
    throw "event queue is not initialised or display is already created!";

  _display = al_create_display(w, h);
  if (!_display)
    throw "couldn't create display!";

  al_register_event_source(_event_queue, al_get_display_event_source(_display));
  _w = w;
  _h = h;
}

void allegro_project::pre_render() {}

void allegro_project::render()
{
  al_clear_to_color(al_map_rgb(0,0,255*0.2));
  al_draw_textf(_system_font, al_map_rgb(0, 255, 0), _w/2, _h/2,  
		ALLEGRO_ALIGN_CENTER, "%s", "TEST");
}

void allegro_project::post_render() {}

bool allegro_project::init_fps_timer(double speed_sec)
{
  if (!_event_queue) 
    return false;
  if (_fps)
    al_destroy_timer(_fps);
  _fps = al_create_timer(speed_sec);
  if (!_fps) return false;
  
  al_register_event_source(_event_queue, al_get_timer_event_source(_fps));
  return true;
}

void allegro_project::main_loop()
{
  if (!_event_queue) return;
  ALLEGRO_EVENT ev;
  bool drawing_enabled = false;
  while (true)
    {
      al_wait_for_event(_event_queue, &ev);
      switch (ev.type)
	{
	case ALLEGRO_EVENT_TIMER:
	  if (_display) drawing_enabled = true;
	  break;
	case ALLEGRO_EVENT_DISPLAY_CLOSE:
	  {
	    return;
	    break;
	  }
	case ALLEGRO_EVENT_KEY_DOWN:
	  {
	    if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
	      return;
	    break;
	  }
	case ALLEGRO_EVENT_DISPLAY_RESIZE:
	  {
	    al_acknowledge_resize(ev.display.source);
	    _w = ev.display.width;
	    _h = ev.display.height;
	    GLsizei min_size = (GLsizei) std::min(_w, _h);
	    glViewport ((_w - min_size)/2, (_h - min_size)/2, min_size, min_size);
	    break;
	  }
	default:
	  break;
	}
      if (drawing_enabled && al_event_queue_is_empty(_event_queue))
	{	
	  drawing_enabled = false;
	  pre_render();
	  render();
	  post_render();
	  al_flip_display();
	}
    }
}

const ALLEGRO_FONT* allegro_project::get_system_font()
{
  return _system_font;
}

// allegro_opengl_project implementation ////////////////////////////////

void allegro_opengl_project::pre_render() 
{

  glPushMatrix(); // save 2d world matrix
  enable_global_lighting();

  glClearColor(0.0, 0.0, 0.2, 1);
  glEnable(GL_DEPTH_TEST);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1, 0.01, 20); // init perspective view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 0, 10,
	    0, 0, 0, 
	    0, 1, 0 ); 

  camera_transform.scale(1.7, 1.7, 1.7);
  camera_transform.rotate(1, 1, 1);
  camera_transform.apply();
}

void allegro_opengl_project::render()
{
  glShadeModel(GL_SMOOTH);
  glEnable(GL_ALPHA_TEST);

  GLfloat n[6][3] = {    // normals for the 6 faces of a cube
    {-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
    {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };
  GLint faces[6][4] = {  // vertex indices for the 6 faces of a cube
    {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
    {4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
  GLfloat v[8][3];
  
  v[0][0] = v[1][0] = v[2][0] = v[3][0] = -1;
  v[4][0] = v[5][0] = v[6][0] = v[7][0] = 1;
  v[0][1] = v[1][1] = v[4][1] = v[5][1] = -1;
  v[2][1] = v[3][1] = v[6][1] = v[7][1] = 1;
  v[0][2] = v[3][2] = v[4][2] = v[7][2] = 1;
  v[1][2] = v[2][2] = v[5][2] = v[6][2] = -1; 
    
  GLfloat red_dif[]={0.9,  0.0, 0.0, 1.0};
  GLfloat red_amb[]={0.4,  0.0, 0.0, 1.0};
  GLfloat red_spe[]={0.0,  0.0, 0.0, 1.0};
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
  al_draw_textf(_system_font, al_map_rgb(0, 255, 0), _w/2, _h/2,  
		ALLEGRO_ALIGN_CENTER, "%s", "TEST");
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

//  allegro_opengl_project::transform implementation ///////////////////////////

void allegro_opengl_project::transform::reset()
{
  _xs = 1;
  _ys = 1;
  _zs = 1;
  _xa = 0;
  _ya = 0;
  _za = 0;
  _x = 0;
  _y = 0;
  _z = 0;
  _changed_scale = true;
  _changed_rotation = true;
  _changed_translation = true;
}
void allegro_opengl_project::transform::scale(double dxs, double dys, double dzs)
{
  _xs = dxs;
  _ys = dys;
  _zs = dzs;
  _changed_scale = true;
}
void allegro_opengl_project::transform::rotate(double dxa, double dya, double dza)
{
  _xa += dxa;
  _ya += dya;
  _za += dza;
  if (_xa >= 360) _xa -= 360;
  if (_ya >= 360) _ya -= 360;
  if (_za >= 360) _za -= 360;  
  _changed_rotation = true;
} 
void allegro_opengl_project::transform::translate(double dx, double dy, double dz)
{
  _x += dx;
  _y += dy;
  _z += dz;
  _changed_translation = true;
}
void allegro_opengl_project::transform::apply()
{
  if (_changed_scale) 
      glScaled(_xs, _ys, _zs);

  if (_changed_rotation)
    {
      glRotated(_xa, 1, 0, 0);
      glRotated(_ya, 0, 1, 0);
      glRotated(_za, 0, 0, 1);
    }

  if (_changed_translation)
      glTranslated(_x, _y, _z); 

  _changed_scale = false;
  _changed_rotation = false;
  _changed_translation = false;  
}

double allegro_opengl_project::transform::get_x() { return _x; }
double allegro_opengl_project::transform::get_y() { return _y; }
double allegro_opengl_project::transform::get_z() { return _z; }
