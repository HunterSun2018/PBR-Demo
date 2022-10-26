#include <iostream>
#include <functional>
#include <tuple>
#include <optional>
//#include <getopt/getopt.h>
#include <unistd.h>

#include <filament/Engine.h>
#include <filament/Material.h>
#include <filamentapp/Config.h>

#include "demo.hpp"

using namespace std;
using namespace filament;

tuple<string, string> get_options(int argc, char *argv[]);

const string IBL_FOLDER = "../assets/ibl/lightroom_14b";
const string gltf_path = "../assets/models";

int main(int argc, char **argv)
{

    try
    {
        get_options(argc, argv);
        
        Config config;

        config.title = "PBR demo";
        config.backend = Engine::Backend::OPENGL;
        config.iblDirectory = FilamentApp::getRootAssetsPath() + IBL_FOLDER;

        auto demo = Demo::create();

        demo->set_gltf((FilamentApp::getRootAssetsPath() + gltf_path).c_str());
        demo->run(config);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}

tuple<string, string> get_options(int argc, char *argv[])
{
    int opt;
    string model, ibl_path;

    while ((opt = getopt(argc, argv, "du:m:n:w:c:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            ibl_path = optarg;
            break;
        case 'm':
            model = optarg;
            break;
        // case 'n':
        //     mnemonic = optarg;
        //     break;
        // case 'w':
        //     waypoint = optarg;
        //     break;
        // case 'c':
        //     chain_id = std::stoi(optarg);
        //     break;
        // case 'd':
        //     distrbuting = true;
        //     break;
        case 'h':
            throw runtime_error("demo -i ibl_path -m model_path");
            break;
        case '?':
        default:
            break;
        }
    }

    return {model, ibl_path};
}
