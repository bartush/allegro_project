#include "allegro_project.h"

int main(int argc, char **argv)
{
  allegro_opengl_project algl;
  algl.init(ALLEGRO_OPENGL | ALLEGRO_RESIZABLE);
  algl.create_display(800, 600);
  algl.main_loop();
  return 0;
}
