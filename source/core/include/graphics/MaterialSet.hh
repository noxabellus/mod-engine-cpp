#ifndef MATERIALSET_H
#define MATERIALSET_H

#include "../cstd.hh"
#include "../util.hh"
#include "../Array.hh"
#include "../JSON.hh"
#include "../Exception.hh"

#include "../AssetHandle.hh"

#include "Material.hh"



namespace mod {
  struct MaterialSet {
    char* origin;
    u32_t asset_id = 0;

    Array<MaterialHandle> materials;


    /* Create a new uninitialized MaterialSet */
    MaterialSet () { }

    /* Create a new empty MaterialSet with an initialized origin */
    MaterialSet (char const* in_origin)
    : origin(str_clone(in_origin))
    { }

    /* Create a new MaterialSet and initialize its material array from a buffer of material refs */
    MaterialSet (char const* in_origin, MaterialHandle const* in_materials, size_t material_count)
    : origin(str_clone(in_origin))
    { for (size_t i = 0; i < material_count; i ++) materials.append(in_materials[i]); }

    /* Create a new MaterialSet and initialize its material array by copying from an array of material handles */
    MaterialSet (char const* in_origin, Array<MaterialHandle> const& in_materials)
    : MaterialSet(in_origin, in_materials.elements, in_materials.count)
    { }

    /* Create a new MaterialSet from a parameter pack list of Materials */
    template <typename ... A> static MaterialSet from_elements (char const* origin, A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      MaterialHandle materials [arg_count] = { ((MaterialHandle) args)... };
      return { origin, materials, arg_count };
    }

    /* Create a new MaterialSet from an existing Array of MaterialHandles and take ownership of the Array */
    static MaterialSet from_ex (char const* origin, Array<MaterialHandle> const& materials) {
      MaterialSet material_set;
      material_set.origin = str_clone(origin);
      material_set.materials = materials;
      return material_set;
    }


    /* Create a new MaterialSet from a JSONItem */
    ENGINE_API static MaterialSet from_json_item (char const* origin, JSONItem const& json);

    /* Create a new MaterialSet from JSON */
    static MaterialSet from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new MaterialSet from a source str */
    ENGINE_API static MaterialSet from_str (char const* origin, char const* source);

    /* Create a new MaterialSet from a source file */
    ENGINE_API static MaterialSet from_file (char const* origin);


    /* Clean up a MaterialSet's heap allocations */
    ENGINE_API void destroy ();


    /* Get a pointer to a MaterialHandle at a given index of a MaterialSet.
     * Returns NULL if the index is out of range */
    MaterialHandle* get (size_t index) const {
      return materials.get_element(index);
    }

    /* Get a MaterialHandle at a given index of a MaterialSet.
     * Panics if the index is out of range */
    MaterialHandle& operator [] (size_t index) const {
      return materials[index];
    }

    /* Get an ArrayIterator for the beginning of a MaterialSet */
    ArrayIterator<MaterialHandle> begin () const {
      return materials.begin();
    }

    /* Get an ArrayIterator for the end of a MaterialSet */    
    ArrayIterator<MaterialHandle> end () const {
      return materials.end();
    }

    /* Set a MaterialHandle at a given index of a MaterialSet.
     * Panics if the index is out of range */
    void set (size_t index, MaterialHandle const& material) {
      materials[index] = material;
    }


    /* Add a Material to the end of a MaterialSet */
    void append (MaterialHandle const& material) {
      materials.append(material);
    }

    /* Insert a Material at a given index in a MaterialSet */
    void insert (size_t index, MaterialHandle const& material) {
      materials.insert(index, material);
    }

    /* Remove a Material at a given index in a MaterialSet */
    void remove (size_t index) {
      materials.remove(index);
    }



    /* Throw an exception using the origin of this Asset, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (s32_t line, char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, char const* fmt, A ... args) {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, s32_t line, char const* fmt, A ... args) {
      if (!cond) asset_error(line, fmt, args...);
    }


    /* Throw an exception using the origin of this Asset */
    template <typename ... A> NORETURN void asset_error (char const* fmt, A ... args) const {
      m_asset_error(origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number */
    template <typename ... A> NORETURN void asset_error (s32_t line, char const* fmt, A ... args) const {
      m_asset_error(origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, char const* fmt, A ... args) const {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, s32_t line, char const* fmt, A ... args) const {
      if (!cond) asset_error(line, fmt, args...);
    }
  };

  
  using MaterialSetHandle = AssetHandle<MaterialSet>;
}

#endif