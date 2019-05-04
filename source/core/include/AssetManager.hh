#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "cstd.hh"
#include "util.hh"
#include "Array.hh"
#include "String.hh"
#include "Exception.hh"

#include "AssetHandle.hh"
#include "graphics/lib.hh"



namespace mod {
  struct AssetName {
    #ifndef CUSTOM_ASSET_NAME_MAX_LENGTH
      static constexpr size_t max_length = 128;
    #else
      static constexpr size_t max_length = CUSTOM_ASSET_NAME_MAX_LENGTH;
    #endif

    char value [max_length] = { 0 };

    AssetName ();
    AssetName (char const* in_value, size_t value_max_length = 0)
    {
      if (value_max_length == 0) value_max_length = strlen(in_value);
      memcpy(value, in_value, value_max_length);
      value[value_max_length] = '\0';
    }
  };

  template <typename T> struct AssetList {
    Array<AssetName> names;
    Array<T> assets;
    u32_t id_counter = 1;



    T* get_asset_by_id (u32_t id) const {
      if (id > 0 && id < id_counter) {
        for (auto [ i, asset ] : assets) {
          if (asset.asset_id == id) return &asset;
        }
      }

      return NULL;
    }

    char const* get_name_by_id (u32_t id) const {
      if (id > 0 && id < id_counter) {
        for (auto [ i, asset ] : assets) {
          if (asset.asset_id == id) return names[i].value;
        }
      }

      return NULL;
    }

    T* get_asset_by_name (char const* name) const {
      for (auto [ i, asset_name ] : names) {
        if (str_cmp_caseless(asset_name.value, name) == 0) return &assets[i];
      }

      return NULL;
    }

    AssetHandle<T> get_handle_by_name (char const* name) const {
      for (auto [ i, asset_name ] : names) {
        if (str_cmp_caseless(asset_name.value, name) == 0) return { assets[i].asset_id };
      }

      return { };
    }


    s64_t get_index_from_pointer (T const* asset) const {
      auto base_i = (size_t) assets.elements;
      auto asset_i = (size_t) asset;
      auto end_i = (size_t) (assets.elements + assets.count);

      if (asset_i >= base_i && asset_i < end_i) return (asset_i - base_i) / sizeof(T);
      else return -1;
    }

    s64_t get_index_from_id (u32_t id) const {
      for (auto [ i, asset ] : assets) {
        if (asset.asset_id == id) return i;
      }
      
      return -1;
    }

    s64_t get_index_from_name (char const* name) const {
      for (auto [ i, asset_name ] : names) {
        if (asset_name.value == name) return i;
      }
      
      return -1;
    }


    T& operator [] (u32_t id) const {
      T* ptr = get_asset_by_id(id);
      m_assert(ptr != NULL, "Cannot get %s asset from id %" PRIu32 ", no asset with this id exists; it may have been deleted", id);
      return *ptr;
    }

    T& operator [] (char const* name) const {
      T* ptr = get_asset_by_name(name);
      m_assert(ptr != NULL, "Cannot get %s asset from name '%s', no asset with this name exists; it may have been deleted", name);
      return ptr;
    }


    AssetHandle<T> add (char const* name, T const& asset) {
      size_t name_length = strlen(name);

      m_asset_assert(
        name_length < AssetName::max_length,
        asset.origin,
        "Failed to store this asset in the AssetManager: the name given '%s' is too long (Length is %zu, max is %zu)",
        name, name_length, AssetName::max_length
      );

      u32_t id = id_counter;
      u32_t index = assets.count;

      names.append({ name });

      assets.append(asset);

      assets[index].asset_id = id;

      ++ id_counter;

      return { id };
    }


    void remove (size_t index) {
      names.remove(index);
      assets[index].destroy();
      assets.remove(index);
    }


    void destroy () {
      names.destroy();
      for (auto [ i, asset ] : assets) asset.destroy();
      assets.destroy();
    }
  };
  


  struct WatchedFilePath {
    #ifndef CUSTOM_WATCHED_FILE_PATH_MAX_LENGTH
      static constexpr size_t max_length = 256;
    #else
      static constexpr size_t max_length = CUSTOM_WATCHED_FILE_PATH_MAX_LENGTH;
    #endif

    char value [max_length] = { 0 };

