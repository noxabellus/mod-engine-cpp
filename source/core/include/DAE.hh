#ifndef DAE_H
#define DAE_H

#include "XML.hh"
#include "math/lib.hh"
#include "graphics/lib.hh"



namespace mod {
  struct DAE;

  struct DAESource {
    String id;
    bool float_array;
    union {
      Array<f64_t> floats;
      Array< pair_t<char*, size_t> > names;
    };

    DAESource () { }
    DAESource (String in_id, Array<f64_t> in_floats)
    : id(in_id)
    , float_array(true)
    , floats(in_floats)
    { }
    DAESource (String in_id, Array< pair_t<char*, size_t> > in_names)
    : id(in_id)
    , float_array(false)
    , names(in_names)
    { }
  };

  struct DAEAccessor {
    String id;
    String source_id;
    size_t count;
    size_t offset;
    size_t stride;
    size_t elements;
  };

  struct DAEInput {
    String semantic;
    String source_id;
    size_t offset;
    size_t set;
  };

  struct DAETriangles {
    XMLItem* origin;
    size_t advance;
    Array<DAEInput> inputs;
    Array<u32_t> indices;
  };

  struct DAEVertexBinding {
    String id;
    String source_id;
  };

  struct DAEJointData {
    XMLItem* origin;
    Array<DAEInput> inputs;

    DAEJointData () { }
    DAEJointData (XMLItem* in_origin)
    : origin(in_origin)
    { }
  };

  struct DAEWeightData {
    XMLItem* origin;
    Array<DAEInput> inputs;
    size_t count;
    Array<u32_t> vcount;
    Array<u32_t> indices;
    
    DAEWeightData () { }
    DAEWeightData (XMLItem* in_origin)
    : origin(in_origin)
    { }
  };

  struct DAEIJoint {
    static constexpr size_t max_vcount = 16;

    u32_t count;
    u32_t joints [max_vcount + 2];
    u32_t weights [max_vcount + 2];

    DAEIJoint () { }

    DAEIJoint (u32_t in_count)
    : count(in_count) { }
  };
  

  struct DAEIVertex {
    DAETriangles& triangles;

    bool set;

    size_t index;

    s64_t duplicate;

    u32_t position;

    s64_t normal;
    s64_t uv;
    s64_t color;

    DAEIVertex (DAETriangles& in_triangles, size_t in_index)
    : triangles(in_triangles)
    , set(false)
    , index(in_index)
    , duplicate(-1)
    , position(0)
    , normal(-1)
    , uv(-1)
    , color(-1)
    { }

    ENGINE_API void set_attributes (s64_t in_normal, s64_t in_uv, s64_t in_color);

    ENGINE_API bool has_same_attributes (s64_t test_normal, s64_t test_uv, s64_t test_color) const;
  };


  struct DAEBoneBinding {
    u32_t origin_index;
    String id = { };
    String sid = { };

    String name;
    s32_t parent_index;
    Transform3D base_transform;
    // Matrix4 inverse_bind_matrix;
  };


  struct DAEIBone {
    DAEIBone* parent = NULL;

    u32_t origin_index;
    String id = { };
    String sid = { };

    String name = { };
    Array<DAEIBone> children;
    Matrix4 base_matrix = { };
    Matrix4 bind_matrix = { };
    Matrix4 inverse_bind_matrix = { };


    DAEIBone () { }
    DAEIBone (size_t children_alloc)
    : children(children_alloc)
    { }

    DAEIBone& operator = (DAEIBone const& bone) {
      parent = bone.parent;

      origin_index = bone.origin_index;
      id = bone.id;
      sid = bone.sid;

      name = bone.name;
      // children.destroy();
      children = bone.children;
      for (auto [ i, child ] : children) child.parent = this;
      base_matrix = bone.base_matrix;
      bind_matrix = bone.bind_matrix;
      inverse_bind_matrix = bone.inverse_bind_matrix;

      return *this;
    }


    ENGINE_API void add_child (DAEIBone const& bone);

    ENGINE_API void remove_child (DAEIBone& bone);

    ENGINE_API void calculate_bind (Matrix4 const& transform);

    ENGINE_API Transform3D final_transform () const;

    ENGINE_API static DAEIBone process (DAE const& dae, XMLItem& joint);

    ENGINE_API bool filter (std::function<bool (DAEIBone const&)> filter);

    ENGINE_API void collapse (Array<DAEBoneBinding>& out_bones, s32_t parent_index = -1) const;

    ENGINE_API bool traverse_cond (std::function<bool (DAEIBone const&)> callback) const;

    ENGINE_API void traverse (std::function<void (DAEIBone const&)> callback) const;

    ENGINE_API void destroy ();
  };
  

  struct DAEAnimSampler {
    XMLItem* origin;
    String id;
    Array<DAEInput> inputs;
  };

  struct DAEAnimChannel {
    String id;
    String target_id;
    String sampler_id;
  };

  struct DAEAnimClip {
    String name;
    f32_t start;
    f32_t end;
    Array<String> channel_ids;
  };

  struct DAEIKeyframe;
  struct DAEBoneBindingList;
  
  struct DAEIChannel {
    u32_t target_index;
    Matrix4 base_matrix;
    Matrix4 bind_matrix;
    bool filtered = false;

    DAEIChannel (u32_t in_target_index, Matrix4 const& in_base_matrix)
    : target_index(in_target_index)
    , base_matrix(in_base_matrix)
    { }

    Transform3D final_transform (DAEIKeyframe const& owner, DAEBoneBindingList const& binding_list) const;
  };

  struct DAEIKeyframe {
    DAE const* dae;
    f32_t time;
    Array<DAEIChannel> channels;

