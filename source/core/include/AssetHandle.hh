#ifndef ASSET_HANDLE_H
#define ASSET_HANDLE_H

#include "cstd.hh"
#include "util.hh"


namespace mod {
  struct Shader;
  struct ShaderProgram;
  struct Texture;
  struct Material;
  struct MaterialSet;
  struct RenderMesh2D;
  struct RenderMesh3D;


  namespace AssetType {
    enum: u8_t {
      Shader,
      ShaderProgram,
      Texture,
      Material,
      MaterialSet,
      RenderMesh2D,
      RenderMesh3D,

      total_type_count,

      Invalid = (u8_t) -1
    };

    static constexpr char const* names [total_type_count] = {
      "Shader",
      "ShaderProgram",
      "Texture",
      "Material",
      "MaterialSet",
      "RenderMesh2D",
      "RenderMesh3D"
    };

    /* Get the name of an AssetType as a str */
    static constexpr char const* name (u8_t type) {
      if (type < total_type_count) return names[type];
      return "Invalid";
    }
    
    /* Get an AssetType from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t type = 0; type < total_type_count; type ++) {
        if ((max_length == SIZE_MAX || strlen(names[type]) == max_length) && str_cmp_caseless(name, names[type], max_length) == 0) return type;
      }

      return Invalid;
    }

    /* Determine if a value is a valid AssetType */
    static constexpr bool validate (u8_t type) {
      return type < total_type_count;
    }

    /* Get an AssetType enum from a type */
    template <typename T> constexpr s8_t from_type () {
      if constexpr (std::is_same<T, ::mod::Shader>::value) return Shader;
      else if constexpr (std::is_same<T, ::mod::ShaderProgram>::value) return ShaderProgram;
      else if constexpr (std::is_same<T, ::mod::Texture>::value) return Texture;
      else if constexpr (std::is_same<T, ::mod::Material>::value) return Material;
      else if constexpr (std::is_same<T, ::mod::MaterialSet>::value) return MaterialSet;
      else if constexpr (std::is_same<T, ::mod::RenderMesh2D>::value) return RenderMesh2D;
      else if constexpr (std::is_same<T, ::mod::RenderMesh3D>::value) return RenderMesh3D;
      else return Invalid;
    }
  }


  template <typename T> struct AssetHandle {
    bool valid = false;
    bool managed = false;
    union {
      // TODO try storing last known index of managed assets; This is a tradeoff between constness of users and avoiding extra lookups, hard to say which is better as of now
      u32_t asset_id;
      T* direct = NULL;
    };

    AssetHandle () { }

    AssetHandle (u32_t id) {
      if (id != 0) {
        T* ptr = AssetManager.get_pointer_from_id<T>(id);
        
        if (ptr != NULL) {
          valid = true;
          managed = true;
          asset_id = id;
        }
      }
    }

    AssetHandle (char const* name) {
      if (name != NULL) {
        T* ptr = AssetManager.get_pointer_from_name<T>(name);

        if (ptr != NULL) {
          valid = true;
          managed = true;
          asset_id = ptr->asset_id;
        }
      }
    }
    
    AssetHandle (T* asset) {
      if (asset != NULL) {
        valid = true;
        if (asset->asset_id != 0) {
          managed = true;
          asset_id = asset->asset_id;
        } else {
          managed = false;
          direct = asset;
        }
      }
    }

    AssetHandle& operator = (u32_t id) {
      if (id != 0) {
        T* ptr = AssetManager.get_pointer_from_id<T>(id);
        
        if (ptr != NULL) {
          valid = true;
          managed = true;
          asset_id = id;

          return *this;
        }
      }

      valid = false;
      return *this;
    }

    AssetHandle& operator = (char const* name) {
      if (name != NULL) {
        T* ptr = AssetManager.get_pointer_from_name<T>(name);

        if (ptr != NULL) {
          valid = true;
          managed = true;
          asset_id = ptr->asset_id;

          return *this;
        } 
      }

      valid = false;
      return *this;
    }

    AssetHandle& operator = (T* asset) {
      if (asset != NULL) {
        valid = true;
        
        if (asset->asset_id != 0) {
          managed = true;
          asset_id = asset->asset_id;
        } else {
          managed = false;
          direct = asset;
        }

        return *this;
      }

      valid = false;
      return *this;
    }

    s32_t get_id () const {
      if (valid && managed) return asset_id;
      else return 0;
    }

    T* get_ptr () const {
      if (valid) {
        if (managed) {
          return AssetManager.get_pointer_from_id<T>(asset_id);
        } else return direct;
      } else return NULL;
    }

    T& dereference () const {
      T* ptr = get_ptr();
      m_assert(ptr != NULL, "AssetHandle<%s> pointer was null", typeid(T).name());
      return *ptr;
    }


    T& operator * () const {
      return dereference();
    }

    T* operator -> () const {
      return &dereference();
    }

    void clear () {
      valid = false;
      managed = false;
      direct = NULL;
    }

    void destroy () {
      if (!valid) return;

      if (managed) {
        AssetManager.remove(*this);
      } else {
        direct->destroy();
      }

      clear();
    }
  };
}

#endif