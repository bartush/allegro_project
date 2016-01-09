#include "allegro_project.h"

int main(int argc, char **argv)
{
  try
    {
      //allegro_project algl;
      allegro_opengl_project algl;
      algl.init(ALLEGRO_OPENGL | ALLEGRO_RESIZABLE);
      algl.create_display(500,500);
      algl.main_loop();
      throw "the end!";
    } 
  catch(const char* ex)
    {
      std::cout << "exception: " << ex << std::endl;
    }
  return 0;
}