    ENGINE_API DAEIChannel* get_channel (u32_t target_index) const;
  };

  struct DAEBoneBindingList {
    Array<DAEBoneBinding> bindings;

    ArrayIterator<DAEBoneBinding> begin () const { return bindings.begin(); }
    ArrayIterator<DAEBoneBinding> end () const { return bindings.end(); }

    DAEBoneBinding& operator [] (size_t index) const { return bindings[index]; }

    void destroy () { bindings.destroy(); }

    ENGINE_API bool traverse_cond (std::function<bool (size_t, DAEBoneBinding const&)> callback, size_t index = 0) const;

    ENGINE_API void traverse (std::function<void (size_t, DAEBoneBinding const&)> callback, size_t index = 0) const;

    ENGINE_API void collapse (Array<Bone>& out_array) const;
  };

  struct DAEIAnimation {
    String name;
    DAE const* dae;
    f32_t length;
    Array<DAEIKeyframe> keyframes;

    ENGINE_API DAEIKeyframe& get_keyframe_for_time (f32_t time);

    ENGINE_API static DAEIAnimation process (DAE const* dae, DAEIBone const& root_ibone, DAEAnimClip& clip);

    ENGINE_API void calculate_bind (DAEIBone const& root_ibone, Matrix4 const& transform) const;

    ENGINE_API void collapse (DAEBoneBindingList const& binding_list, Array<SkeletalKeyframe>& out_array) const;

    ENGINE_API void filter (DAEBoneBindingList const& binding_list) const;

    ENGINE_API void destroy ();
  };


  struct DAE {
    ENGINE_API static std::function<bool (DAEIBone const&)> std_bone_filter;


    XML xml;

    XMLItem& root;

    XMLItem* mesh;
    XMLItem* skin;

    Array<DAESource> sources;
    Array<DAEAccessor> accessors;
    Array<DAEVertexBinding> vertex_bindings;
    Array<DAETriangles> triangles_list;
    DAEBoneBindingList bone_bindings_list;
    DAEJointData joint_data;
    DAEWeightData weight_data;

    Array<DAEAnimSampler> anim_samplers;
    Array<DAEAnimChannel> anim_channels;

    Array<DAEAnimClip> anim_clips;

    Array<DAEIAnimation> animations;

    Matrix4 transform;



    /* Create a new DAE from an XML, taking ownership of the XML */
    ENGINE_API DAE (XML const& in_xml, Matrix4 const& in_transform = Constants::Matrix4::identity, bool apply_bone_filter = true, std::function<bool (DAEIBone const&)> bone_filter = std_bone_filter);

    ENGINE_API void destroy () ;


    static DAE from_str (char const* origin, char const* source, Matrix4 const& transform = Constants::Matrix4::identity, bool apply_bone_filter = true, std::function<bool (DAEIBone const&)> bone_filter = std_bone_filter) {
      return { XML::from_str(origin, source), transform, apply_bone_filter, bone_filter };
    }

    static DAE from_str_ex (char const* origin, char* source, Matrix4 const& transform = Constants::Matrix4::identity, bool apply_bone_filter = true, std::function<bool (DAEIBone const&)> bone_filter = std_bone_filter) {
      return { XML::from_str_ex(origin, source), transform, apply_bone_filter, bone_filter };
    }

    static DAE from_file (char const* origin, Matrix4 const& transform = Constants::Matrix4::identity, bool apply_bone_filter = true, std::function<bool (DAEIBone const&)> bone_filter = std_bone_filter) {
      return { XML::from_file(origin), transform, apply_bone_filter, bone_filter };
    }
    

    ENGINE_API DAEAccessor& get_accessor (String const& id) const;

    ENGINE_API DAESource& get_source (String const& id) const;

    ENGINE_API static DAEInput& get_poly_input (DAETriangles const& triangles, char const* semantic);

    ENGINE_API static DAEInput* get_poly_input_pointer (DAETriangles const& triangles, char const* semantic);

    ENGINE_API DAEInput& get_wd_input (char const* semantic) const;

    ENGINE_API DAEInput* get_wd_input_pointer (char const* semantic) const;

    ENGINE_API DAEInput& get_joint_input (char const* semantic) const;

    ENGINE_API DAEInput* get_joint_input_pointer (char const* semantic) const;

    ENGINE_API s64_t get_bone_index (String& id) const;

    ENGINE_API static s64_t get_origin_bone_index_from_node_id (DAEIBone const& root_ibone, String& id);

    ENGINE_API s64_t get_filtered_bone_index_from_node_id (String& id) const;

    ENGINE_API s64_t get_filtered_bone_index (u32_t origin_index) const;
    
    ENGINE_API DAEAnimClip& get_anim_clip (char const* name) const;

    ENGINE_API DAEAnimChannel& get_anim_channel (String& id) const;

    ENGINE_API DAEAnimSampler& get_anim_sampler (String& id) const;

    ENGINE_API static DAEInput& get_sampler_input (DAEAnimSampler const& sampler, char const* semantic);

    ENGINE_API DAEIAnimation& get_animation (char const* name) const;







    ENGINE_API RenderMesh3D load_mesh () const;
      



    ENGINE_API Skeleton load_skeleton () const;


    




    ENGINE_API SkeletalAnimation load_animation (char const* name = NULL) const;







  private:
    ENGINE_API DAEVertexBinding& get_vertex_binding (String const& id) const;

    ENGINE_API Array<DAEInput> gather_inputs (XMLItem& origin) const;

    ENGINE_API void gather_base_data (XMLItem& section);
  };
}

#endif