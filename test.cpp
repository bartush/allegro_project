#include "allegro_project.h"
#include <vector>
#include <map>

int main(int argc, char **argv)
{
    uint32_t version = al_get_allegro_version();
    int major = version >> 24;
    int minor = (version >> 16) & 255;
    int revision = (version >> 8) & 255;
    int release = version & 255;
    std::cout << std::endl << "allero version: " << major << "." << minor << "." << revision << "." << release << std::endl;

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
