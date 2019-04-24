#ifndef RENDER_MESH_3D_H
#define RENDER_MESH_3D_H

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
  namespace Mesh3DAttribute {
    enum: u8_t {
      Position,
      Normal,
      UV,
      Color,

      Face,

      total_attribute_count,

      Invalid = (u8_t) -1
    };

    static constexpr char const* names [total_attribute_count] = {
      "Position",
      "Normal",
      "UV",
      "Color",

      "Face"
    };

    static constexpr char const* name (u8_t attribute) {
      if (attribute < total_attribute_count) return names[attribute];
      else return "Invalid";
    }

    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t attribute = 0; attribute < total_attribute_count; attribute ++) {
        if (str_cmp_caseless(name, names[attribute], max_length) == 0) return attribute;
      }

      return Invalid;
    }

    static constexpr bool validate (u8_t attribute) {
      return attribute < total_attribute_count;
    }
  };
  
  struct RenderMesh3D {
    using UpdateMask = Bitmask<8>;
  
    static constexpr u8_t bounds_flag = Mesh3DAttribute::total_attribute_count;
    static_assert(bounds_flag < UpdateMask::bit_count, "RenderMesh3D::UpdateMask is not large enough");

    char* origin;
    u32_t asset_id = 0;

    u32_t gl_vao;
    u32_t gl_vbos [Mesh3DAttribute::total_attribute_count];

    Array<Vector3f> positions;
    Array<Vector3f> normals;
    Array<Vector2f> uvs;
    Array<Vector3f> colors;

    Array<Vector3u> faces;

    UpdateMask needs_update = { };

    bool dynamic;

    MaterialConfig material_config;

    AABB3 bounds;



    /* Create a new uninitialized RenderMesh3D */
    RenderMesh3D () { }


    /* Create a new RenderMesh3D and initialize it with empty data segments */
    ENGINE_API RenderMesh3D (char const* in_origin, bool in_dynamic);


    /* Create a new RenderMesh3D and initialize all of its data by copying from buffers */
    ENGINE_API RenderMesh3D (
      char const* in_origin,

      bool in_dynamic,
      
      size_t vertex_count,
      Vector3f const* in_positions,
      Vector3f const* in_normals,
      Vector2f const* in_uvs,
      Vector3f const* in_colors,
      
      size_t face_count,
      Vector3u const* in_faces,

      MaterialConfig const& in_material_config = { }
    );

    /* Create a new RenderMesh3D and initialize its position and face data by copying from buffers */
    RenderMesh3D (
      char const* in_origin,

      bool in_dynamic,
      
      size_t vertex_count,
      Vector3f const* in_positions,
      Vector3f const* in_normals,
      
      size_t face_count,
      Vector3u const* in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh3D(
      in_origin,
      in_dynamic,
      vertex_count,
      in_positions,
      in_normals,
      NULL,
      NULL,
      face_count,
      in_faces,
      in_material_config
    )
    { }

    /* Create a new RenderMesh3D and initialize its position and face data by copying from buffers */
    RenderMesh3D (
      char const* in_origin,

      bool in_dynamic,
      
      size_t vertex_count,
      Vector3f const* in_positions,
      
      size_t face_count,
      Vector3u const* in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh3D(
      in_origin,
      in_dynamic,
      vertex_count,
      in_positions,
      NULL,
      NULL,
      NULL,
      face_count,
      in_faces,
      in_material_config
    )
    { }


    /* Create a new RenderMesh3D and initialize all of its data by copying from arrays */
    RenderMesh3D (
      char const* in_origin,

      bool in_dynamic,
      
      Array<Vector3f> const& in_positions,
      Array<Vector3f> const& in_normals,
      Array<Vector2f> const& in_uvs,
      Array<Vector3f> const& in_colors,
      
      Array<Vector3u> const& in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh3D(
      in_origin,
      in_dynamic,
      in_positions.count,
      in_positions.elements,
      in_normals.elements,
      in_uvs.elements,
      in_colors.elements,
      in_faces.count,
      in_faces.elements,
      in_material_config
    )
    { }

    /* Create a new RenderMesh3D and initialize its position and face data by copying from arrays */
    RenderMesh3D (
      char const* in_origin,

      bool in_dynamic,
      
      Array<Vector3f> const& in_positions,
      Array<Vector3f> const& in_normals,
    
      Array<Vector3u> const& in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh3D (
      in_origin,
      in_dynamic,
      in_positions.count,
      in_positions.elements,
      in_normals.elements,
      in_faces.count,
      in_faces.elements,
      in_material_config
    )
    { }

    /* Create a new RenderMesh3D and initialize its position and face data by copying from arrays */
    RenderMesh3D (
      char const* in_origin,

      bool in_dynamic,
      
      Array<Vector3f> const& in_positions,
    
      Array<Vector3u> const& in_faces,

      MaterialConfig const& in_material_config = { }
    )
    : RenderMesh3D (
      in_origin,
      in_dynamic,
      in_positions.count,
      in_positions.elements,
      NULL,
      in_faces.count,
      in_faces.elements,
      in_material_config
    )
    { }


    /* Create a new RenderMesh3D and initialize all of its data taking ownership of existing buffers */
    ENGINE_API static RenderMesh3D from_ex (
      char const* origin,

      bool dynamic,
      
      size_t vertex_count,
      Vector3f* positions,
      Vector3f* normals,
      Vector2f* uvs,
      Vector3f* colors,
      
      size_t face_count,
      Vector3u* faces,

      MaterialConfig const& material_config = { }
    );

    /* Create a new RenderMesh3D and initialize its position and face data taking ownership of existing buffers */
    static RenderMesh3D from_ex (
      char const* origin,

      bool dynamic,
      
      size_t vertex_count,
      Vector3f* positions,
      Vector3f* normals,
      
      size_t face_count,
      Vector3u* faces,

      MaterialConfig const& material_config = { }
    )
    { return from_ex (
      origin,
      dynamic,
      vertex_count,
      positions,
      normals,
      NULL,
      NULL,
      face_count,
      faces,
      material_config
    ); }

    /* Create a new RenderMesh3D and initialize its position and face data taking ownership of existing buffers */
    static RenderMesh3D from_ex (
      char const* origin,

      bool dynamic,
      
      size_t vertex_count,
      Vector3f* positions,
      
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
      NULL,
      face_count,
      faces,
      material_config
    ); }


    /* Create a new RenderMesh3D and initialize all of its data taking ownership of existing arrays */
    ENGINE_API static RenderMesh3D from_ex (
      char const* origin,

      bool dynamic,
      
      Array<Vector3f> const& positions,
      Array<Vector3f> const& normals,
      Array<Vector2f> const& uvs,
      Array<Vector3f> const& colors,
      
      Array<Vector3u> const& faces,

      MaterialConfig const& material_config = { }
    );

    /* Create a new RenderMesh3D and initialize its position and face data taking ownership of existing arrays */
    ENGINE_API static RenderMesh3D from_ex (
      char const* origin,

      bool dynamic,
      
      Array<Vector3f> const& positions,
      Array<Vector3f> const& normals,

      Array<Vector3u> const& faces,

      MaterialConfig const& material_config = { }
    );

    /* Create a new RenderMesh3D and initialize its position and face data taking ownership of existing arrays */
    ENGINE_API static RenderMesh3D from_ex (
      char const* origin,

      bool dynamic,
      
      Array<Vector3f> const& positions,

      Array<Vector3u> const& faces,

      MaterialConfig const& material_config = { }
    );
    

    /* Create a new RenderMesh3D from a JSONItem */
    ENGINE_API static RenderMesh3D from_json_item (char const* origin, JSONItem const& json);

    /* Create a new RenderMesh3D from JSON */
    static RenderMesh3D from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new RenderMesh3D from a source str */
    ENGINE_API static RenderMesh3D from_str (char const* origin, char const* source);

    /* Create a new RenderMesh3D from a source file */
    ENGINE_API static RenderMesh3D from_file (char const* origin);



    /* Recalculate the axis-aligned bounding box of a RenderMesh3D and overwrite its existing one */
    ENGINE_API void recalculate_bounds ();

    /* Get an up-to-date axis-aligned bounding box for a RenderMesh3D */
    ENGINE_API AABB3 const& get_aabb ();


    /* Reset all of a RenderMesh3D's arrays to 0-length but keep their capacity. Sets all flags of needs_update to true.
     * If material_config is in multi_material mode it will also be cleared but will stay in multi_material mode.
     * If material_config is not in multi_material mode it will be re-zero-initialized */
    ENGINE_API void clear ();


    /* Clean up a RenderMesh3D's heap allocations and delete its OpenGL data */
    ENGINE_API void destroy ();


    /* Calculate standard vertex normals for a mesh, overwriting existing normals */
    ENGINE_API void calculate_normals ();

    /* Calculate per-face vertex normals for a mesh, overwriting existing normals */
    ENGINE_API void calculate_face_normals ();


    /* Copy all of a RenderMesh3D's out of date data (indicated by needs_update bitmask) to OpenGL */
    ENGINE_API void update ();

    /* Update and bind a RenderMesh3D to the OpenGL context */
    ENGINE_API void use ();


    /* Draw a RenderMesh3D into OpenGL's active framebuffer using the active ShaderProgram */
    ENGINE_API void draw_with_active_shader ();

    /* Draw a subsection of a RenderMesh3D into OpenGL's active framebuffer using the active ShaderProgram.
     * Panics if the RenderMesh3D is not a multi-material mesh,
     * or if the given section index is out of bounds */
    ENGINE_API void draw_section_with_active_shader (size_t section_index);

    /* Draw a RenderMesh3D into OpenGL's active framebuffer using a designated Material */
    ENGINE_API void draw_with_material (MaterialHandle const& material);

    /* Draw a subsection of a RenderMesh3D into OpenGL's active framebuffer using a designated Material.
     * Panics if the RenderMesh3D is not a multi-material mesh,
     * or if the given section index is out of bounds */
    ENGINE_API void draw_section_with_material (size_t section_index, MaterialHandle const& material);

    /* Draw a RenderMesh3D into OpenGL's active framebuffer using a designated MaterialSet.
     * If the mesh is multi-material, each subsection selects the appropriate material from the set.
     * If the mesh is not multi-material, the whole mesh is drawn with the mesh's single material index.
     * Panics if any material index is out of bounds for the MaterialSet */
    ENGINE_API void draw_with_material_set (MaterialSetHandle const& material_set);



    /* Enable uv coordinates for a RenderMesh3D and initialize the data by copying from a buffer.
     * Assumes there is at least mesh.positions.count Vector2fs in the buffer.
     * Panics if uvs are already enabled */
    ENGINE_API void enable_uvs (Vector2f const* data);

    /* Enable uv coordinates for a RenderMesh3D and initialize the data by copying from an array.
     * Panics if there are not enough elements in the array, or if uvs are already enabled */
    ENGINE_API void enable_uvs (Array<Vector2f> const& arr);

    /* Enable uv coordinates for a RenderMesh3D and initialize the data by taking ownership of a buffer.
     * Assumes there is at least mesh.positions.count Vector2fs in the buffer.
     * Panics if uvs are already enabled */
    ENGINE_API void enable_uvs_ex (Vector2f* data);

    /* Enable uv coordinates for a RenderMesh3D and initialize the data by taking ownership of an array.
     * Panics if there are not enough elements in the array, or if uvs are already enabled */
    ENGINE_API void enable_uvs_ex (Array<Vector2f> const& arr);

    /* Disable uv coordinates for a RenderMesh3D and free its data.
     * Does nothing if no uvs are enabled */
    ENGINE_API void disable_uvs ();


    /* Enable colors for a RenderMesh3D and initialize the data by copying from a buffer.
     * Assumes there is at least mesh.positions.count Vector3fs in the buffer.
     * Panics if colors are already enabled */
    ENGINE_API void enable_colors (Vector3f const* data);

    /* Enable colors for a RenderMesh3D and initialize the data by copying from an array.
     * Panics if there are not enough elements in the array, or if colors are already enabled */
    ENGINE_API void enable_colors (Array<Vector3f> const& arr);

    /* Enable colors for a RenderMesh3D and initialize the data by taking ownership of a buffer.
     * Assumes there is at least mesh.positions.count Vector3fs in the buffer.
     * Panics if colors are already enabled */
    ENGINE_API void enable_colors_ex (Vector3f* data);

    /* Enable colors for a RenderMesh3D and initialize the data by taking ownership of an array.
     * Panics if there are not enough elements in the array, or if colors are already enabled */
    ENGINE_API void enable_colors_ex (Array<Vector3f> const& arr);

    /* Disable colors for a RenderMesh3D and free its data.
     * Does nothing if no colors are enabled */
    ENGINE_API void disable_colors ();



    /* Get pointers to the various attributes of a vertex in a RenderMesh3D.
     * Returns a quad_t<Vector3f* (Position), Vector3f* (Normal), Vector2f* (UV), Vector3f* (Color)>.
     * If a particular attribute is disabled, the pointer will be NULL.
     * Panics if the given index is out of range */
    ENGINE_API quad_t<Vector3f*, Vector3f*, Vector2f*, Vector3f*> get_vertex (size_t index) const;


    /* Set the various attributes of a vertex in a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector3f const& position, Vector3f const& normal);

    /* Set the various attributes of a vertex in a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector3f const& position, Vector3f const& normal, Vector2f const& uv);

    /* Set the various attributes of a vertex in a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector3f const& position, Vector3f const& normal, Vector3f const& color);

    /* Set the various attributes of a vertex in a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if the given index is out of range, or if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void set_vertex (size_t index, Vector3f const& position, Vector3f const& normal, Vector2f const& uv, Vector3f const& color);


    /* Append the various attributes of a vertex to the ends of a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector3f const& position, Vector3f const& normal);

    /* Append the various attributes of a vertex to the ends of a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector3f const& position, Vector3f const& normal, Vector2f const& uv);

    /* Append the various attributes of a vertex to the ends of a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector3f const& position, Vector3f const& normal, Vector3f const& color);

    /* Append the various attributes of a vertex to the ends of a RenderMesh3D.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void append_vertex (Vector3f const& position, Vector3f const& normal, Vector2f const& uv, Vector3f const& color);


    /* Insert the various attributes of a vertex in a RenderMesh3D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector3f const& position, Vector3f const& normal);

    /* Insert the various attributes of a vertex in a RenderMesh3D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector3f const& position, Vector3f const& normal, Vector2f const& uv);

    /* Insert the various attributes of a vertex in a RenderMesh3D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector3f const& position, Vector3f const& normal, Vector3f const& color);

    /* Insert the various attributes of a vertex in a RenderMesh3D at a given index.
     * If a particular attribute is disabled, the overload of this function not including that attribute may be used.
     * Panics if unexpected attributes were supplied, or if attributes were expected but not supplied */
    ENGINE_API void insert_vertex (size_t index, Vector3f const& position, Vector3f const& normal, Vector2f const& uv, Vector3f const& color);


    /* Remove the various attributes of a vertex from a given index in a RenderMesh3D.
     * Does nothing to the vertex data if the index is out of range, but will still change the needs_update mask */
    ENGINE_API void remove_vertex (size_t index);


    /* Mark a RenderMesh3D to have its vertex attributes updated before use */
    void dirty_vertices () {
      using namespace Mesh3DAttribute;
      needs_update.set_multiple(Position, bounds_flag, Normal);
      if (uvs.elements != NULL) needs_update.set(UV);
      if (colors.elements != NULL) needs_update.set(Color);
    }

    /* Mark a RenderMesh3D to have its vertex positions and bounds updated before use */
    void dirty_positions () {
      needs_update.set_multiple(Mesh3DAttribute::Position, bounds_flag);
    }
    
    /* Mark a RenderMesh3D to have its normals updated before use */
    void dirty_normals () {
      needs_update.set(Mesh3DAttribute::Normal);
    }

    /* Mark a RenderMesh3D to have its uvs updated before use */
    void dirty_uvs () {
      if (uvs.elements != NULL) needs_update.set(Mesh3DAttribute::UV);
    }

    /* Mark a RenderMesh3D to have its colors updated before use */
    void dirty_colors () {
      if (colors.elements != NULL) needs_update.set(Mesh3DAttribute::Color);
    }



    
    /* Get the face of a RenderMesh3D at the given index.
     * Panics if the index is out of range */
    ENGINE_API Vector3u& get_face (size_t index) const;


    /* Set the face of a RenderMesh3D at the given index.
     * Panics if the index is out of range */
    ENGINE_API void set_face (size_t index, Vector3u const& face);


    /* Append a new face to the end of a RenderMesh3D */
    ENGINE_API void append_face (Vector3u const& face);

    /* Insert a new face at the given index in a RenderMesh3D */
    ENGINE_API void append_face (size_t index, Vector3u const& face);

    /* Remove a face at the given index in a RenderMesh3D */
    ENGINE_API void remove_face (size_t index);

    /* Mark a RenderMesh3D to have its faces updated before use */
    void dirty_faces () {
      needs_update.set(Mesh3DAttribute::Face);
    }



    /* Set a RenderMesh3D to single-material mode */
    ENGINE_API void set_material (size_t material_index, bool cast_shadow = false);

    /* Set a RenderMesh3D to mutli-material mode by copying values from an array of MaterialInfo */
    ENGINE_API void set_multi_material (Array<MaterialInfo> const& materials);

    /* Set a RenderMesh3D to mutli-material mode by taking ownership of an array of MaterialInfo */
    ENGINE_API void set_multi_material_ex (Array<MaterialInfo> const& materials);


    /* Get a multi-material config element at the given index of a RenderMesh3D.
     * Panics if the mesh is not multi-material, or of the given index is out of range */
    ENGINE_API MaterialInfo& get_material_element (size_t index) const;

    /* Set a multi-material config element at the given index of a RenderMesh3D.
     * Panics if the mesh is not multi-material, or if the given index is out of range */
    ENGINE_API void set_material_element (size_t index, MaterialInfo const& value);

    /* Append a new MaterialInfo element at the end of a multi-material config of a RenderMesh3D
     * Panics if the mesh is not multi-material */
    ENGINE_API void append_material_element (MaterialInfo const& value);

    /* Insert a new MaterialInfo element at the given index in a multi-material config of a RenderMesh3D
     * Panics if the mesh is not multi-material */
    ENGINE_API void insert_material_element (size_t index, MaterialInfo const& value);

    /* Remove a MaterialInfo element at the given index in a multi-material config of a RenderMesh3D
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

  
  using RenderMesh3DHandle = AssetHandle<RenderMesh3D>;
}

#endif