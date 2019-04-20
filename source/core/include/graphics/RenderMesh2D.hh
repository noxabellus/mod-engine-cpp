#ifndef RENDER_MESH_2D_H
#define RENDER_MESH_2D_H

#include "../cstd.hh"
#include "../util.hh"
#include "../Bitmask.hh"
#include "../Array.hh"
#include "../JSON.hh"
#include "../Exception.hh"

#include "../math/lib.hh"

#include "../AssetHandle.hh"

#include "Material.hh"
#include "MaterialSet.hh"

#include "MaterialConfig.hh"



namespace mod {
  struct RenderMesh2D {
    using UpdateMask = Bitmask<8>;
  

    char* origin;
    u32_t asset_id = 0;

    u32_t gl_vao;
    u32_t gl_vbos [4];

    Array<Vector2f> positions;
    Array<Vector2f> uvs;
    Array<Vector3f> colors;

    Array<Vector3u> faces;

    UpdateMask needs_update = { };

    bool dynamic;

    MaterialConfig material_config;



    /* Create a new uninitialized RenderMesh2D */
    RenderMesh2D () { }


    /* Create a new RenderMesh2D and initialize it with empty data segments */
    ENGINE_API RenderMesh2D (char const* in_origin, bool in_dynamic);


    /* Create a new RenderMesh2D and initialize all of its data by copying from buffers */
    ENGINE_API RenderMesh2D (
      char const* in_origin,

      bool in_dynamic,
      
      size_t vertex_count,
      Vector2f const* in_positions,
      Vector2f const* in_uvs,
      Vector3f const* in_colors,
      
      size_t face_count,
      Vector3u const* in_faces,

      MaterialConfig const& in_material_config = { }
    );

