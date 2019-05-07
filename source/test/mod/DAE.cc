
#include "../main.hh"
namespace mod {
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
    , normal(0)
    , uv(0)
    , color(0)
    { }

    void set_attributes (s64_t in_normal, s64_t in_uv, s64_t in_color) {
      normal = in_normal;
      uv = in_uv;
      color = in_color;
      
      set = true;
    }

    bool has_same_attributes (s64_t test_normal, s64_t test_uv, s64_t test_color) const {
      return normal == test_normal
          && uv == test_uv
          && color == test_color;
    }
  };


  struct DAEIBone {
    DAEIBone* parent = NULL;

    String name = { };
    Array<DAEIBone> children = { 256 };
    Matrix4 base_matrix = { };
    Matrix4 bind_matrix = { };
    Matrix4 inverse_bind_matrix = { };


    DAEIBone () = default;


    void add_child (DAEIBone const& bone) {
      size_t bone_index = children.count;
      children.append(bone);
      DAEIBone& new_bone = children[bone_index];
      new_bone.parent = this;
      for (auto [ i, child ] : new_bone.children) child.parent = &new_bone; 
    }

    void remove_child (DAEIBone& bone) {
      children.remove(children.get_index(bone));
    }

    void calculate_bind (Matrix4 const& transform) {
      if (parent == NULL) bind_matrix = transform * base_matrix;
      else bind_matrix = parent->bind_matrix * base_matrix;

      inverse_bind_matrix = bind_matrix.inverse();

      for (auto [ i, child ] : children) child.calculate_bind(transform);
    }

    Transform3D final_transform () const {
      if (parent != NULL) return (parent->inverse_bind_matrix * bind_matrix).decompose();
      else return bind_matrix.decompose();
    }

    static DAEIBone traverse (XMLItem& joint) {
      DAEIBone bone;

      bone.name = joint.get_attribute("name").value.clone();

      Matrix4 matrix;

      XMLItem& matrix_item = joint.first_named("matrix");
      String& matrix_text = matrix_item.get_text();

      char* base = matrix_text.value;
      char* end = NULL;
      for (size_t j = 0; j < 16; j ++) {
        matrix[j] = strtof(base, &end);
        matrix_item.asset_assert(end != NULL && base != end, "Not enough elements in matrix");
        base = end;
      }
      
      bone.base_matrix = matrix.transpose();

      for (size_t i = 0; i < joint.count_of_name("node"); i ++) {
        XMLItem& child_joint = joint.nth_named(i, "node");
        XMLAttribute* type_attribute = child_joint.get_attribute_pointer("type");

        if (type_attribute != NULL && type_attribute->value == "JOINT") {
          bone.add_child(traverse(child_joint));
        }
      }

      return bone;
    }

    bool strip_ik (std::function<bool (DAEIBone const&)> filter) {
      if (parent != NULL) {
        if (filter(*this)) {
          for (auto [ i, child ] : children) parent->add_child(child);
          parent->remove_child(*this);
          return true;
        } else {
          for (auto [ i, child ] : children) if (child.strip_ik(filter)) return true;
        }
      } else {
        if (children.count > 1) {
          for (auto [ i, child ] : children) if (child.strip_ik(filter)) return true;
        } else if (filter(*this)) {
          DAEIBone child = children[0];
          name.destroy();
          name = child.name;
          children.destroy();
          children = child.children;
          for (auto [ i, new_child ] : children) new_child.parent = this;
          bind_matrix = child.bind_matrix;
          inverse_bind_matrix = child.inverse_bind_matrix;
        }
      }

      return false;
    }

    void collapse (Array<Bone>& out_bones, s32_t parent_index = -1) const {
      size_t index = out_bones.count;

      out_bones.append({
        name,
        parent_index,
        final_transform()
      });
      
      for (auto [ i, child ] : children) child.collapse(out_bones, index);
    }

    void destroy () {
      for (auto [ i, child ] : children) child.destroy();
      children.destroy();
    }
  };

  

  struct DAE {
    static std::function<bool (DAEIBone const&)> std_bone_filter;


    XML xml;

    XMLItem& root;

    XMLItem* mesh;
    XMLItem* skin;

    Array<DAESource> sources;
    Array<DAEAccessor> accessors;
    Array<DAEVertexBinding> vertex_bindings;
    Array<DAETriangles> triangles_list;
    DAEJointData joint_data;
    DAEWeightData weight_data;



    /* Create a new DAE from an XML, taking ownership of the XML */
    DAE (XML const& in_xml)
    : xml(in_xml)
    , root(in_xml.first_named("COLLADA"))
    {
      try {
        XMLItem& geometries = root.first_named("library_geometries");

        XMLItem& geometry = geometries.first_named("geometry");

        if (geometries.count_of_name("geometry") > 1) printf(
          "Warning: DAE loader currently only supports single geometries, only the first (named '%s') will be loaded\n",
          geometry.get_attribute("name").value.value
        );

        XMLItem& in_mesh = geometry.first_named("mesh");

        if (geometry.count_of_name("mesh") > 1) printf(
          "Warning: DAE loader currently only supports single mesh, only the first will be loaded\n"
        );

        XMLItem& controllers = root.first_named("library_controllers");

        XMLItem& controller = controllers.first_named("controller");

        XMLItem& in_skin = controller.first_named("skin");

        if (controllers.count_of_name("controller") > 1) printf(
          "Warning: DAE loader currently only supports single controllers, only the first (named '%s') will be loaded\n",
          controller.get_attribute("name").value.value
        );

        if (controller.count_of_name("skin") > 1) printf(
          "Warning: DAE loader currently only supports single skin, only the first will be loaded\n"
        );

        in_skin.asset_assert(
          geometry.get_attribute("id").value == in_skin.get_attribute("source").value.value + 1,
          "The skin found does not have a source attribute ('%s') matching the geometry id ('%s')",
          geometry.get_attribute("id").value.value,
          in_skin.get_attribute("source").value.value
        );

        mesh = &in_mesh;
        skin = &in_skin;

        gather_base_data(in_mesh);
        gather_base_data(in_skin);


        size_t vertex_binding_count = in_mesh.count_of_name("vertices");

        vertex_bindings = { vertex_binding_count };

        for (size_t i = 0; i < vertex_binding_count; i ++) {
          XMLItem& vertices = in_mesh.nth_named(i, "vertices");
          XMLItem& input = vertices.first_named("input");

          vertex_bindings.append({
            vertices.get_attribute("id").value, // borrowing string here, destroyed by XML
            input.get_attribute("source").value // borrowing string here, destroyed by XML
          });
        }


        size_t triangles_count = in_mesh.count_of_name("triangles");

        bool polylist_mode;

        if (triangles_count == 0) {
          polylist_mode = true;
          triangles_count = in_mesh.count_of_name("polylist");
          in_mesh.asset_assert(triangles_count != 0, "Expected at least one element 'triangles' or 'polylist'");
        } else {
          polylist_mode = false;
        }

        triangles_list = { triangles_count };

        for (size_t i = 0; i < triangles_count; i ++) {
          XMLItem& triangles = polylist_mode? in_mesh.nth_named(i, "polylist") : in_mesh.nth_named(i, "triangles");

          Array<DAEInput> inputs = gather_inputs(triangles);

          size_t count = strtoumax(triangles.get_attribute("count").value.value, NULL, 10);
          
          if (polylist_mode) {
            XMLItem& vcount = triangles.first_named("vcount");

            char* base = vcount.get_text().value;
            char* end = NULL;

            for (size_t k = 0; k < count; k ++) {
              u32_t x = strtoul(base, &end, 10);
              vcount.asset_assert(end != NULL && end != base, "Less indices than expected or other parsing error at vcount %zu (expected %zu vcounts)", k, count);
              base = end;
              vcount.asset_assert(x == 3, "DAE parser only supports polylists in triangles mode, but found vcount != 3 (%" PRIu32 ")", x);
            }
          }
          
          size_t advance = inputs.reduce(static_cast<size_t>(0), [] (size_t& acc, DAEInput const& input) {
            acc = num::max(acc, input.offset);
          }) + 1;

          size_t indices_count = count * advance * 3;

          Array<u32_t> indices = { indices_count };
          
          XMLItem& p = triangles.first_named("p");
          char* base = p.get_text().value;
          char* end = NULL;

          for (size_t k = 0; k < indices_count; k ++) {
            u32_t index = strtoul(base, &end, 10);
            p.asset_assert(end != NULL && end != base, "Less indices than expected or other parsing error at index %zu (expected %zu indices)", k, indices_count);
            base = end;
            indices.append(index);
          }

          triangles_list.append({
            &triangles,
            advance,
            inputs,
            indices
          });
        }


        XMLItem& joints = in_skin.first_named("joints");
        joint_data = { &joints };

        joint_data.inputs = gather_inputs(joints);


        XMLItem& weights = in_skin.first_named("vertex_weights");
        weight_data = { &weights };

        weight_data.inputs = gather_inputs(weights);
        weight_data.count = strtoumax(weights.get_attribute("count").value.value, NULL, 10);
        
        {
          XMLItem& vcount = weights.first_named("vcount");
          char* base = vcount.get_text().value;
          char* end = NULL;

          for (size_t i = 0; i < weight_data.count; i ++) {
            u32_t num = strtoul(base, &end, 10);
            vcount.asset_assert(end != NULL && end != base, "Less values than expected or other parsing error at index %zu (expected %zu values)", i, weight_data.count);
            weight_data.vcount.append(num);
            base = end;
          }
        }

        {
          XMLItem v = weights.first_named("v");
          char* base = v.get_text().value;
          char* end = NULL;

          while (*base != '<') {
            u32_t index = strtoul(base, &end, 10);

            if (base == end || end == NULL) break;

            base = end;
            
            weight_data.indices.append(index);
          }
        }
      } catch (Exception& exception) {
        destroy();
        throw exception;
      }
    }

    void destroy () {
      for (auto [ i, source ] : sources) {
        if (source.float_array) source.floats.destroy();
        else source.names.destroy();
      }

      sources.destroy();

      accessors.destroy();

      vertex_bindings.destroy();

      for (auto [ i, triangles ] : triangles_list) {
        triangles.inputs.destroy();
        triangles.indices.destroy();
      }

      triangles_list.destroy();

      joint_data.inputs.destroy();

      weight_data.inputs.destroy();
      weight_data.vcount.destroy();
      weight_data.indices.destroy();
      
      xml.destroy();
    }


    static DAE from_str (char const* origin, char const* source) {
      return XML::from_str(origin, source);
    }

    static DAE from_str_ex (char const* origin, char* source) {
      return XML::from_str_ex(origin, source);
    }

    static DAE from_file (char const* origin) {
      return XML::from_file(origin);
    }
    

    DAEAccessor& get_accessor (String const& id) const {
      for (auto [ i, accessor ] : accessors) {
        if (accessor.id == id.value + 1) return accessor;
      }

      mesh->asset_error("Could not find Accessor '%s'", id.value);
    }

    DAESource& get_source (String const& id) const {
      for (auto [ i, source ] : sources) {
        if (source.id == id.value + 1) return source;
      }

      mesh->asset_error("Could not find Source '%s'", id.value);
    }

    static DAEInput& get_poly_input (DAETriangles const& triangles, char const* semantic) {
      for (auto [ i, input ] : triangles.inputs) {
        if (input.semantic == semantic) return input;
      }

      triangles.origin->asset_error("Could not find semantic Input '%s'", semantic);
    }

    static DAEInput* get_poly_input_pointer (DAETriangles const& triangles, char const* semantic) {
      for (auto [ i, input ] : triangles.inputs) {
        if (input.semantic == semantic) return &input;
      }

      return NULL;
    }

    DAEInput& get_wd_input (char const* semantic) const {
      for (auto [ i, input ] : weight_data.inputs) {
        if (input.semantic == semantic) return input;
      }

      weight_data.origin->asset_error("Could not find semantic Input '%s'", semantic);
    }

    DAEInput* get_wd_input_pointer (char const* semantic) const {
      for (auto [ i, input ] : weight_data.inputs) {
        if (input.semantic == semantic) return &input;
      }

      return NULL;
    }






    RenderMesh3D load_mesh (Matrix4 const& transform = Constants::Matrix4::identity) const {
      // needs a try catch wrapper!!

      Array<DAEIJoint> i_joints;

      DAEInput& weight_input = get_wd_input("WEIGHT");
      DAEAccessor& weight_accessor = get_accessor(weight_input.source_id);
      DAESource& weight_source = get_source(weight_accessor.source_id);

      {
        auto& [ wd_origin, inputs, count, vcount, indices ] = const_cast<DAEWeightData&>(weight_data); // Bug fix: const structures are improperly decomposed in c++17

        size_t j = 0;
        for (auto [ i, c ] : vcount) {
          wd_origin->asset_assert(c < DAEIJoint::max_vcount, "VCount %" PRIu32 " (at index %zu) exceeds maximum", c, i);

          DAEIJoint idata = { c };

          for (size_t k = 0; k < c; k ++) {
            size_t l = j * 2 + k * 2;
            idata.joints[k] = indices[l];
            idata.weights[k] = indices[l + 1];
          }

          i_joints.append(idata);
            
          j += c;
        }
      }


      Array<MaterialInfo> material_config_data;
      Array<DAEIVertex> i_vertices;
      Array<u32_t> i_indices;
      
      {
        for (auto [ i, triangles ] : triangles_list) {
          auto& [ _p, face_count, inputs, indices ] = triangles;

          MaterialInfo mat_info = { i, indices.count / 3, 0 };


          DAEInput& position_input = get_poly_input(triangles, "VERTEX");

          for (size_t j = 0; j < indices.count; j += triangles.advance) {
            size_t iv_index = j / triangles.advance;

            u32_t position = indices[j + position_input.offset];

            DAEIVertex iv = { triangles, iv_index };
            iv.position = position;
            i_vertices.append(iv);
          }


          DAEInput* normal_input = get_poly_input_pointer(triangles, "NORMAL");
          DAEInput* uv_input = get_poly_input_pointer(triangles, "TEXCOORD");
          DAEInput* color_input = get_poly_input_pointer(triangles, "COLOR");

          bool have_normal = normal_input != NULL;
          bool have_uv = uv_input != NULL;
          bool have_color = color_input != NULL;

          if (have_normal || have_uv || have_color) {
            for (size_t j = 0; j < indices.count; j += triangles.advance) {
              size_t iv_index = j / triangles.advance;

              s64_t normal = have_normal? static_cast<s64_t>(indices[j + normal_input->offset]) : -1;
              s64_t uv = have_uv? static_cast<s64_t>(indices[j + uv_input->offset]) : -1;
              s64_t color = have_color? static_cast<s64_t>(indices[j + color_input->offset]) : -1;

              DAEIVertex* existing_vertex = &i_vertices[iv_index];

              if (!existing_vertex->set) {
                existing_vertex->set_attributes(normal, uv, color);
                i_indices.append(iv_index);
              } else if (existing_vertex->has_same_attributes(normal, uv, color)) {
                i_indices.append(iv_index);
              } else {
                bool found_existing = false;

                while (existing_vertex->duplicate != -1) {
                  existing_vertex = &i_vertices[existing_vertex->duplicate];

                  if (existing_vertex->has_same_attributes(normal, uv, color)) {
                    i_indices.append(existing_vertex->index);
                    found_existing = true;
                    break;
                  }
                }

                if (!found_existing) {
                  size_t new_iv_index = i_vertices.count;

                  DAEIVertex new_iv = { triangles, new_iv_index };
                  new_iv.set_attributes(normal, uv, color);
                  i_vertices.append(new_iv);

                  existing_vertex->duplicate = new_iv_index;

                  i_indices.append(new_iv_index);
                }
              }
            }
          }
          

          mat_info.length = indices.count / 3 - mat_info.start_index;

          material_config_data.append(mat_info);
        }

        mesh->asset_assert(i_indices.count % 3 == 0, "Final indices count was not cleanly divisible by 3, make sure your mesh is triangulated");
      }

      Array<Vector3f> final_positions { i_vertices.count };
      Array<Vector3f> final_normals { i_vertices.count };
      Array<Vector2f> final_uvs { i_vertices.count };
      Array<Vector3f> final_colors { i_vertices.count };
      Array<Vector3u> final_faces = { i_indices.count / 3 };
      Array<Vector4u> final_joints = { i_vertices.count };
      Array<Vector4f> final_weights = { i_vertices.count };


      for (size_t i = 0; i < i_indices.count; i += 3) {
        final_faces.append({ i_indices[i], i_indices[i + 2], i_indices[i + 1] });
      }


      MaterialConfig final_material_config;

      if (material_config_data.count != 1) {
        final_material_config = MaterialConfig::from_ex(material_config_data);
      } else {
        material_config_data.destroy();
      }


      bool incomplete_normals = false;

      for (auto [ i, iv ] : i_vertices) {
        DAEInput& position_input = get_poly_input(iv.triangles, "VERTEX");
        DAEAccessor& position_accessor = get_accessor(position_input.source_id);
        DAESource& position_source = get_source(position_accessor.source_id);

        Vector3f pos;

        for (size_t j = 0; j < 3; j ++) {
          pos.elements[j] = position_source.floats[position_accessor.offset + iv.position * position_accessor.stride + j];
        }

        final_positions.append(pos.apply_matrix(transform));


        if (!incomplete_normals) {
          if (iv.normal != -1) {
            Vector3f norm = { 0, 0, 0 };

            DAEInput& normal_input = get_poly_input(iv.triangles, "NORMAL");
            DAEAccessor& normal_accessor = get_accessor(normal_input.source_id);
            DAESource& normal_source = get_source(normal_accessor.source_id);

            for (size_t j = 0; j < 3; j ++) {
              norm.elements[j] = normal_source.floats[normal_accessor.offset + iv.normal * normal_accessor.stride + j];
            }

            final_normals.append(norm);
          } else {
            incomplete_normals = true;
            final_normals.destroy();
          }
        }


        Vector2f uv = { 0, 0 };
        
        if (iv.uv != -1) {
          DAEInput& uv_input = get_poly_input(iv.triangles, "TEXCOORD");
          DAEAccessor& uv_accessor = get_accessor(uv_input.source_id);
          DAESource& uv_source = get_source(uv_accessor.source_id);

          for (size_t j = 0; j < 2; j ++) {
            uv.elements[j] = uv_source.floats[uv_accessor.offset + iv.uv * uv_accessor.stride + j];
          }
        }

        final_uvs.append(uv);


        Vector3f color = { 1, 1, 1 };
        
        if (iv.color != -1) {
          DAEInput& color_input = get_poly_input(iv.triangles, "COLOR");
          DAEAccessor& color_accessor = get_accessor(color_input.source_id);
          DAESource& color_source = get_source(color_accessor.source_id);

          for (size_t j = 0; j < 3; j ++) {
            color.elements[j] = color_source.floats[color_accessor.offset + iv.color * color_accessor.stride + j];
          }
        }

        final_colors.append(color);


        Vector4u joints = { 0, 0, 0, 0 };
        Vector4f weights = { 0, 0, 0, 0 };
        DAEIJoint& ij = i_joints[iv.position];

        if (ij.count <= 4) {
          for (size_t j = 0; j < ij.count; j ++) {
            joints[j] = ij.joints[j];
            weights[j] = weight_source.floats[weight_accessor.offset + ij.weights[j] * weight_accessor.stride];
          }
        } else {
          pair_t<u32_t, f32_t> sorted [DAEIJoint::max_vcount];

          for (size_t j = 0; j < ij.count; j ++) {
            sorted[j].a = ij.joints[j];
            sorted[j].b = weight_source.floats[weight_accessor.offset + ij.weights[j] * weight_accessor.stride];
          }

          quick_sort(sorted, 0, ij.count - 1, [&] (pair_t<u32_t, f32_t> const& x, pair_t<u32_t, f32_t> const& y) {
            return x.b > y.b;
          });

          for (size_t j = 0; j < 3; j ++) {
            joints[j] = sorted[j].a;
            weights[j] = sorted[j].b;
          }
        }

        weights.normalize();

        final_joints.append(joints);
        final_weights.append(weights);
      }


      RenderMesh3D dae_mesh = RenderMesh3D::from_ex(
        "collada!",

        true,

        final_positions.count,
        final_positions.elements,
        incomplete_normals || final_normals.count == 0? NULL : final_normals.elements,
        final_uvs.count == 0? NULL : final_uvs.elements,
        final_colors.count == 0? NULL : final_colors.elements,

        final_faces.count,
        final_faces.elements,

        final_material_config
      );


      dae_mesh.use();

      u32_t buffers [2];
      glGenBuffers(2, buffers);

      glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
      glVertexAttribIPointer(4, 4, GL_UNSIGNED_INT, sizeof(Vector4u), reinterpret_cast<void*>(0));
      glBufferData(GL_ARRAY_BUFFER, final_joints.count * sizeof(Vector4u), final_joints.elements, GL_DYNAMIC_DRAW);
      glEnableVertexAttribArray(4);

      glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
      glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vector4f), reinterpret_cast<void*>(0));
      glBufferData(GL_ARRAY_BUFFER, final_weights.count * sizeof(Vector4f), final_weights.elements, GL_DYNAMIC_DRAW);
      glEnableVertexAttribArray(5);


      i_vertices.destroy();

      i_indices.destroy();


      return dae_mesh;
    }



    Skeleton load_skeleton (Matrix4 const& transform = Constants::Matrix4::identity, bool apply_filter = true, std::function<bool (DAEIBone const&)> filter = std_bone_filter) const {
      XMLItem& lib_vis_scenes = root.first_named("library_visual_scenes");

      XMLItem& scene = lib_vis_scenes.first_named("visual_scene");

      lib_vis_scenes.asset_assert(lib_vis_scenes.count_of_name("visual_scene") == 1, "Expected exactly one visual scene");

      XMLItem& armature = scene.find_by_attribute_value("node", "id", "Armature");

      XMLItem& root_joint = armature.find_by_attribute_value("node", "type", "JOINT");


      DAEIBone root_ibone = DAEIBone::traverse(root_joint);

      root_ibone.calculate_bind(transform);


      if (apply_filter) {
        bool cont;
        do cont = root_ibone.strip_ik(filter);
        while (cont);
      }


      Array<Bone> final_bones;

      root_ibone.collapse(final_bones);
      root_ibone.destroy();
      

      Skeleton skeleton = Skeleton::from_ex(
        "collada!",
        final_bones
      );

      /*
        // for (auto [ i, bone ] : skeleton) {
        //   bone.bind_matrix = transform * bone.bind_matrix;
        //   bone.inverse_bind_matrix = bone.bind_matrix.inverse();
        // }


        // std::function<bool()> const remove_ik = [&] () {
        //   for (auto [ i, bone ] : skeleton) {
        //     if (bone.parent_index == -1) continue;

        //     if (bone.name.starts_with("IK") || bone.name.starts_with("TK")) {
        //       size_t removed_index = i;
        //       size_t new_parent_index = bone.parent_index;

        //       for (auto [ j, child_bone ] : skeleton) {
        //         if (child_bone.parent_index == removed_index) child_bone.parent_index = new_parent_index;
        //       }

        //       for (auto [ j, other_bone ] : skeleton) {
        //         if (other_bone.parent_index > removed_index) -- other_bone.parent_index;
        //       }

        //       skeleton.bones.remove(i);

        //       return true;
        //     }
        //   }

          return false;
        };

        bool removed;
        do removed = remove_ik();
        while (removed);

        
        // Bone& root_bone = skeleton.root();

        // if (root_bone.name.starts_with("IK") || root_bone.name.starts_with("TK")) {
        //   skeleton.asset_assert(skeleton.get_child_count(root_bone) == 1, "Cannot remove IK Bones from Skeleton because the root Bone is an IK Bone with more than one child Bone");

        //   size_t old_root = skeleton.root_index;
        //   size_t new_root = 0;

        //   for (auto [ i, bone ] : skeleton) {
        //     if (bone.parent_index == old_root) {
        //       new_root = i;
        //       bone.parent_index = -1;
        //       break;
        //     }
        //   }

        //   skeleton.bones.remove(old_root);

        //   for (auto [ i, bone ] : skeleton) {
        //     if (bone.parent_index > old_root) -- bone.parent_index;
        //   }
        // }
      */

      return skeleton;
    }







  private:
    DAEVertexBinding& get_vertex_binding (String const& id) const {
      for (auto [ k, binding ] : vertex_bindings) {
        if (binding.id == id.value + 1) return binding;
      }

      mesh->asset_error("Could not find VertexBinding '%s'", id.value);
    }

    Array<DAEInput> gather_inputs (XMLItem& origin) const {
      size_t input_count = origin.count_of_name("input");

      Array<DAEInput> inputs = { input_count };

      for (size_t j = 0; j < input_count; j ++) {
        XMLItem& input = origin.nth_named(j, "input");

        String semantic = input.get_attribute("semantic").value; // borrowing string here, destroyed by XML
        String source_id = input.get_attribute("source").value; // borrowing string here, destroyed by XML

        if (semantic == "VERTEX") {
          source_id = get_vertex_binding(source_id).source_id;
        }

        XMLAttribute* set = input.get_attribute_pointer("set");
        XMLAttribute* offset = input.get_attribute_pointer("offset");
        inputs.append({
          semantic,
          source_id,
          offset != NULL? strtoumax(offset->value.value, NULL, 10) : 0,
          set != NULL? strtoumax(set->value.value, NULL, 10) : 0
        });
      }

      return inputs;
    }

    void gather_base_data (XMLItem& section) {
      size_t sources_count = section.count_of_name("source");

      for (size_t i = 0; i < sources_count; i ++) {
        XMLItem source = section.nth_named(i, "source");

        XMLItem* float_array = source.first_named_pointer("float_array");
        XMLItem* name_array = source.first_named_pointer("Name_array");

        if (float_array != NULL) {
          String id = float_array->get_attribute("id").value; // borrowing string here, destroyed by XML

          size_t float_count = strtoumax(float_array->get_attribute("count").value.value, NULL, 10);
          Array<f64_t> floats { float_count };

          char* base = float_array->get_text().value;
          char* end = NULL;
          for (size_t j = 0; j < float_count; j ++) {
            floats.append(strtod(base, &end));
            float_array->asset_assert(end != base, "Failed to parse float at index %zu", j);
            base = end;
          }

          sources.append({ id, floats });
        } else if (name_array != NULL) {
          String id = name_array->get_attribute("id").value;

          size_t name_count = strtoumax(name_array->get_attribute("count").value.value, NULL, 10);
          Array < pair_t<char*, size_t> > names;

          char* base = name_array->get_text().value;
          char* end = NULL;
          for (size_t j = 0; j < name_count; j ++) {
            while (char_is_whitespace(*base)) ++ base;
            end = base;
            while (!char_is_whitespace(*end)) ++ end;
            names.append({ base, static_cast<size_t>(end - base) });
            base = end;
          }

          sources.append({ id, names });
        }

        XMLItem accessor = source.first_named("technique_common").first_named("accessor");

        XMLAttribute* offset = accessor.get_attribute_pointer("offset");

        accessors.append({
          source.get_attribute("id").value, // borrowing string here, destroyed by XML
          accessor.get_attribute("source").value, // borrowing string here, destroyed by XML
          strtoumax(accessor.get_attribute("count").value.value, NULL, 10),
          offset != NULL? strtoumax(offset->value.value, NULL, 10) : 0,
          strtoumax(accessor.get_attribute("stride").value.value, NULL, 10),
          accessor.count_of_name("param")
        });
      }
    }
  };

  std::function<bool (DAEIBone const&)> DAE::std_bone_filter = [] (DAEIBone const& bone) { return bone.name.starts_with("IK") || bone.name.starts_with("TK"); };
}