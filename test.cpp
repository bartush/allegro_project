#include "allegro_project.h"
#include "imgui.h"
#include "imgui_impl_allegro5.h"
#include <vector>
#include <map>

int main(int argc, char **argv)
{
    allegro_project::allegro_check_version();
    allegro_opengl_project algl;
    algl.init(ALLEGRO_OPENGL | ALLEGRO_RESIZABLE);
    algl.create_display(800, 600);
    algl.main_loop();
    return 0;
}