    WatchedFilePath () { }
    WatchedFilePath (char const* in_value, size_t length = 0)
    {
      if (length == 0) length = num::min(strlen(in_value), max_length);
      memcpy(value, in_value, length);
      value[length] = '\0';
    }
  };

  struct WatchedFile {
    time_t last_update;
    u8_t asset_type;
    u32_t asset_id;
    bool needs_update;
  };

  struct WatchedFileList {
    #ifndef CUSTOM_ASSET_MANAGER_WATCH_FILE_SLEEP_INTERVAL
      static constexpr struct timespec sleep_interval = { 0, 500000000 };
    #else
      static constexpr struct timespec sleep_interval = CUSTOM_ASSET_MANAGER_WATCH_FILE_SLEEP_INTERVAL;
    #endif
    

    mtx_t mtx;
    thrd_t thrd;
    bool shutdown;
    Array<WatchedFilePath> paths;
    Array<WatchedFile> files;

    s64_t get_index_from_path (char const* path) {
      for (auto [ i, i_path ] : paths) {
        if (str_cmp_caseless(path, i_path.value) == 0) return i;
      }

      return -1;
    }

    WatchedFile* get_file_from_path (char const* path) {
      s64_t index = get_index_from_path(path);
      if (index != -1) return &files[index];
      else return NULL;
    }

    void remove (size_t index) {
      paths.remove(index);
      files.remove(index);
    }

    void destroy () {
      paths.destroy();
      files.destroy();
    }
  };
  

  struct WatchedFileError {
    #ifndef CUSTOM_WATCHED_FILE_ERROR_MAX_LENGTH
      static constexpr size_t max_length = 512;
    #else
      static cosntexpr size_t max_length = CUSTOM_WATCHED_FILE_ERROR_MAX_LENGTH;
    #endif

    char value [max_length];

    WatchedFileError () { }
    WatchedFileError (char const* in_value, size_t length = 0)
    {
      if (length == 0) length = num::min(strlen(in_value), max_length);
      memcpy(value, in_value, length);
      value[length] = '\0';
    }
  };


  struct WatchedFileReport {
    WatchedFilePath path;
    bool have_error;
    WatchedFileError error;
  };


  struct AssetManager_t {
    static constexpr u8_t database_type = AssetType::total_type_count;

    AssetList<Shader> shader;
    AssetList<ShaderProgram> shader_program;
    AssetList<Texture> texture;
    AssetList<Material> material;
    AssetList<MaterialSet> material_set;
    AssetList<RenderMesh2D> render_mesh_2d;
    AssetList<RenderMesh3D> render_mesh_3d;

    WatchedFileList watch_list;

    bool have_lock;


    ENGINE_API AssetManager_t& init ();

    ENGINE_API void destroy ();

    ENGINE_API static s32_t watch_files (void* unused_thread_parameter);

    ENGINE_API void update_watched_files (Array<WatchedFileReport>* update_reports_output = NULL);

    ENGINE_API void load_database_from_json_item (char const* origin, JSONItem const& json, String* err_msg_output = NULL, bool watch_sub = true);

    void load_database_from_json (char const* origin, JSON const& json, String* err_msg_output = NULL, bool watch_sub = true) {
      return load_database_from_json_item(origin, json.data, err_msg_output, watch_sub);
    }

    ENGINE_API void load_database_from_str (char const* origin, char const* source, String* err_msg_output = NULL, bool watch_sub = true);

    ENGINE_API void load_database_from_file (char const* origin, String* err_msg_output = NULL, bool watch = true, bool watch_sub = true);


