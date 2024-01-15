#include <iostream>
#include <functional>
#include <tuple>
#include <optional>
#include <format>
#include <unistd.h>

#include <filament/Engine.h>
#include <filament/Material.h>
#include <filamentapp/Config.h>

#include "demo.hpp"

using namespace std;
using namespace filament;

tuple<string, string, backend::Backend> get_options(int argc, char *argv[]);

const string IBL_FOLDER = "../assets/ibl/lightroom_14b";
const string gltf_path = "../assets/models";

int main(int argc, char **argv)
{

    try
    {
        auto [model, ibl_path, backend] = get_options(argc, argv);

        Config config;

        config.title = "PBR demo";
        config.backend = backend;
        config.iblDirectory = ibl_path.empty() ? FilamentApp::getRootAssetsPath() + IBL_FOLDER : ibl_path.c_str();

        auto demo = Demo::create();
        auto gltf = model.empty() ? (FilamentApp::getRootAssetsPath() + gltf_path).c_str() : model;

        demo->set_gltf(gltf);
        demo->run(config);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

tuple<string, string, backend::Backend> get_options(int argc, char *argv[])
{
    int opt;
    string model, ibl_path;
    backend::Backend backend = Engine::Backend::OPENGL;
    string usage = "usage : \n\tdemo -i ibl_path -m model_path -b [opengl|vulkan]";

    while ((opt = getopt(argc, argv, "i:m:b:h")) != -1)
    {
        switch (opt)
        {
        case 'i':
            ibl_path = optarg;
            break;
        case 'm':
            model = optarg;
            break;
        case 'b':
            backend = string("vulkan") == optarg ? Engine::Backend::VULKAN : Engine::Backend::OPENGL;
            break;
        case 'h':
        case '?':
            cout << usage << endl;                    
            exit(0);
        default:
            break;
        }
    }

    return {model, ibl_path, backend};在
}
