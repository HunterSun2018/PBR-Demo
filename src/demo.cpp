#include <iostream>
#include <fstream>
#include <iterator>
#include <filesystem>
#include <format>
#include "demo.hpp"

using namespace std;
using namespace filament;
using namespace filament::gltfio;
using namespace filament::viewer;
using namespace utils;
namespace fs = std::filesystem;

class DemoImp : public Demo
{
    filament::Engine *_engine;
    MaterialProvider *_materials;
    AssetLoader *_asset_loader;
    FilamentAsset *_asset;
    gltfio::FilamentInstance *_instance = nullptr;
    NameComponentManager *_names;
    ViewerGui *_viewer;
    AutomationSpec *_am_spec;
    AutomationEngine *_am_engine;

    gltfio::ResourceLoader *_res_loader;
    gltfio::TextureProvider *_stb_decoder = nullptr;
    gltfio::TextureProvider *_ktx_decoder = nullptr;

    string _gltf_path;

    virtual void set_gltf(std::string_view gltf_path) override
    {
        _gltf_path = gltf_path;

        fs::path path(gltf_path);

        if (fs::is_directory(path))
        {
            for (const auto &entry : fs::directory_iterator(path))
            {
                auto extension = entry.path().extension().string();
                if (extension == ".gltf" || extension == ".glb")
                {
                    _gltf_path = entry.path();
                    break;
                }
            }

            if (fs::is_directory(_gltf_path))
                throw runtime_error(format("no glTF file found in {}", gltf_path));

            cout << _gltf_path << endl;
        }
    }

    virtual void setup(filament::Engine *engine, filament::View *view, filament::Scene *scene) override
    {
        std::cout << "setup" << std::endl;
        _engine = engine;

        _names = new NameComponentManager(EntityManager::get());
        _viewer = new ViewerGui(engine, scene, view, 410);
        _materials = createJitShaderProvider(engine);

        _asset_loader = AssetLoader::create({engine, _materials, _names});
        _am_spec = AutomationSpec::generateDefaultTestCases();
        _am_engine = new AutomationEngine(_am_spec, &_viewer->getSettings());

        _asset = load_asset(_gltf_path);
        _instance = _asset->getInstance();

        load_resources(_gltf_path);

        _viewer->setAsset(_asset);

        // _viewer->setUiCallback([this, scene, view, engine](){
        //     float progress = this->_res_loader->asyncGetLoadProgress();
        //     if (progress < 1.0) {
        //         //ImGui::ProgressBar(progress);
        //     } else {
        //         // The model is now fully loaded, so let automation know.
        //         this->_am_engine->signalBatchMode();
        //     }
        // });
    }

    virtual void cleanup(filament::Engine *engine, filament::View *, filament::Scene *) override
    {
        std::cout << "cleanup" << std::endl;

        _am_engine->terminate();
        _res_loader->asyncCancelLoad();
        _asset_loader->destroyAsset(_asset);
        _materials->destroyMaterials();

        // engine->destroy(_scene.groundPlane);
        // engine->destroy(_scene.groundVertexBuffer);
        // engine->destroy(_scene.groundIndexBuffer);
        // engine->destroy(_scene.groundMaterial);
        // engine->destroy(_colorGrading);

        // engine->destroy(_scene.fullScreenTriangleVertexBuffer);
        // engine->destroy(_scene.fullScreenTriangleIndexBuffer);

        // auto& em = EntityManager::get();
        // for (auto e : _scene.overdrawVisualizer) {
        //     engine->destroy(e);
        //     em.destroy(e);
        // }

        // for (auto mi : _scene.overdrawMaterialInstances) {
        //     engine->destroy(mi);
        // }
        // engine->destroy(_scene.overdrawMaterial);

        delete _viewer;
        delete _materials;
        delete _names;
        delete _res_loader;
        delete _stb_decoder;
        delete _ktx_decoder;

        AssetLoader::destroy(&_asset_loader);
    }

    virtual void drop_handler(std::string_view path) override
    {
        std::cout << "drop_handler" << std::endl;

        load_asset(path);
        load_resources(path);
    }

    virtual void animate(filament::Engine *engine, filament::View *view, double now) override
    {
        _res_loader->asyncUpdateLoad();

        // Optionally fit the model into a unit cube at the origin.
        _viewer->updateRootTransform();

        // Gradually add renderables to the scene as their textures become ready.
        _viewer->populateScene();

        _viewer->applyAnimation(now);
    }

    // virtual void resize(filament::Engine *engine, filament::View *view);

    // virtual void gui(filament::Engine *engine, filament::View *view);

    // virtual void pre_render(filament::Engine* engine, filament::View* view, filament::Scene* scene, filament::Renderer* renderer);

    // virtual void post_render(filament::Engine* engine, filament::View* view, filament::Scene* scene, filament::Renderer* renderer);
private:    
    FilamentAsset *load_asset(std::string_view file_name)
    {
        ifstream ifs(file_name, ios::in | ios::binary);

        if (!ifs.is_open())
            throw runtime_error(format("cannot open file {}", file_name));

        istreambuf_iterator isb_iter(ifs);
        vector<uint8_t> buffer(isb_iter, {});

        // Parse the glTF file and create Filament entities.
        FilamentAsset *asset = _asset_loader->createAsset(buffer.data(), buffer.size());        

        if (!asset)
            throw runtime_error(format("unable to parse file {}", file_name));

        return asset;
    }

    void load_resources(std::string_view gltf_path)
    {
        fs::path path = gltf_path;

        ResourceConfiguration configuration = {};
        configuration.engine = _engine;
        configuration.gltfPath = gltf_path.data();
        configuration.normalizeSkinningWeights = true;

        _res_loader = new gltfio::ResourceLoader(configuration);
        _stb_decoder = createStbProvider(_engine);
        _ktx_decoder = createKtx2Provider(_engine);
        _res_loader->addTextureProvider("image/png", _stb_decoder);
        _res_loader->addTextureProvider("image/jpeg", _stb_decoder);
        _res_loader->addTextureProvider("image/ktx2", _ktx_decoder);

        if (!_res_loader->asyncBeginLoad(_asset))
            throw runtime_error(format("unable to start loading resource for {}", gltf_path));

        auto instance = _asset->getInstance();
        instance->recomputeBoundingBoxes();

        _asset->releaseSourceData();

        // Enable stencil writes on all material instances.
        const size_t matInstanceCount = _asset->getMaterialInstanceCount();
        MaterialInstance *const *const instances = _asset->getMaterialInstances();
        for (int mi = 0; mi < matInstanceCount; mi++)
        {
            instances[mi]->setStencilWrite(true);
            instances[mi]->setStencilOpDepthStencilPass(MaterialInstance::StencilOperation::INCR);
        }

        auto ibl = FilamentApp::get().getIBL();
        if (ibl)
        {
            _viewer->setIndirectLight(ibl->getIndirectLight(), ibl->getSphericalHarmonics());
        }
    }
};

std::shared_ptr<Demo> Demo::create()
{
    return static_pointer_cast<Demo>(make_shared<DemoImp>());
}