    template <typename T> void add_watched_file (char const* path, u32_t asset_id = 0) {
      static constexpr u8_t asset_type = AssetType::from_type<T>();

      static_assert(
        AssetType::validate(asset_type) || asset_type == database_type,
        "Cannot watch invalid Asset type"
      );


      size_t path_length = strlen(path);

      m_asset_assert(
        path_length < WatchedFilePath::max_length,
        path,
        "Cannot add file to AssetManager watched files list, the path is too long (Length is %zu, max is %zu)",
        path_length, WatchedFilePath::max_length
      );

      
      if constexpr (asset_type != database_type) {
        m_asset_assert(
          get_pointer_from_id<T>(asset_id) != NULL,
          path,
          "Cannot watch %s Asset with invalid id %" PRIu32,
          typeid(T).name(), asset_id
        );
      }


      bool took_lock = false;
      
      if (!have_lock) {
        mtx_lock_safe(&watch_list.mtx);
        took_lock = true;
        have_lock = true;
      }

      for (auto [ i, existing_path ] : watch_list.paths) {
        if (str_cmp_caseless(existing_path.value, path) == 0) {
          if (took_lock) {
            mtx_unlock_safe(&watch_list.mtx);
            have_lock = false;
          }
          return;
        }
      }

      watch_list.paths.append({ path });
      watch_list.files.append({
        time(NULL),
        asset_type,
        asset_id,
        false
      });

      if (took_lock) {
        mtx_unlock_safe(&watch_list.mtx);
        have_lock = false;
      }
    }

    ENGINE_API void remove_watched_file (char const* path);


    template <typename T> AssetList<T>& get_list () const {
      static_assert(
           std::is_same<T, Shader>::value
        || std::is_same<T, ShaderProgram>::value
        || std::is_same<T, Texture>::value
        || std::is_same<T, Material>::value
        || std::is_same<T, MaterialSet>::value
        || std::is_same<T, RenderMesh2D>::value
        || std::is_same<T, RenderMesh3D>::value,
        "Invalid type for get_list"
      );

      if constexpr (std::is_same<T, Shader>::value) return const_cast<AssetList<T>&>(shader);
      else if constexpr (std::is_same<T, ShaderProgram>::value) return const_cast<AssetList<T>&>(shader_program);
      else if constexpr (std::is_same<T, Texture>::value) return const_cast<AssetList<T>&>(texture);
      else if constexpr (std::is_same<T, Material>::value) return const_cast<AssetList<T>&>(material);
      else if constexpr (std::is_same<T, MaterialSet>::value) return const_cast<AssetList<T>&>(material_set);
      else if constexpr (std::is_same<T, RenderMesh2D>::value) return const_cast<AssetList<T>&>(render_mesh_2d);
      else if constexpr (std::is_same<T, RenderMesh3D>::value) return const_cast<AssetList<T>&>(render_mesh_3d);
      else m_error("Unknown error occurred");
    }


    template <typename T> AssetHandle<T> get (u32_t id) const {
      return { id };
    }

    template <typename T> T& get_reference (u32_t id) const {
      T* ptr = get_pointer_from_id<T>(id);
      m_asset_assert(
        ptr != NULL,
        "AssetManager",
        "Cannot get %s asset reference from id %" PRIu32 ", the asset may have been deleted",
        typeid(T).name(), id
      );
      return *ptr;
    }

    template <typename T> T* get_pointer_from_id (u32_t id) const {
      return get_list<T>().get_asset_by_id(id);
    }
    
    template <typename T> T* get_pointer_from_index (u32_t index) const {
      return get_list<T>().assets.get_element(index);
    }

    template <typename T> T* get_pointer_from_name (char const* name) const {
      return get_list<T>().get_asset_by_name(name);
    }

    template <typename T> AssetHandle<T> get (char const* name) const {
      return get_list<T>().get_handle_by_name(name);
    }

    template <typename T> AssetHandle<T> set (char const* name, T const& asset) {
      s64_t existing_index = get_index_from_name<T>(name);

      if (existing_index == -1) {
        return get_list<T>().add(name, asset);
      } else {
        T* existing_asset = get_pointer_from_index<T>(existing_index);

        if constexpr (std::is_same<T, Shader>::value) {
          m_asset_assert(
            asset.type == existing_asset->type,
            asset.origin,
            "Cannot replace existing Shader named '%s' (with origin '%s') of type %s with new one of type %s",
            name, existing_asset->origin, ShaderType::name(existing_asset->type), ShaderType::name(asset.type)
          );
        }


        u32_t asset_id = existing_asset->asset_id;

        existing_asset->destroy();

        *existing_asset = asset;

        existing_asset->asset_id = asset_id;
        

        if constexpr (std::is_same<T, Shader>::value) {
          for (auto [ i, program ] : shader_program.assets) {
            if (program.vertex_shader.get_id() == asset_id
            ||  program.fragment_shader.get_id() == asset_id
            ||  program.tesselation_control_shader.get_id() == asset_id
            ||  program.tesselation_evaluation_shader.get_id() == asset_id
            ||  program.geometry_shader.get_id() == asset_id
            ||  program.compute_shader.get_id() == asset_id) {
              ShaderProgram new_program = {
                program.origin,
                program.vertex_shader,
                program.fragment_shader,
                program.tesselation_control_shader,
                program.tesselation_evaluation_shader,
                program.geometry_shader,
                program.compute_shader
              };

              new_program.asset_id = program.asset_id;

              program.destroy();

              program = new_program;
            }
          }
        }

        return { asset_id };
      }
    }


