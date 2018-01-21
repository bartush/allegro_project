#include "allegro_project.h"
#include <vector>

int main(int argc, char **argv)
{
  allegro_opengl_project algl;
  algl.init(ALLEGRO_OPENGL | ALLEGRO_RESIZABLE);
  algl.create_display(800, 600);
  algl.main_loop();

  if (argc > 1)
    std::cout << std::endl << "argv[0]: " << argv[0]
	      << std::endl << "argv[1]: " << argv[1]
	      << std::endl;
  return 0;
}
