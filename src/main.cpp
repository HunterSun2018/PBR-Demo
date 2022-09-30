#include <iostream>
#include <functional>
#include <filament/FilamentAPI.h>
#include <filament/Engine.h>
#include <filament/Material.h>
#include <filamentapp/Config.h>
#include <filamentapp/FilamentApp.h>
#include <utils/EntityManager.h>

using namespace std;
using namespace filament;

class Demo
{
    Material* _material;

public:    
    void setup(Engine *engine, View *view, Scene *scene)
    {
        auto& tcm = engine->getTransformManager();
        auto& rcm = engine->getRenderableManager();
        auto& em = utils::EntityManager::get();

        _material = Material::Builder().package(RESOURCES_AIDEFAULTMAT_DATA, RESOURCES_AIDEFAULTMAT_SIZE).build(*engine);

    }

    void cleanup(Engine *engine, View *, Scene *)
    {
    }

    void animate(Engine *engine, View *view, double now)
    {
    }

    // auto bind_setup()
    // {
    //     return bind(&Demo::setup, this, placeholders::_1, placeholders::_2, placeholders::_3);
    // }
};

int main(int argc, char **argv)
{
    Config config;

    config.title = "PBR demo";
    config.backend = Engine::Backend::OPENGL;
    // config.iblDirectory = FilamentApp::getRootAssetsPath() + IBL_FOLDER;

    auto &app = FilamentApp::get();
    {
        using namespace std::placeholders;
        Demo demo;

        app.animate(bind(&Demo::animate, &demo, _1, _2, _3));

        app.run(config,
                bind(&Demo::setup, &demo, _1, _2, _3),
                bind(&Demo::cleanup, &demo, _1, _2, _3));
    }

    return 0;
}
