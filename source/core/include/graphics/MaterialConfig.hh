#ifndef MATERIAL_CONFIG_H
#define MATERIAL_CONFIG_H

#include "../Array.hh"
#include "../JSON.hh"


namespace mod {
  struct MaterialInfo {
    size_t material_index;
    size_t start_index;
    size_t length;
    bool cast_shadow;

    MaterialInfo () { }
    MaterialInfo (size_t in_material_index, size_t in_start_index, size_t in_length, bool in_cast_shadow = false)
    : material_index(in_material_index)
    , start_index(in_start_index)
    , length(in_length)
    , cast_shadow(in_cast_shadow)
    { }
  };

  
  struct MaterialConfig {
    bool multi_material;
    union {
      struct {
        size_t material_index;
        bool cast_shadow;
      };
      Array<MaterialInfo> materials;
    };

    MaterialConfig ()
    : multi_material(false)
    , material_index(0)
    , cast_shadow(false)
    { }

    MaterialConfig (size_t in_material_index, bool in_cast_shadow = false)
    : multi_material(false)
    , material_index(in_material_index)
    , cast_shadow(in_cast_shadow)
    { }

    MaterialConfig (Array<MaterialInfo> const& in_materials)
    : multi_material(true)
    { materials.copy(in_materials); }

    static MaterialConfig empty_multi () {
      MaterialConfig config;
      config.multi_material = true;
      config.materials = { };
      return config;
    }

    static MaterialConfig from_ex (Array<MaterialInfo> const& in_materials) {
      MaterialConfig config;
      config.multi_material = true;
      config.materials = in_materials;
      return config;
    }

    static MaterialConfig from_json_item (size_t face_count, JSONItem const& item) {
      MaterialConfig material_config;

      if (item.type == JSONType::Object) {
        bool cast_shadow;

        JSONItem* cast_shadow_item = item.get_object_item("cast_shadow");

        if (cast_shadow_item != NULL) {
          cast_shadow = cast_shadow_item->get_boolean();
        } else {
          cast_shadow = false;
        }

        material_config = {
          static_cast<size_t>(item.get_object_number("material_index")),
          cast_shadow
        };
      } else if (item.type == JSONType::Array) {
        material_config.multi_material = true;
        material_config.materials = { };

        for (auto [ i, material_element_item ] : item.get_array()) {
          bool cast_shadow;

          JSONItem* cast_shadow_item = material_element_item.get_object_item("cast_shadow");

          if (cast_shadow_item != NULL) {
            cast_shadow = cast_shadow_item->get_boolean();
          } else {
            cast_shadow = false;
          }

          JSONItem* start_index_item = material_element_item.get_object_item("start_index");
          JSONItem* length_item = material_element_item.get_object_item("length");

          material_element_item.asset_assert(start_index_item != NULL, "Expected a number 'start_index'");
          material_element_item.asset_assert(length_item != NULL, "Expected a number 'length'");

          MaterialInfo info = {
            static_cast<size_t>(material_element_item.get_object_number("material_index")),
            static_cast<size_t>(start_index_item->get_number()),
            static_cast<size_t>(length_item->get_number()),
            cast_shadow
          };

          start_index_item->asset_assert(
            info.start_index < face_count,
            "Starting face index %zu is out of range, face count of mesh is only %zu",
            info.start_index, face_count
          );

          size_t last_index = info.start_index + info.length;

          length_item->asset_assert(
            last_index <= face_count,
            "Starting face index %zu with length %zu (last index == %zu) is out of range, face count of the mesh is only %zu",
            info.start_index, info.length, last_index, face_count
          );

          material_config.append(info);
        }
      } else item.asset_error(
        "Expected an Object containing a single-material configuration or an Array containing multi-material configurations, not a %s",
        JSONType::name(item.type)
      );

      return material_config;
    }

    void clear () {
      if (multi_material) {
        materials.clear();
      } else {
        material_index = 0;
        cast_shadow = false;
      }
    }

    void destroy () {
      if (multi_material) {
        materials.destroy();
        multi_material = false;
      }

      clear();
    }


    MaterialInfo& operator [] (size_t index) const {
      m_assert(multi_material, "Cannot get MaterialInfo section %zu because multi material mode is not enabled", index);
      return materials[index];
    }

    ArrayIterator<MaterialInfo> begin () const {
      m_assert(multi_material, "Cannot get MaterialInfo iterator because multi-material mode is not enabled");
      return materials.begin();
    }
    
    ArrayIterator<MaterialInfo> end () const {
      m_assert(multi_material, "Cannot get MaterialInfo iterator because multi-material mode is not enabled");
      return materials.end();
    }


    void append (MaterialInfo const& info) {
      m_assert(multi_material, "Cannot append MaterialInfo element because multi-material mode is not enabled");
      materials.append(info);
    }

    void insert (size_t index, MaterialInfo const& info) {
      m_assert(multi_material, "Cannot insert MaterialInfo element because multi-material mode is not enabled");
      materials.insert(index, info);
    }

    void remove (size_t index) {
      m_assert(multi_material, "Cannot remove MaterialInfo element because multi-material mode is not enabled");
      materials.remove(index);
    }
  };
}

#endif