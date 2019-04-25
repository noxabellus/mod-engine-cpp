#include "../include/AssetManager.hh"



namespace mod {
  AssetManager_t AssetManager = { };

  AssetManager_t& AssetManager_t::init () {
    have_lock = false;
    watch_list.shutdown = false;
    mtx_init_safe(&watch_list.mtx, mtx_plain);
    thrd_create_safe(&watch_list.thrd, (thrd_start_t) AssetManager_t::watch_files, NULL);
    return *this;
  }

  void AssetManager_t::destroy () {
    m_assert(!have_lock, "Unknown error occurred: Already had mutex lock at start of AssetManager::destroy");

    mtx_lock_safe(&watch_list.mtx);

    watch_list.shutdown = true;

    mtx_unlock_safe(&watch_list.mtx);

    thrd_join_safe(AssetManager.watch_list.thrd, NULL);

    watch_list.destroy();

    shader.destroy();
    shader_program.destroy();
    // texture.destroy();
    // material.destroy();
    // material_set.destroy();
    // render_mesh_2d.destroy();
    // render_mesh_3d.destroy();
  }

  s32_t AssetManager_t::watch_files (void* _) {
    while (true) {
      mtx_lock_safe(&AssetManager.watch_list.mtx);

      if (AssetManager.watch_list.shutdown == true) {
        mtx_unlock_safe(&AssetManager.watch_list.mtx);

        return 0;
      }

      for (auto [ i, file ] : AssetManager.watch_list.files) {
        if (file.needs_update) continue;

        struct stat file_stats;

        // TODO should we stop tracking deleted files? They could be replaced but not sure how this should be handled yet
        if (stat(AssetManager.watch_list.paths[i].value, &file_stats) != 0) continue;


        f64_t time_diff = difftime(file_stats.st_mtime, file.last_update);

        if (time_diff > 0.0) file.needs_update = true;
      }

      mtx_unlock_safe(&AssetManager.watch_list.mtx);

      thrd_sleep_safe(&WatchedFileList::sleep_interval, NULL);
    }
  }


  void AssetManager_t::update_watched_files (Array<WatchedFileReport>* update_reports_output) {
    static String report_error_intermediate;
    static WatchedFileReport report;

    m_assert(!have_lock, "Unknown error occurred, already had mutex lock at start of AssetManager::update_watched_files");

    mtx_lock_safe(&watch_list.mtx);
    have_lock = true;

    time_t curr_time = time(NULL);


    for (auto [ i, file ] : watch_list.files) {
      if (!file.needs_update) continue;

      char const* path = watch_list.paths[i].value;

      char const* name;

      if (update_reports_output != NULL) {
        report_error_intermediate.clear();

        report = { path, false };
      }

      try {
        switch (file.asset_type) {
          case AssetType::Shader: {
            name = shader.get_name_by_id(file.asset_id);

            create_asset_from_file<Shader>(name, path, false);
          } break;

          case AssetType::ShaderProgram: {
            name = shader_program.get_name_by_id(file.asset_id);

            create_asset_from_file<ShaderProgram>(name, path, false);
          } break;

          case AssetType::Texture: {
            name = texture.get_name_by_id(file.asset_id);

            create_asset_from_file<Texture>(name, path, false);
          } break;

          case AssetType::Material: {
            name = material.get_name_by_id(file.asset_id);

            create_asset_from_file<Material>(name, path, false);
          } break;

          case AssetType::MaterialSet: {
            name = material_set.get_name_by_id(file.asset_id);

            create_asset_from_file<MaterialSet>(name, path, false);
          } break;

          case AssetType::RenderMesh2D: {
            name = render_mesh_2d.get_name_by_id(file.asset_id);

            create_asset_from_file<RenderMesh2D>(name, path, false);
          } break;

          case AssetType::RenderMesh3D: {
            name = render_mesh_3d.get_name_by_id(file.asset_id);

            create_asset_from_file<RenderMesh3D>(name, path, false);
          } break;

          case database_type: {
            if (update_reports_output != NULL) {
              load_database_from_file(path, &report_error_intermediate, false, true);
              if (report_error_intermediate.length > 0) {
                report.have_error = true;
                report.error = report_error_intermediate.value;
              }
            } else {
              load_database_from_file(path, NULL, false, true);
            }
          } break;

          default: m_asset_error(path, "AssetManager cannot update watched file, the AssetType (%" PRIu8 ") is invalid", file.asset_type);
        }
      } catch (Exception& exception) {
        if (update_reports_output != NULL) {
          report.have_error = true;
          exception.print(report_error_intermediate);
          report.error = report_error_intermediate.value;
        } else {
          exception.print();
        }

        exception.handle();
      }

      file.needs_update = false;
      file.last_update = curr_time;

      if (update_reports_output != NULL) {
        update_reports_output->append(report);
      }
    }

    m_assert(have_lock, "Unknown error occured: Did not have mutex lock at end of AssetManager::update_watched_files");
    mtx_unlock_safe(&watch_list.mtx);
    have_lock = false;
  }


