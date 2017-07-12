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
  BEGIN_EXCEPTION_CATCH()
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
  al_get_keyboard_state(&_keyboard_state);

  if(!al_install_mouse())
    throw "could't install mouse!";
  al_register_event_source(_event_queue, al_get_mouse_event_source());
  al_get_mouse_state(&_mouse_state);
  _prev_mouse_state = _mouse_state;
	  
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

  _init = true;
  END_EXCEPTION_CATCH();
}

void allegro_project::create_display(int w, int h)
{
  BEGIN_EXCEPTION_CATCH()
  if (!_init) throw "Allegro project is not initialized!";
  if (!_event_queue || _display)
    throw "event queue is not initialised or display is already created!";

  _display = al_create_display(w, h);
  if (!_display)
    throw "couldn't create display!";

  al_register_event_source(_event_queue, al_get_display_event_source(_display));
  display_resize(w, h);
  END_EXCEPTION_CATCH()
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
  _prev_mouse_state = _mouse_state;
  al_get_mouse_state(&_mouse_state);
  al_get_keyboard_state(&_keyboard_state);
}

void allegro_project::display_resize(int w, int h)
{
  _w = w;
  _h = h;
}

void allegro_project::main_loop()
{
  BEGIN_EXCEPTION_CATCH()
  if (!_init) throw "Allegro openGL project is not initialized!";
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
      if (drawing_enabled && al_event_queue_is_empty(_event_queue))
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
  return _system_font;
}

// allegro_opengl_project implementation ////////////////////////////////

void allegro_opengl_project::create_display(int w, int h)
{
  allegro_project::create_display(w, h);
  display_resize(w, h);
  _camera.translate(0, 0, -10);
  _camera.rotate(180, 0, 0);
}

void allegro_opengl_project::display_resize(int w, int h)
{
  allegro_project::display_resize(w, h);
  glViewport(0, 0, _w, _h);	    
  _camera.init_projection(45, 1, 100, static_cast<double>(_w) / _h);
}

void allegro_opengl_project::check_input_state()
{
  allegro_project::check_input_state();

  const double zoom_scale = 0.5;
  const double rot_scale = 0.2;
  const double pan_scale = 0.01;

  double dx = _prev_mouse_state.x - _mouse_state.x;
  double dy = _prev_mouse_state.y - _mouse_state.y;
  double dz = _prev_mouse_state.z - _mouse_state.z;

  _camera.translate(0, 0, -dz * zoom_scale);

  arcball_angles arcball = get_arcball_angles(_prev_mouse_state.x, _prev_mouse_state.y,
					      _mouse_state.x, _mouse_state.y);


  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_R))
    {
      _camera.reset();
      _camera.translate(0, 0, -10);
      _camera.rotate(180, 0, 0);
    }
  
  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_UP))
    _camera.rotate(-1, 0, 0);
  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_DOWN))
    _camera.rotate(+1, 0, 0);
  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_LEFT))
    _camera.rotate(0, +1, 0);

  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_RIGHT))
    _camera.rotate(0, -1, 0);

  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_RSHIFT) || 
      al_key_down(&_keyboard_state, ALLEGRO_KEY_LSHIFT))
    {
      if (al_key_down(&_keyboard_state, ALLEGRO_KEY_UP))
	_camera.translate(0, +0.2, 0);
      if (al_key_down(&_keyboard_state, ALLEGRO_KEY_DOWN))
	_camera.translate(0, -0.2, 0);
      if (al_key_down(&_keyboard_state, ALLEGRO_KEY_LEFT))
	_camera.translate(-0.2, 0, 0);
      if (al_key_down(&_keyboard_state, ALLEGRO_KEY_RIGHT))
	_camera.translate(+0.2, 0, 0);

      if ((_prev_mouse_state.buttons & 4) && (_mouse_state.buttons & 4))
	_camera.translate(-dx * pan_scale, dy * pan_scale, 0);
    } 
  else if ((_prev_mouse_state.buttons & 4) && (_mouse_state.buttons & 4))
    _camera.rotate(arcball.arc_x * rot_scale, arcball.arc_y * rot_scale, arcball.arc_z * rot_scale);
    //    _camera.rotate(-dy * rot_scale, dx * rot_scale, 0);


  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_MINUS))
    _camera.translate(0, 0, -0.2);
  if (al_key_down(&_keyboard_state, ALLEGRO_KEY_EQUALS))
    _camera.translate(0, 0, +0.2);
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

  _camera.update();
}

