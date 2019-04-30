#include "../main.hh"

mod::RenderMesh3D load_dae (char const* path) {
  using namespace mod;

  XML xml = XML::from_file(path);

  XMLItem& collada = xml.first_named("COLLADA");

  XMLItem& geometries = collada.first_named("library_geometries");

  XMLItem& geometry = geometries.first_named("geometry");

  if (geometries.count_of_name("geometry") > 1) printf(
    "Warning: Collada loader currently only supports single geometries, only the first (named '%s') will be loaded\n",
    geometry.get_attribute("name").value.value
  );

  XMLItem& mesh = geometry.first_named("mesh");

  XMLItem& controllers = collada.first_named("library_controllers");

  XMLItem& controller = controllers.first_named("controller");

  XMLItem& skin = controller.first_named("skin");

  if (controllers.count_of_name("controller") > 1) printf(
    "Warning: Collada loader currently only supports single controllers, only the first (named '%s') will be loaded\n",
    controller.get_attribute("name").value.value
  );

  if (controller.count_of_name("controller") > 1) printf(
    "Warning: Collada loader currently only supports single skin, only the first (named '%s') will be loaded\n",
    skin.get_attribute("name").value.value
  );

  skin.asset_assert(
    geometry.get_attribute("id").value == skin.get_attribute("source").value.value + 1,
    "The skin found does not have a source attribute ('%s') matching the geometry id ('%s')",
    geometry.get_attribute("id").value.value,
    skin.get_attribute("source").value.value
  );


  // gather source data //

  struct Source {
    String id;
    bool float_array;
    union {
      Array<f64_t> floats;
      Array< pair_t<char*, size_t> > names;
    };

    Source () { }
    Source (String in_id, Array<f64_t> in_floats)
    : id(in_id)
    , float_array(true)
    , floats(in_floats)
    { }
    Source (String in_id, Array< pair_t<char*, size_t> > in_names)
    : id(in_id)
    , float_array(false)
    , names(in_names)
    { }
  };

  Array<Source> sources;

  struct Accessor {
    String id;
    String source_id;
    size_t count;
    size_t offset;
    size_t stride;
    size_t elements;
  };

  Array<Accessor> accessors;

  const auto gather_base_data = [&] (XMLItem& base) {
    size_t sources_count = base.count_of_name("source");

    for (size_t i = 0; i < sources_count; i ++) {
      XMLItem source = base.nth_named(i, "source");

      XMLItem* float_array = source.first_named_pointer("float_array");
      XMLItem* name_array = source.first_named_pointer("Name_array");

      if (float_array != NULL) {
        String id = float_array->get_attribute("id").value; // borrowing string here, destroyed by XML

        size_t float_count = strtoumax(float_array->get_attribute("count").value.value, NULL, 10);
        Array<f64_t> floats { float_count };

        char* base = float_array->get_text().value;
        char* end = NULL;
        for (size_t i = 0; i < float_count; i ++) {
          floats.append(strtod(base, &end));
          float_array->asset_assert(end != base, "Failed to parse float at index %zu", i);
          base = end;
        }

        sources.append({ id, floats });
      } else if (name_array != NULL) {
        String id = name_array->get_attribute("id").value;

        size_t name_count = strtoumax(name_array->get_attribute("count").value.value, NULL, 10);
        Array < pair_t<char*, size_t> > names;

        char* base = name_array->get_text().value;
        char* end = NULL;
        for (size_t i = 0; i < name_count; i ++) {
          while (char_is_whitespace(*base)) ++ base;
          end = base;
          while (!char_is_whitespace(*end)) ++ end;
          names.append({ base, (size_t) (end - base) });
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
  };

  gather_base_data(mesh);
  gather_base_data(skin);



  
  // Gather inputs //

  struct VertexBinding {
    String id;
    String source_id;
  };

  size_t vertex_binding_count = mesh.count_of_name("vertices");

  Array<VertexBinding> vertex_bindings { vertex_binding_count };

  for (size_t i = 0; i < vertex_binding_count; i ++) {
    XMLItem& vertices = mesh.nth_named(i, "vertices");
    XMLItem& input = vertices.first_named("input");

    vertex_bindings.append({
      vertices.get_attribute("id").value, // borrowing string here, destroyed by XML
      input.get_attribute("source").value // borrowing string here, destroyed by XML
    });
  }

  const auto get_vertex_binding = [&] (String const& id) -> VertexBinding& {
    for (auto [ k, binding ] : vertex_bindings) {
      if (binding.id == id.value + 1) return binding;
    }

    mesh.asset_error("Could not find VertexBinding '%s'", id);
  };


  struct Input {
    String semantic;
    String source_id;
    size_t offset;
    size_t set;
  };

  struct Polylist {
    XMLItem& origin;
    size_t count;
    Array<Input> inputs;
    Array<u32_t> indices;
  };

  size_t polylist_count = mesh.count_of_name("polylist");

  Array<Polylist> polylists { polylist_count };

  const auto gather_inputs = [&] (XMLItem& origin) -> Array<Input> {
    size_t input_count = origin.count_of_name("input");

    Array<Input> inputs = { input_count };

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
  };

  for (size_t i = 0; i < polylist_count; i ++) {
    XMLItem& polylist = mesh.nth_named(i, "polylist");

    Array<Input> inputs = gather_inputs(polylist);

    size_t count = strtoumax(polylist.get_attribute("count").value.value, NULL, 10);
    
    size_t indices_count = count * inputs.count * 3;

    Array<u32_t> indices = { indices_count };
    
    XMLItem& p = polylist.first_named("p");
    char* base = p.get_text().value;
    char* end = NULL;

    for (size_t k = 0; k < indices_count; k ++) {
      u32_t index = strtoul(base, &end, 10);
      p.asset_assert(end != NULL && end != base, "Less indices than expected or other parsing error at index %zu (expected %zu indices)", k, indices_count);
      base = end;
      indices.append(index);
    }


    polylists.append({
      polylist,
      count,
      inputs,
      indices
    });
  }


  struct JointData {
    XMLItem& origin;
    Array<Input> inputs;
  };

  XMLItem& joints = skin.first_named("joints");
  JointData joint_data = { joints };

  joint_data.inputs = gather_inputs(joints);


  struct WeightData {
    XMLItem& origin;
    Array<Input> inputs;
    size_t count;
    Array<u32_t> vcount;
    Array<u32_t> indices;
  };

  XMLItem& weights = skin.first_named("vertex_weights");
  WeightData weight_data = { weights };

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


  const auto get_accessor = [&] (String const& id) -> Accessor& {
    for (auto [ i, accessor ] : accessors) {
      if (accessor.id == id.value + 1) return accessor;
    }

    mesh.asset_error("Could not find Accessor '%s'", id.value);
  };

  const auto get_source = [&] (String const& id) -> Source& {
    for (auto [ i, source ] : sources) {
      if (source.id == id.value + 1) return source;
    }

    mesh.asset_error("Could not find Source '%s'", id.value);
  };

  const auto get_poly_input = [&] (Polylist const& polylist, char const* semantic) -> Input& {
    for (auto [ i, input ] : polylist.inputs) {
      if (input.semantic == semantic) return input;
    }

    polylist.origin.asset_error("Could not find semantic Input '%s'", semantic);
  };

  const auto get_poly_input_pointer = [&] (Polylist const& polylist, char const* semantic) -> Input* {
    for (auto [ i, input ] : polylist.inputs) {
      if (input.semantic == semantic) return &input;
    }

    return NULL;
  };

  const auto get_wd_input = [&] (WeightData const& wd, char const* semantic) -> Input& {
    for (auto [ i, input ] : wd.inputs) {
      if (input.semantic == semantic) return input;
    }

    wd.origin.asset_error("Could not find semantic Input '%s'", semantic);
  };

  const auto get_wd_input_pointer = [&] (WeightData const& wd, char const* semantic) -> Input* {
    for (auto [ i, input ] : wd.inputs) {
      if (input.semantic == semantic) return &input;
    }

    return NULL;
  };



  // /*
    // print //

    // for (auto [ i, polylist ] : polylists) {
    //   auto& [ _p, count, inputs, indices ] = polylist;

    //   printf("Polylist %zu\n- Count: %zu\n- Inputs: %zu\n- Indices: %zu\n", i, count, inputs.count, indices.count);

    //   printf("  Indices\n  - [ ");
    //   for (size_t j = 0; j < indices.count; j += inputs.count) {
    //     printf("[ ");
    //     for (size_t k = j; k < j + inputs.count; k ++) {
    //       printf("%" PRIu32, indices[k]);
    //       if (k < j + inputs.count - 1) printf(", ");
    //     }
    //     printf(" ], ");

    //     if (j > 4 * inputs.count) {
    //       printf("...");
    //       break;
    //     }
    //   }
    //   printf(" ]\n");

    //   for (auto [ j, input ] : inputs) {
    //     auto& [ semantic, source_id, offset, set ] = input;
    //     printf("  Input %zu\n  - Semantic: %s\n  - Source ID: %s\n  - Offset: %zu\n  - Set: %zu\n", j, semantic.value, source_id.value, offset, set);

    //     auto& [ id, asource_id, acount, aoffset, stride, elements ] = get_accessor(source_id);
    //     printf("    Accessor\n    - Source ID: %s\n    - Count: %zu\n    - Offset: %zu\n    - Stride: %zu\n    - Elements: %zu\n", asource_id.value, acount, aoffset, stride, elements);

    //     auto& [ sid, floats ] = get_source(asource_id);
    //     printf("      Source\n      - Count: %zu\n      - Floats: [ ", floats.count);

    //     for (size_t k = aoffset; k < acount * stride; k += stride) {
    //       printf("[ ");
    //       for (size_t l = k; l < k + elements; l ++) {
    //         printf("%lf", floats[l]);
    //         if (l < k + elements - 1) printf(", ");
    //       }
    //       printf(" ], ");

    //       if (k > 3 * 3) {
    //         printf("...");
    //         break;
    //       }
    //     }
    //     printf(" ]\n");
    //   }

    //   printf("\n");
    // }

    // {
    //   auto& [ _jd, inputs ] = joint_data;
    //   printf("Joint Data\n- Inputs: %zu\n", inputs.count);

    //   for (auto [ i, input ] : inputs) {
    //     auto& [ semantic, source_id, offset, set ] = input;
    //     printf("  Input %zu\n  - Semantic: %s\n  - Source ID: %s\n  - Offset: %zu\n  - Set: %zu\n", i, semantic.value, source_id.value, offset, set);
        
    //     auto& [ id, asource_id, acount, aoffset, stride, elements ] = get_accessor(source_id);
    //     printf("    Accessor\n    - Source ID: %s\n    - Count: %zu\n    - Offset: %zu\n    - Stride: %zu\n    - Elements: %zu\n", asource_id.value, acount, aoffset, stride, elements);

    //     Source& source = get_source(asource_id);
    //     printf("      Source\n      - Count: %zu\n", source.float_array? source.floats.count : source.names.count);

    //     printf("      - %s: [ ", source.float_array? "floats" : "names");
    //     for (size_t k = aoffset; k < acount * stride; k += stride) {
    //       printf("[ ");
    //       for (size_t l = k; l < k + elements; l ++) {
    //         if (source.float_array) {
    //           printf("%lf", source.floats[l]);
    //         } else {
    //           printf("%.*s", (s32_t) source.names[l].b, source.names[l].a);
    //         }
    //         if (l < k + elements - 1) printf(", ");
    //       }
    //       printf(" ], ");

    //       if (k > 3 * 3) {
    //         printf("...");
    //         break;
    //       }
    //     }
    //     printf(" ]\n");
    //   }
    // }

    // {
    //   auto& [ _wd, inputs, count, vcount, indices ] = weight_data;
    //   printf("Weight Data\n- Inputs: %zu\n- Count: %zu\n- VCount: %zu\n- Indices: %zu\n", inputs.count, count, vcount.count, indices.count);

    //   for (auto [ i, input ] : inputs) {
    //     auto& [ semantic, source_id, offset, set ] = input;
    //     printf("  Input %zu\n  - Semantic: %s\n  - Source ID: %s\n  - Offset: %zu\n  - Set: %zu\n", i, semantic.value, source_id.value, offset, set);

    //     auto& [ id, asource_id, acount, aoffset, stride, elements ] = get_accessor(source_id);
    //     printf("    Accessor\n    - Source ID: %s\n    - Count: %zu\n    - Offset: %zu\n    - Stride: %zu\n    - Elements: %zu\n", asource_id.value, acount, aoffset, stride, elements);

    //     Source& source = get_source(asource_id);
    //     printf("      Source\n      - Count: %zu\n", source.float_array? source.floats.count : source.names.count);

    //     printf("      - %s: [ ", source.float_array? "floats" : "names");
    //     for (size_t k = aoffset; k < acount * stride; k += stride) {
    //       printf("[ ");
    //       for (size_t l = k; l < k + elements; l ++) {
    //         if (source.float_array) {
    //           printf("%lf", source.floats[l]);
    //         } else {
    //           printf("%.*s", (s32_t) source.names[l].b, source.names[l].a);
    //         }
    //         if (l < k + elements - 1) printf(", ");
    //       }
    //       printf(" ], ");

    //       if (k > 3 * 3) {
    //         printf("...");
    //         break;
    //       }
    //     }
    //     printf(" ]\n");
    //   }

    //   printf("  VCount [ ");
    //   for (size_t i = 0; i < vcount.count; i ++) {
    //     u32_t c = vcount[i];
    //     printf("%" PRIu32, c);
    //     if (i < vcount.count - 1) printf(", ");
    //     if (i == 16) { printf("..."); i = 140; }
    //     if (i > 200) { printf("..."); break; }
    //   }
    //   printf(" ]\n");

    //   printf("  Indices [ ");
    //   for (auto [ i, x ] : indices) {
    //     printf("%" PRIu32, x);
    //     if (i < indices.count - 1) printf(", ");
    //     if (i > 100) { printf("..."); break; }
    //   }
    //   printf(" ]\n");
    // }
  // */

  static constexpr size_t max_vcount = 6;

  struct IJoint {
    u32_t count;
    u32_t joints [max_vcount];
    u32_t weights [max_vcount];
    IJoint () { }
    IJoint (u32_t in_count)
    : count(in_count) { }
  };


  Array<IJoint> i_joints;

  Input& joint_input = get_wd_input(weight_data, "JOINT");
  Accessor& joint_accessor = get_accessor(joint_input.source_id);
  Source& joint_source = get_source(joint_accessor.source_id);

  Input& weight_input = get_wd_input(weight_data, "WEIGHT");
  Accessor& weight_accessor = get_accessor(weight_input.source_id);
  Source& weight_source = get_source(weight_accessor.source_id);

  {
    auto& [ _wd, inputs, count, vcount, indices ] = weight_data;

    size_t j = 0;
    for (auto [ i, c ] : vcount) {
      IJoint idata = { c };
      for (size_t k = 0; k < c; k ++) {
        size_t l = j * 2 + k * 2;
        idata.joints[k] = indices[l];
        idata.weights[k] = indices[l + 1];
      }

      i_joints.append(idata);
        
      j += c;
    }
  }

  // for (auto [ i, ij ] : i_joints) {
  //   printf("%zu (%u) [ ", i, ij.count);
  //   for (size_t j = 0; j < ij.count; j ++) {
  //     printf("%u: %u (%.3f)", ij.joints[j], ij.weights[j], weight_source.floats[weight_accessor.offset + ij.weights[j] * weight_accessor.stride]);
  //     if (j < ij.count - 1) printf(", ");
  //   }
  //   printf(" ]\n");
  // }


  struct IVertex {
    Polylist& polylist;

    bool set;

    size_t index;

    s64_t duplicate;

    u32_t position;

    s64_t normal;
    s64_t uv;
    s64_t color;


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



  Array<MaterialInfo> material_config_data;
  Array<IVertex> i_vertices;
  Array<u32_t> i_indices;
  
  {
    for (auto [ i, polylist ] : polylists) {
      auto& [ _p, face_count, inputs, indices ] = polylist;

      MaterialInfo mat_info = { i, indices.count / 3, 0 };


      Input& position_input = get_poly_input(polylist, "VERTEX");

      for (size_t j = 0; j < indices.count; j += inputs.count) {
        size_t iv_index = j / inputs.count;

        u32_t position = indices[j + position_input.offset];

        IVertex iv = { polylist, false, iv_index, -1 };
        iv.position = position;
        i_vertices.append(iv);
      }


      Input* normal_input = get_poly_input_pointer(polylist, "NORMAL");
      Input* uv_input = get_poly_input_pointer(polylist, "TEXCOORD");
      Input* color_input = get_poly_input_pointer(polylist, "COLOR");

      bool have_normal = normal_input != NULL;
      bool have_uv = uv_input != NULL;
      bool have_color = color_input != NULL;

      if (have_normal || have_uv || have_color) {
        for (size_t j = 0; j < indices.count; j += inputs.count) {
          size_t iv_index = j / inputs.count;

          s64_t normal = have_normal? indices[j + normal_input->offset] : -1;
          s64_t uv = have_uv? indices[j + uv_input->offset] : -1;
          s64_t color = have_color? indices[j + color_input->offset] : -1;

          IVertex* existing_vertex = &i_vertices[iv_index];

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

              IVertex new_iv = { polylist, false, new_iv_index, -1 };
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

    mesh.asset_assert(i_indices.count % 3 == 0, "Final indices count was not cleanly divisible by 3, make sure your mesh is triangulated");
  }

  Array<Vector3f> final_positions { i_vertices.count };
  Array<Vector3f> final_normals { i_vertices.count };
  Array<Vector2f> final_uvs { i_vertices.count };
  Array<Vector3f> final_colors { i_vertices.count };
  Array<Vector3u> final_faces = { i_indices.count / 3 };
  Array<Vector3u> final_joints = { i_vertices.count };
  Array<Vector3f> final_weights = { i_vertices.count };


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
    Input& position_input = get_poly_input(iv.polylist, "VERTEX");
    Accessor& position_accessor = get_accessor(position_input.source_id);
    Source& position_source = get_source(position_accessor.source_id);

    Vector3f pos;

    for (size_t j = 0; j < 3; j ++) {
      pos.elements[j] = position_source.floats[position_accessor.offset + iv.position * position_accessor.stride + j];
    }

    final_positions.append(pos);


    if (!incomplete_normals) {
      if (iv.normal != -1) {
        Vector3f norm = { 0, 0, 0 };

        Input& normal_input = get_poly_input(iv.polylist, "NORMAL");
        Accessor& normal_accessor = get_accessor(normal_input.source_id);
        Source& normal_source = get_source(normal_accessor.source_id);

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
      Input& uv_input = get_poly_input(iv.polylist, "TEXCOORD");
      Accessor& uv_accessor = get_accessor(uv_input.source_id);
      Source& uv_source = get_source(uv_accessor.source_id);

      for (size_t j = 0; j < 2; j ++) {
        uv.elements[j] = uv_source.floats[uv_accessor.offset + iv.uv * uv_accessor.stride + j];
      }
    }

    final_uvs.append(uv);


    Vector3f color = { 1, 1, 1 };
    
    if (iv.color != -1) {
      Input& color_input = get_poly_input(iv.polylist, "COLOR");
      Accessor& color_accessor = get_accessor(color_input.source_id);
      Source& color_source = get_source(color_accessor.source_id);

      for (size_t j = 0; j < 3; j ++) {
        color.elements[j] = color_source.floats[color_accessor.offset + iv.color * color_accessor.stride + j];
      }
    }

    final_colors.append(color);


    Vector3u joints = { 0, 0, 0 };
    Vector3f weights = { 0, 0, 0 };
    IJoint& ij = i_joints[iv.position];

    if (ij.count <= 3) {
      for (size_t j = 0; j < ij.count; j ++) {
        joints[j] = ij.joints[j];
        weights[j] = weight_source.floats[weight_accessor.offset + ij.weights[j] * weight_accessor.stride];
      }
    } else {
      pair_t<u32_t, f32_t> sorted [max_vcount];

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

      weights.normalize();
    }

    final_joints.append(joints);
    final_weights.append(weights);
  }

  // FILE* jdump = fopen("jdump.txt", "w");
  // for (size_t i = 0; i < final_joints.count; i ++) {
  //   Vector3u& joint = final_joints[i];
  //   Vector3f& weight = final_weights[i];
    
  //   fprintf(jdump, "%zu [ %u: %.3f, %u: %.3f, %u: %.3f ]\n", i, joint[0], weight[0], joint[1], weight[1], joint[2], weight[2]);
  // }
  // fclose(jdump);


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
  glVertexAttribIPointer(4, 3, GL_UNSIGNED_INT, sizeof(Vector3u), (void*) 0);
  glBufferData(GL_ARRAY_BUFFER, final_joints.count * sizeof(Vector3u), final_joints.elements, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(4);

  glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3f), (void*) 0);
  glBufferData(GL_ARRAY_BUFFER, final_weights.count * sizeof(Vector3f), final_weights.elements, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(5);

  // cleanup //

  for (auto [ i, source ] : sources) {
    if (source.float_array) source.floats.destroy();
    else source.names.destroy();
  }

  sources.destroy();


  accessors.destroy();


  vertex_bindings.destroy();


  for (auto [ i, polylist ] : polylists) {
    polylist.inputs.destroy();
    polylist.indices.destroy();
  }

  polylists.destroy();


  joint_data.inputs.destroy();
  weight_data.inputs.destroy();
  weight_data.vcount.destroy();
  weight_data.indices.destroy();


  i_vertices.destroy();

  i_indices.destroy();


  xml.destroy();


  return dae_mesh;
}