  void AssetManager_t::remove_watched_file (char const* path) {
    bool took_lock = false;
    if (!have_lock) {
      mtx_lock_safe(&watch_list.mtx);
      have_lock = true;
      took_lock = true;
    }

    s64_t index = watch_list.get_index_from_path(path);

    if (index != -1) watch_list.remove(index);
    
    if (took_lock) {
      mtx_unlock_safe(&watch_list.mtx);
      have_lock = false;
    }
  }


  void AssetManager_t::load_database_from_json_item (char const* origin, JSONItem const& json, String* err_msg_output, bool watch_sub) {
    JSONItem* shaders_item = json.get_object_item("shaders");
    JSONItem* shader_programs_item = json.get_object_item("shader_programs");
    JSONItem* textures_item = json.get_object_item("textures");
    JSONItem* materials_item = json.get_object_item("materials");
    JSONItem* material_sets_item = json.get_object_item("material_sets");
    JSONItem* render_mesh_2ds_item = json.get_object_item("render_mesh_2ds");
    JSONItem* render_mesh_3ds_item = json.get_object_item("render_mesh_3ds");


    char relative_path [1024];

    size_t origin_len = strlen(origin);

    const auto get_file_rel_path = [&] (JSONItem const& path_item) -> char* {
      String& path_string = path_item.get_string();

      bool ok = str_dir_relativize_path(origin, path_string.value, relative_path, 1023);

      path_item.asset_assert(
        ok,
        "The path '%s' cannot be relativized using the current database path: "
        "The resulting path is either too long, or the database path is not deep enough to accomodate the back tracking required",
        path_string.value
      );

      return relative_path;
    };

    const auto get_sub_rel_path = [&] (char const* type, String const& name, JSONItem const& ref_item) -> char* {
      size_t type_len = strlen(type);
      size_t len = origin_len + type_len + name.length + 2;
      ref_item.asset_assert(
        len < 1024,
        "The name '%s' combined with the current database path and its type produces an origin '%s:%s:%s' which is too long (Length %zu, max 1024)",
        name.value, origin, type, name.value, len
      );

      memcpy(relative_path, origin, origin_len);
      
      char* type_pos = relative_path + origin_len;
      *type_pos = ':';
      ++ type_pos;
      memcpy(type_pos, type, type_len);

      char* name_pos = type_pos + type_len;
      *name_pos = ':';
      ++ name_pos;
      memcpy(name_pos, name.value, name.length + 1);

      return relative_path;
    };


    if (shaders_item != NULL) {
      JSONObject& shaders_obj = shaders_item->get_object();

      for (auto [ i, name ] : shaders_obj.keys) {
        try {
          create_asset_from_file<Shader>(
            name.value,
            get_file_rel_path(shaders_obj.items[i]),
            watch_sub
          );
        } catch (Exception& exception) {
          if (err_msg_output != NULL) {
            exception.print(*err_msg_output);
          } else {
            exception.print();
          }

          exception.handle();
        }
      }
    }

    if (shader_programs_item != NULL) {
      JSONObject shader_programs_obj = shader_programs_item->get_object();

      for (auto [ i, name ] : shader_programs_obj.keys) {
        JSONItem& item = shader_programs_obj.items[i];

        try {
          if (item.type == JSONType::String) {
            create_asset_from_file<ShaderProgram>(
              name.value,
              get_file_rel_path(item),
              watch_sub
            );
          } else {
            create_asset_from_json_item<ShaderProgram>(
              name.value,
              get_sub_rel_path("shader_programs", name, item),
              item
            );
          }
        } catch (Exception& exception) {
          if (err_msg_output != NULL) {
            exception.print(*err_msg_output);
          } else {
            exception.print();
          }
          
          exception.handle();
        }
      }
    }

    if (textures_item != NULL) {
      JSONObject textures_obj = textures_item->get_object();

      for (auto [ i, name ] : textures_obj.keys) {
        JSONItem& item = textures_obj.items[i];

        try {
          if (item.type == JSONType::String) {
            create_asset_from_file<Texture>(
              name.value,
              get_file_rel_path(item),
              watch_sub
            );
          } else {
            create_asset_from_json_item<Texture>(
              name.value,
              get_sub_rel_path("textures", name, item),
              item
            );
          }
        } catch (Exception& exception) {
          if (err_msg_output != NULL) {
            exception.print(*err_msg_output);
          } else {
            exception.print();
          }
          
          exception.handle();
        }
      }
    }

    if (materials_item != NULL) {
      JSONObject materials_obj = materials_item->get_object();

      for (auto [ i, name ] : materials_obj.keys) {
        JSONItem& item = materials_obj.items[i];
        
        try {
          if (item.type == JSONType::String) {
            create_asset_from_file<Material>(
              name.value,
              get_file_rel_path(item),
              watch_sub
            );
          } else {
            create_asset_from_json_item<Material>(
              name.value,
              get_sub_rel_path("materials", name, item),
              item
            );
          }
        } catch (Exception& exception) {
          if (err_msg_output != NULL) {
            exception.print(*err_msg_output);
          } else {
            exception.print();
          }
          
          exception.handle();
        }
      }
    }

    if (material_sets_item != NULL) {
      JSONObject material_sets_obj = material_sets_item->get_object();

      for (auto [ i, name ] : material_sets_obj.keys) {
        JSONItem& item = material_sets_obj.items[i];
        
        try {
          if (item.type == JSONType::String) {
            create_asset_from_file<MaterialSet>(
              name.value,
              get_file_rel_path(item),
              watch_sub
            );
          } else {
            create_asset_from_json_item<MaterialSet>(
              name.value,
              get_sub_rel_path("material_sets", name, item),
              item
            );
          }
        } catch (Exception& exception) {
          if (err_msg_output != NULL) {
            exception.print(*err_msg_output);
          } else {
            exception.print();
          }
          
          exception.handle();
        }
      }
    }

    if (render_mesh_2ds_item != NULL) {
      JSONObject render_mesh_2ds_obj = render_mesh_2ds_item->get_object();

      for (auto [ i, name ] : render_mesh_2ds_obj.keys) {
        JSONItem& item = render_mesh_2ds_obj.items[i];
        
        try {
          if (item.type == JSONType::String) {
            create_asset_from_file<RenderMesh2D>(
              name.value,
              get_file_rel_path(item),
              watch_sub
            );
          } else {
            create_asset_from_json_item<RenderMesh2D>(
              name.value,
              get_sub_rel_path("render_mesh_2ds", name, item),
              item
            );
          }
        } catch (Exception& exception) {
          if (err_msg_output != NULL) {
            exception.print(*err_msg_output);
          } else {
            exception.print();
          }
          
          exception.handle();
        }
      }
    }

    if (render_mesh_3ds_item != NULL) {
      JSONObject render_mesh_3ds_obj = render_mesh_3ds_item->get_object();

      for (auto [ i, name ] : render_mesh_3ds_obj.keys) {
        JSONItem& item = render_mesh_3ds_obj.items[i];
        
        try {
          if (item.type == JSONType::String) {
            create_asset_from_file<RenderMesh3D>(
              name.value,
              get_file_rel_path(item),
              watch_sub
            );
          } else {
            create_asset_from_json_item<RenderMesh3D>(
              name.value,
              get_sub_rel_path("render_mesh_3ds", name, item),
              item
            );
          }
        } catch (Exception& exception) {
          if (err_msg_output != NULL) {
            exception.print(*err_msg_output);
          } else {
            exception.print();
          }
          
          exception.handle();
        }
      }
    }
  }

  void AssetManager_t::load_database_from_str (char const* origin, char const* source, String* err_msg_output, bool watch_sub) {
    JSON json = JSON::from_str(origin, source);

    try {
      load_database_from_json(origin, json, err_msg_output, watch_sub);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();
  }

  void AssetManager_t::load_database_from_file (char const* origin, String* err_msg_output, bool watch, bool watch_sub) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load AssetManager database: Unable to read file"
    );

    try {
      load_database_from_str(origin, (char*) source, err_msg_output, watch_sub);
    } catch (Exception& exception) {
      free(source);
      throw exception;
    }

    free(source);
  }
}