void allegro_opengl_project::render()
{
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

  draw_compas();

  ALLEGRO_COLOR text_color = al_map_rgb(0, 100, 100);

  al_draw_textf(_system_font, text_color, 10, _h - 45, ALLEGRO_ALIGN_LEFT,
		"%s", "use arrow keys or middle mouse button to rotate model");
  al_draw_textf(_system_font, text_color, 10, _h - 35, ALLEGRO_ALIGN_LEFT,
		"%s", "hold shift and middle mouse button to pan");
  al_draw_textf(_system_font, text_color, 10, _h - 25, ALLEGRO_ALIGN_LEFT,
		"%s", "\"+/-\" or mouse wheel to zoom in/out");
  al_draw_textf(_system_font, text_color, 10, _h - 15, ALLEGRO_ALIGN_LEFT,
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
  int compas_size = 50;
  int axis_length = 35; 
  
  glPushMatrix();

  //glDisable(GL_DEPTH_TEST);
  //glDisable(GL_CULL_FACE);
  //glDisable(GL_TEXTURE_2D);
  //glDisable(GL_LIGHTING);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  // gluOrtho2D(-100, 100, -100, 100);
  glOrtho(0, _w, _h, 0, -compas_size, compas_size);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslated(_w - compas_size, compas_size, 0);
  glRotated(_camera.get_xa(), 1, 0, 0);
  glRotated(-_camera.get_ya(), 0, 1, 0);
  glRotated(_camera.get_za(), 0, 0, 1);
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


  //draw axis names
  
  std::pair<float, float> x_label_pos, y_label_pos, z_label_pos;

  x_label_pos = std::make_pair(axis_length + 10, 0);
  y_label_pos = std::make_pair(0, - (axis_length + 10));
  z_label_pos = std::make_pair(0,0);

  ALLEGRO_TRANSFORM t;
  al_identity_transform(&t);
  al_rotate_transform_3d(&t, 0, 0, 1, _camera.get_za_radinas());
  al_rotate_transform_3d(&t, 0, 1, 0, _camera.get_ya_radians());
  al_rotate_transform_3d(&t, 1, 0, 0, _camera.get_xa_radians());
  
  al_transform_coordinates(&t, &x_label_pos.first, &x_label_pos.second);
  al_transform_coordinates(&t, &y_label_pos.first, &y_label_pos.second);
  

  al_draw_textf(_system_font,
		al_map_rgb(100, 0, 100),
		_w - compas_size + x_label_pos.first,  // x coord
		compas_size - x_label_pos.second, // y cord
		ALLEGRO_ALIGN_LEFT,
		"%s", "X");
  
    al_draw_textf(_system_font,
		al_map_rgb(0, 100, 100),
		_w - compas_size + y_label_pos.first,  // x coord
		compas_size - y_label_pos.second, // y cord
		ALLEGRO_ALIGN_LEFT,
		"%s", "Y");

}

allegro_opengl_project::arcball_angles allegro_opengl_project::get_arcball_angles(double screen_x1, 
										  double screen_y1, 
										  double screen_x2, 
										  double screen_y2)
{
  arcball_angles result;
  //double arcball_radius = std::max(_w/2, _h/2);

  double px1 =  (screen_x1 - _w/2);
  double py1 =  (screen_y1 - _h/2);

  double px2 =  (screen_x2 - _w/2);
  double py2 =  (screen_y2 - _h/2);

  double r_p1 = sqrt(px1*px1 + py1*py1);
  double r_p2 = sqrt(px2*px2 + py2*py2);


  double arcball_radius = std::max(r_p1, r_p2);
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

  result.arc_x = acos(std::min(1.0, norm_py1*norm_py2 + norm_pz1*norm_pz2));//*180/3.14;

  result.arc_y = acos(std::min(1.0,norm_px1*norm_px2 + norm_pz1*norm_pz2));//*180/3.14;

  result.arc_z = acos(std::min(1.0,norm_px1*norm_px2 + norm_py1*norm_py2));//*180/3.14;


  //if (std::isnan(result.arc_x)) result.arc_x = 0;
  //if (std::isnan(result.arc_y)) result.arc_y = 0;
  //if (std::isnan(result.arc_z)) result.arc_z = 0;

  std::cout << result.arc_x << ";" << result.arc_y << ";" << result.arc_z << std::endl;

  return result;
}


//  allegro_opengl_project::transformation implementation ///////////////////////////

void allegro_opengl_project::camera_frame::init_projection(double fov, double znear, double zfar, double aspect)
{
  _fov = fov;
  _znear = znear;
  _zfar = zfar;
  _aspect = aspect;
  _init = true;
}

void allegro_opengl_project::camera_frame::reset()
{
  if (!_init) return;
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

void allegro_opengl_project::camera_frame::reset_projection()
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  double w,h;
  const double pi = std::acos(-1);
  h = 2 * _znear * std::tan(_fov*pi/(2*180));
  w = h * _aspect;
  glFrustum(-w/2, w/2, -h/2, h/2, _znear, _zfar);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void allegro_opengl_project::camera_frame::scale(double xs, double ys, double zs, bool absolute)
{
  _xs = (absolute ? 0 : _xs) + xs;
  _ys = (absolute ? 0 : _ys) + ys;
  _zs = (absolute ? 0 : _zs) + zs;
  _changed_scale = true;
}
void allegro_opengl_project::camera_frame::rotate(double xa, double ya, double za, bool absolute)
{
  _xa = (absolute ? 0 : _xa) + xa;
  _ya = (absolute ? 0 : _ya) + ya;
  _za = (absolute ? 0 : _za) + za;
  if (_xa >= 360) _xa -= 360;
  if (_ya >= 360) _ya -= 360;
  if (_za >= 360) _za -= 360;  
  _changed_rotation = true;
} 
void allegro_opengl_project::camera_frame::translate(double x, double y, double z, bool absolute)
{
  _x = (absolute ? 0 : _x) + x;
  _y = (absolute ? 0 : _y) + y;
  _z = (absolute ? 0 : _z) + z;
  _changed_translation = true;
}
void allegro_opengl_project::camera_frame::apply()
{
  if (!_init) throw "camera is not initialized";

  reset_projection();

  if (_changed_translation)
    glTranslated(_x, _y, _z); 

  if (_changed_rotation)
    {
      glRotated(_xa, 1, 0, 0);
      glRotated(_ya, 0, 1, 0);
      glRotated(_za, 0, 0, 1);
    }

  if (_changed_scale) 
      glScaled(_xs, _ys, _zs);

  _changed_scale = false;
  _changed_rotation = false;
  _changed_translation = false;  
}
void allegro_opengl_project::camera_frame::update()
{
  if (!_init) throw "camera is not initialized";

  reset_projection();

  glTranslated(_x, _y, _z); 

  glRotated(_xa, 1, 0, 0);
  glRotated(_ya, 0, 1, 0);
  glRotated(_za, 0, 0, 1);

  glScaled(_xs, _ys, _zs);
  _changed_scale = false;
  _changed_rotation = false;
  _changed_translation = false;
}

double allegro_opengl_project::camera_frame::get_x() { return _x; }
double allegro_opengl_project::camera_frame::get_y() { return _y; }
double allegro_opengl_project::camera_frame::get_z() { return _z; }

double allegro_opengl_project::camera_frame::get_xa() { return _xa; }
double allegro_opengl_project::camera_frame::get_ya() { return _ya; }
double allegro_opengl_project::camera_frame::get_za() { return _za; }

float allegro_opengl_project::camera_frame::get_xa_radians()
{
  return _xa * std::acos(-1) / 180;
}
float allegro_opengl_project::camera_frame::get_ya_radians()
{
  return _ya * std::acos(-1) / 180;
}
float allegro_opengl_project::camera_frame::get_za_radinas()
{
  return _za * std::acos(-1) / 180;
}