    /* Create a new RenderMesh2D and initialize its position and face data by copying from buffers */
    RenderMesh2D (
      char const* in_origin,

      bool in_dynamic,
      
      size_t vertex_count,
      Vector2f const* in_positions,
      
      size_t face_count,
      Vector3u const* in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh2D(
      in_origin,
      in_dynamic,
      vertex_count,
      in_positions,
      NULL,
      NULL,
      face_count,
      in_faces,
      in_material_config
    )
    { }


    /* Create a new RenderMesh2D and initialize all of its data by copying from arrays */
    RenderMesh2D (
      char const* in_origin,

      bool in_dynamic,
      
      Array<Vector2f> const& in_positions,
      Array<Vector2f> const& in_uvs,
      Array<Vector3f> const& in_colors,
      
      Array<Vector3u> const& in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh2D(
      in_origin,
      in_dynamic,
      in_positions.count,
      in_positions.elements,
      in_uvs.elements,
      in_colors.elements,
      in_faces.count,
      in_faces.elements,
      in_material_config
    )
    { }

    /* Create a new RenderMesh2D and initialize its position and face data by copying from arrays */
    RenderMesh2D (
      char const* in_origin,

      bool in_dynamic,
      
      Array<Vector2f> const& in_positions,
    
      Array<Vector3u> const& in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh2D (
      in_origin,
      in_dynamic,
      in_positions.count,
      in_positions.elements,
      in_faces.count,
      in_faces.elements,
      in_material_config
    )
    { }


    /* Create a new RenderMesh2D and initialize all of its data taking ownership of existing buffers */
    ENGINE_API static RenderMesh2D from_ex (
      char const* origin,

      bool dynamic,
      
      size_t vertex_count,
      Vector2f* positions,
      Vector2f* uvs,
      Vector3f* colors,
      
      size_t face_count,
      Vector3u* faces,

      MaterialConfig const& material_config = { }
    );

    /* Create a new RenderMesh2D and initialize its position and face data taking ownership of existing buffers */
    static RenderMesh2D from_ex (
      char const* origin,

      bool dynamic,
      
      size_t vertex_count,
      Vector2f* positions,
      
      size_t face_count,
      Vector3u* faces,

      MaterialConfig const& material_config = { }
    )
    { return from_ex (
      origin,
      dynamic,
      vertex_count,
      positions,
      NULL,
      NULL,
      face_count,
      faces,
      material_config
    ); }


    /* Create a new RenderMesh2D and initialize all of its data taking ownership of existing arrays */
    ENGINE_API static RenderMesh2D from_ex (
      char const* origin,

      bool dynamic,
      
      Array<Vector2f> const& positions,
      Array<Vector2f> const& uvs,
      Array<Vector3f> const& colors,
      
      Array<Vector3u> const& faces,

      MaterialConfig const& material_config = { }
    );

    /* Create a new RenderMesh2D and initialize its position and face data taking ownership of existing arrays */
    ENGINE_API static RenderMesh2D from_ex (
      char const* origin,

      bool dynamic,
      
      Array<Vector2f> const& positions,

      Array<Vector3u> const& faces,

      MaterialConfig const& material_config = { }
    );



    /* Create a new RenderMesh2D from a JSONItem */
    ENGINE_API static RenderMesh2D from_json_item (char const* origin, JSONItem const& json);

    /* Create a new RenderMesh2D from JSON */
    static RenderMesh2D from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new RenderMesh2D from a source str */
    ENGINE_API static RenderMesh2D from_str (char const* origin, char const* source);

    /* Create a new RenderMesh2D from a source file */
    ENGINE_API static RenderMesh2D from_file (char const* origin);



    /* Reset all of a RenderMesh2D's arrays to 0-length but keep their capacity. Sets all flags of needs_update to true.
     * If material_config is in multi_material mode it will also be cleared but will stay in multi_material mode.
     * If material_config is not in multi_material mode it will be re-zero-initialized */
    ENGINE_API void clear ();


    /* Clean up a RenderMesh2D's heap allocations and delete its OpenGL data */
    ENGINE_API void destroy ();



    /* Copy all of a RenderMesh2D's out of date data (indicated by needs_update bitmask) to OpenGL */
    ENGINE_API void update ();

    /* Update and bind a RenderMesh2D to the OpenGL context */
    ENGINE_API void use ();


    /* Draw a RenderMesh2D into OpenGL's active framebuffer using the active ShaderProgram */
    ENGINE_API void draw_with_active_shader ();

    /* Draw a subsection of a RenderMesh2D into OpenGL's active framebuffer using the active ShaderProgram.
     * Panics if the RenderMesh2D is not a multi-material mesh,
     * or if the given section index is out of bounds */
    ENGINE_API void draw_section_with_active_shader (size_t section_index);

    /* Draw a RenderMesh2D into OpenGL's active framebuffer using a designated Material */
    ENGINE_API void draw_with_material (MaterialHandle const& material);

    /* Draw a subsection of a RenderMesh2D into OpenGL's active framebuffer using a designated Material.
     * Panics if the RenderMesh2D is not a multi-material mesh,
     * or if the given section index is out of bounds */
    ENGINE_API void draw_section_with_material (size_t section_index, MaterialHandle const& material);

    /* Draw a RenderMesh2D into OpenGL's active framebuffer using a designated MaterialSet.
     * If the mesh is multi-material, each subsection selects the appropriate material from the set.
     * If the mesh is not multi-material, the whole mesh is drawn with the mesh's single material index.
     * Panics if any material index is out of bounds for the MaterialSet */
    ENGINE_API void draw_with_material_set (MaterialSetHandle const& material_set);



    /* Enable uv coordinates for a RenderMesh2D and initialize the data by copying from a buffer.
     * Assumes there is at least mesh.positions.count Vector2fs in the buffer.
     * Panics if uvs are already enabled */
    ENGINE_API void enable_uvs (Vector2f const* data);

    /* Enable uv coordinates for a RenderMesh2D and initialize the data by copying from an array.
     * Panics if there are not enough elements in the array, or if uvs are already enabled */
    ENGINE_API void enable_uvs (Array<Vector2f> const& arr);

    /* Enable uv coordinates for a RenderMesh2D and initialize the data by taking ownership of a buffer.
     * Assumes there is at least mesh.positions.count Vector2fs in the buffer.
     * Panics if uvs are already enabled */
    ENGINE_API void enable_uvs_ex (Vector2f* data);

    /* Enable uv coordinates for a RenderMesh2D and initialize the data by taking ownership of an array.
     * Panics if there are not enough elements in the array, or if uvs are already enabled */
    ENGINE_API void enable_uvs_ex (Array<Vector2f> const& arr);

    /* Disable uv coordinates for a RenderMesh2D and free its data.
     * Does nothing if no uvs are enabled */
    ENGINE_API void disable_uvs ();


    /* Enable colors for a RenderMesh2D and initialize the data by copying from a buffer.
     * Assumes there is at least mesh.positions.count Vector3fs in the buffer.
     * Panics if colors are already enabled */
    ENGINE_API void enable_colors (Vector3f const* data);

    /* Enable colors for a RenderMesh2D and initialize the data by copying from an array.
     * Panics if there are not enough elements in the array, or if colors are already enabled */
    ENGINE_API void enable_colors (Array<Vector3f> const& arr);

    /* Enable colors for a RenderMesh2D and initialize the data by taking ownership of a buffer.
     * Assumes there is at least mesh.positions.count Vector3fs in the buffer.
     * Panics if colors are already enabled */
    ENGINE_API void enable_colors_ex (Vector3f* data);

    /* Enable colors for a RenderMesh2D and initialize the data by taking ownership of an array.
     * Panics if there are not enough elements in the array, or if colors are already enabled */
    ENGINE_API void enable_colors_ex (Array<Vector3f> const& arr);

    /* Disable colors for a RenderMesh2D and free its data.
     * Does nothing if no colors are enabled */
    ENGINE_API void disable_colors ();



    /* Get pointers to the various attributes of a vertex in a RenderMesh2D.
     * Returns a tri_t<Vector2f* (Position), Vector2f* (UV), Vector3f* (Color)>.
     * If a particular attribute is disabled, the pointer will be NULL.
     * Panics if the given index is out of range */
    ENGINE_API tri_t<Vector2f*, Vector2f*, Vector3f*> get_vertex (size_t index) const;


    /* Set the various attributes of a vertex in a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector2f const& position);

    /* Set the various attributes of a vertex in a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector2f const& position, Vector2f const& uv);

    /* Set the various attributes of a vertex in a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector2f const& position, Vector3f const& color);

    /* Set the various attributes of a vertex in a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector2f const& position, Vector2f const& uv, Vector3f const& color);


    /* Append the various attributes of a vertex to the ends of a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector2f const& position);

    /* Append the various attributes of a vertex to the ends of a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector2f const& position, Vector2f const& uv);

    /* Append the various attributes of a vertex to the ends of a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector2f const& position, Vector3f const& color);

    /* Append the various attributes of a vertex to the ends of a RenderMesh2D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector2f const& position, Vector2f const& uv, Vector3f const& color);


    /* Insert the various attributes of a vertex in a RenderMesh2D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector2f const& position);

    /* Insert the various attributes of a vertex in a RenderMesh2D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector2f const& position, Vector2f const& uv);

    /* Insert the various attributes of a vertex in a RenderMesh2D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector2f const& position, Vector3f const& color);

    /* Insert the various attributes of a vertex in a RenderMesh2D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector2f const& position, Vector2f const& uv, Vector3f const& color);


    /* Remove the various attributes of a vertex from a given index in a RenderMesh2D.
     * Does nothing to the vertex data if the index is out of range, but will still change the needs_update mask */
    ENGINE_API void remove_vertex (size_t index);


    
    /* Get the face of a RenderMesh2D at the given index.
     * Panics if the index is out of range */
    ENGINE_API Vector3u& get_face (size_t index) const;


    /* Set the face of a RenderMesh2D at the given index.
     * Panics if the index is out of range */
    ENGINE_API void set_face (size_t index, Vector3u const& face);


    /* Append a new face to the end of a RenderMesh2D */
    ENGINE_API void append_face (Vector3u const& face);

    /* Insert a new face at the given index in a RenderMesh2D */
    ENGINE_API void append_face (size_t index, Vector3u const& face);

    /* Remove a face at the given index in a RenderMesh2D */
    ENGINE_API void remove_face (size_t index);



    /* Set a RenderMesh2D to single-material mode */
    ENGINE_API void set_material (size_t material_index, bool cast_shadow = false);

    /* Set a RenderMesh2D to mutli-material mode by copying values from an array of MaterialInfo */
    ENGINE_API void set_multi_material (Array<MaterialInfo> const& materials);

    /* Set a RenderMesh2D to mutli-material mode by taking ownership of an array of MaterialInfo */
    ENGINE_API void set_multi_material_ex (Array<MaterialInfo> const& materials);


    /* Get a multi-material config element at the given index of a RenderMesh2D.
     * Panics if the mesh is not multi-material, or of the given index is out of range */
    ENGINE_API MaterialInfo& get_material_element (size_t index) const;

    /* Set a multi-material config element at the given index of a RenderMesh2D.
     * Panics if the mesh is not multi-material, or if the given index is out of range */
    ENGINE_API void set_material_element (size_t index, MaterialInfo const& value);

    /* Append a new MaterialInfo element at the end of a multi-material config of a RenderMesh2D
     * Panics if the mesh is not multi-material */
    ENGINE_API void append_material_element (MaterialInfo const& value);

    /* Insert a new MaterialInfo element at the given index in a multi-material config of a RenderMesh2D
     * Panics if the mesh is not multi-material */
    ENGINE_API void insert_material_element (size_t index, MaterialInfo const& value);

    /* Remove a MaterialInfo element at the given index in a multi-material config of a RenderMesh2D
     * Panics if the mesh is not multi-material */
    ENGINE_API void remove_material_element (size_t index);





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

  
  using RenderMesh2DHandle = AssetHandle<RenderMesh2D>;
}

#endif