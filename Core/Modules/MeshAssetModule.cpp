//
// Created by alex on 11.03.25.
//

#include "MeshAssetModule.h"

#include <MeshIo.h>

#include "Engine.h"
#include "Logger.h"
#include "Pool.h"
#include "PoolHandle.h"

namespace Bcg {
    using MeshAssetPool = Pool<Mesh>;
    using MeshAssetInstancePool = Pool<Mesh>;
    using MeshAssetCache = std::unordered_map<std::string, PoolHandle<Mesh>>;

    MeshAssetModule::MeshAssetModule() : Module("MeshAssetModule", "0.1") {

    }

    void MeshAssetModule::connect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().connect<&MeshAssetModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().connect<&MeshAssetModule::on_shutdown>(this);
        Engine::get_dispatcher().sink<Events::Drop>().connect<&MeshAssetModule::on_drop_file>(this);
        Module::connect_events();
    }

    void MeshAssetModule::disconnect_events() {
        Engine::get_dispatcher().sink<Events::Initialize>().disconnect<&MeshAssetModule::on_initialize>(this);
        Engine::get_dispatcher().sink<Events::Shutdown>().disconnect<&MeshAssetModule::on_shutdown>(this);
        Engine::get_dispatcher().sink<Events::Drop>().disconnect<&MeshAssetModule::on_drop_file>(this);
        Module::disconnect_events();
    }

    void MeshAssetModule::on_initialize(const Events::Initialize &event) {
        Engine::get_context().emplace<MeshAssetPool>("MeshAssetPool");
        Engine::get_context().emplace<MeshAssetInstancePool>("MeshAssetInstancePool");
        Engine::get_context().emplace<MeshAssetCache>();
        Module::on_initialize(event);
    }

    void MeshAssetModule::on_startup(const Events::Startup &event) {
        Module::on_startup(event);
    }

    void MeshAssetModule::on_synchronize(const Events::Synchronize &event) {
        Module::on_synchronize(event);
    }

    void MeshAssetModule::on_shutdown(const Events::Shutdown &event) {
        Module::on_shutdown(event);
    }

    void MeshAssetModule::on_drop_file(const Events::Drop &event) {
        auto &mesh_asset_cache = Engine::get_context().get<MeshAssetCache>();
        auto &mesh_asset_pool = Engine::get_context().get<MeshAssetPool>();
        for (int i = 0; i < event.count; ++i) {
            std::string filepath = event.paths[i];
            MeshIoManager mesh_io(filepath);
            if (!mesh_io.can_load_file()) continue;
            if (mesh_asset_cache.find(filepath) == mesh_asset_cache.end()) {
                auto mesh_handle = mesh_asset_pool.create_handle();
                Mesh &mesh = mesh_handle;
                mesh_io.read(mesh);
                LOG_INFO(fmt::format("{}::on_drop_file: New mesh {} to pool", name, filepath));
                assert(mesh_handle.GetIndex() < mesh_asset_pool.get_objects().size());
                mesh_asset_cache[filepath] = mesh_handle;
            }

        }

    }
}