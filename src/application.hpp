#pragma once
#include <string_view>

#include <filamentapp/Config.h>
#include <filamentapp/FilamentApp.h>
#include <filamentapp/IBL.h>

#include <filament/Engine.h>
#include <filament/Scene.h>
#include <filament/Skybox.h>
#include <filament/TransformManager.h>
#include <filament/View.h>

#include <gltfio/AssetLoader.h>
#include <gltfio/FilamentAsset.h>
#include <gltfio/FilamentInstance.h>
#include <gltfio/ResourceLoader.h>
#include <gltfio/TextureProvider.h>

#include <viewer/AutomationEngine.h>
#include <viewer/AutomationSpec.h>
#include <viewer/ViewerGui.h>

#include <camutils/Manipulator.h>
#include <utils/NameComponentManager.h>
//#include <utils/NameComponentManager.h>

class Application
{
public:
    void run(Config &config)
    {
        using namespace std::placeholders;

        auto &app = FilamentApp::get();

        app.resize(std::bind(&Application::resize, this, _1, _2));
        app.animate(std::bind(&Application::animate, this, _1, _2, _3));
        app.setDropHandler(std::bind(&Application::drop_handler, this, _1));

        app.run(config,
                std::bind(&Application::setup, this, _1, _2, _3),
                std::bind(&Application::cleanup, this, _1, _2, _3),
                std::bind(&Application::gui, this, _1, _2),
                std::bind(&Application::pre_render, this, _1, _2, _3, _4),
                std::bind(&Application::post_render, this, _1, _2, _3, _4));
    }

protected:
    Application(/* args */) {}
    virtual ~Application() {}

    //
    // virtual functions
    //
    virtual void setup(filament::Engine *engine, filament::View *view, filament::Scene *scene) {}

    virtual void cleanup(filament::Engine *engine, filament::View *, filament::Scene *) {}

    virtual void animate(filament::Engine *engine, filament::View *view, double now) {}

    virtual void resize(filament::Engine *engine, filament::View *view) {}

    virtual void gui(filament::Engine *engine, filament::View *view) {}

    virtual void pre_render(filament::Engine *engine, filament::View *view, filament::Scene *scene, filament::Renderer *renderer) {}

    virtual void post_render(filament::Engine *engine, filament::View *view, filament::Scene *scene, filament::Renderer *renderer) {}

    virtual void drop_handler(std::string_view path) {}
};