    template <typename T, typename ... A> AssetHandle<T> create_asset (char const* name, char const* origin, A ... args) {
      T asset = T { origin, args... };

      try {
        return set<T>(name, asset);
      } catch (Exception& exception) {
        asset.destroy();
        throw exception;
      }
    }

    template <typename T> AssetHandle<T> create_asset_from_json_item (char const* name, char const* origin, JSONItem const& json) {
      T asset = T::from_json_item(origin, json);

      try {
        return set<T>(name, asset);
      } catch (Exception& exception) {
        asset.destroy();
        throw exception;
      }
    }

    template <typename T> AssetHandle<T> create_asset_from_json (char const* name, char const* origin, JSON const& json) {
      T asset = T::from_json(origin, json);

      try {
        return set<T>(name, asset);
      } catch (Exception& exception) {
        asset.destroy();
        throw exception;
      }
    }

    template <typename T> AssetHandle<T> create_asset_from_str (char const* name, char const* origin, char const* source) {
      T asset = T::from_str(origin, source);

      try {
        return set<T>(name, asset);
      } catch (Exception& exception) {
        asset.destroy();
        throw exception;
      }
    }

    template <typename T> AssetHandle<T> create_asset_from_file (char const* name, char const* origin, bool watch_file = true) {
      T asset = T::from_file(origin);

      AssetHandle<T> handle;
      
      try {
        handle = set<T>(name, asset);
      } catch (Exception& exception) {
        asset.destroy();
        throw exception;
      }

      if (watch_file) {
        try {
          add_watched_file<T>(origin, handle.get_id());
        } catch (Exception& exception) {
          handle.destroy_asset();
          throw exception;
        }
      }

      return handle;
    }

    template <typename T> void remove (char const* name) {
      s64_t index = get_index_from_name<T>(name);

      if (index == -1) return;

      T* value = get_pointer_from_index<T>(index);
      remove_watched_file(value->origin);
      get_list<T>().remove(index);
    }

    template <typename T> void remove (AssetHandle<T> handle) {
      s64_t index = get_index_from_id<T>(handle.get_id());

      if (index == -1) return;

      T* value = get_pointer_from_index<T>(index);
      remove_watched_file(value->origin);
      get_list<T>().remove(index);
    }


    template <typename T> char const* get_name_from_id (u32_t id) const {
      s64_t index = get_index_from_id<T>(id);

      if (index == -1) return NULL;
      else return get_list<T>().names[index].value;
    }

    template <typename T> char const* get_name_from_pointer (T const* value) const {
      s64_t index = get_index_from_pointer<T>(value);

      if (index == -1) return NULL;
      else return get_list<T>().names[index].value;
    }

    template <typename T> s64_t get_index_from_pointer (T const* value) const {
      if (value == NULL || value->asset_id == 0) return -1;

      return get_list<T>().get_index_from_pointer(value);
    }

    template <typename T> s64_t get_index_from_id (u32_t id) const {
      return get_list<T>().get_index_from_id(id);
    }

    template <typename T> s64_t get_index_from_name (char const* name) const {
      return get_list<T>().get_index_from_name(name);
    }

    template <typename T> AssetHandle<T> get_handle_from_pointer (T const* value) const {
      s64_t index = get_index_from_pointer<T>(value);

      if (index != -1) return { (u32_t) index, value->asset_id };
      else return { 0 };
    }

    template <typename T> T* get_base_pointer () const {
      return (T*) get_list<T>().assets.elements;
    }

    template <typename T> AssetName const* get_name_base_pointer () const {
      return get_list<T>().names.elements;
    }

    template <typename T> u32_t get_count () const {
      return get_list<T>().assets.count;
    }
  };

  ENGINE_API extern AssetManager_t AssetManager;
}

#endif