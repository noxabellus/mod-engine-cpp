#include "../../include/graphics/lib.hh"
#include "../../include/AssetManager.hh"



namespace mod {
  MaterialSet MaterialSet::from_json_item (char const* origin, JSONItem const& json) {
    MaterialSet material_set = { origin };

    try {
      for (size_t i = 0; i < json.get_array().count; i ++) {
        String& material_name = json.get_array_string(i);

        Material* material = AssetManager.get_pointer_from_name<Material>(material_name.value);

        if (material == NULL) {
          json.get_array_item(i)->asset_error(
            "No Material named '%s' has been loaded",
            material_name.value
          );
        }

        material_set.append(material);
      }
    } catch (Exception& exception) {
      material_set.destroy();
      throw exception;
    }

    return material_set;
  }

  MaterialSet MaterialSet::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    MaterialSet material_set;

    try {
      material_set = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return material_set;
  }

  MaterialSet MaterialSet::from_file (char const* origin) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load MaterialSet: Unable to read file"
    );

    MaterialSet material_set;

    try {
      material_set = from_str(origin, (char*) source);
    } catch (Exception& exception) {
      free(source);
      throw exception;
    }

    free(source);

    return material_set;
  }


  void MaterialSet::destroy () {
    if (origin != NULL) {
      free(origin);
      origin = NULL;
    }

    materials.destroy();
  }
}