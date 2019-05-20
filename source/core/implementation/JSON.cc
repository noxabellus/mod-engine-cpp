#include "../include/JSON.hh"


namespace mod {
  void JSONObject::destroy () {
    for (auto [ i, key ] : keys) key.destroy();
    keys.destroy();

    for (auto [ i, item ] : items) item.destroy();
    items.destroy();
  }


  JSONObjectIterator JSONObject::begin () const {
    return { keys.elements, items.elements, 0 };
  }

  JSONObjectIterator JSONObject::end () const {
    return { keys.elements, items.elements, items.count };
  }


  s64_t JSONObject::get_index (char const* key_value, size_t key_length) const {
    for (auto [ i, key ] : keys) {
      if (key.equal_caseless(key_value, key_length)) return i;
    }

    return -1;
  }

  String* JSONObject::get_key (char const* key_value, size_t key_length) const {
    s64_t index = get_index(key_value, key_length);

    if (index != -1) return keys.get_element(index);
    else return NULL;
  }

  JSONItem* JSONObject::get (char const* key_value, size_t key_length) const {
    s64_t index = get_index(key_value, key_length);

    if (index != -1) return items.get_element(index);
    else return NULL;
  }


  size_t JSONObject::set (JSONItem const* item, char const* key_value, size_t key_length) {
    s64_t existing_item_index = get_index(key_value, key_length);

    if (existing_item_index != -1) {
      JSONItem* existing_item = items.get_element(existing_item_index);

      existing_item->destroy();

      existing_item->type = item->type;

      memory::copy(&existing_item->object, &item->object);

      return existing_item_index;
    } else {
      size_t index = items.count;

      keys.append({ key_value, key_length });
      items.append(item);

      return index;
    }
  }


  s64_t JSONObject::set_unique (JSONItem const* item, char const* key_value, size_t key_length) {
    s64_t existing_item_index = get_index(key_value, key_length);

    if (existing_item_index != -1) {
      return -1;
    } else {
      size_t index = items.count;

      keys.append({ key_value, key_length });
      items.append(item);

      return index;
    }
  }


  s64_t JSONObject::set_unique_string_key (JSONItem const* item, String key) {
    s64_t existing_item_index = get_index(key.value, key.length);

    if (existing_item_index != -1) {
      return -1;
    } else {
      size_t index = items.count;

      keys.append(key);
      items.append(item);

      return index;
    }
  }


  void JSONObject::remove (char const* key_value, size_t key_length) {
    s64_t index = get_index(key_value, key_length);

    if (index == -1) return;

    keys.remove(index);
    items.remove(index);
  }







  JSONItem JSONItem::from_str (JSON* root, char const* str, size_t* offset) {
    while (char_is_whitespace(str[*offset])) ++ *offset;

    JSONItem item = { root, *offset };

    try {
      switch (str[*offset]) {
        case 't':
        case 'f': {
          item.type = JSONType::Boolean;

          if (strncmp(str + *offset, "true", 4) == 0) {
            *offset += 4;
            item.boolean = true;
          } else if (strncmp(str + *offset, "false", 5) == 0) {
            *offset += 5;
            item.boolean = false;
          } else {
            ++ *offset;
            goto err;
          }
        } break;


        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
        case '+': case '-':
        case '.': {
          item.type = JSONType::Number;
          char const* base = str + *offset;
          char const* end = NULL;

          double number = strtod(base, const_cast<char**>(&end));

          item.asset_assert(end != NULL && end > base, "Failed to parse number");

          *offset += end - base;

          item.number = number;
        } break;


        case '"': {
          item.type = JSONType::String;
          item.string = root->unescape_string_from_source(str, offset);
        } break;


        case '[': {
          ++ *offset;

          item.type = JSONType::Array;

          item.array = { };

          while (str[*offset] != ']' && str[*offset] != '\0') {
            if (item.array.count > 0) {
              root->asset_assert(str[*offset] == ',', *offset, "Expected a value separator ',' or end of array ']', not '%c'", str[*offset]);
              ++ *offset;
            }

            item.array.append(JSONItem::from_str(root, str, offset));

            while (char_is_whitespace(str[*offset])) ++ *offset;
          }

          root->asset_assert(str[*offset] == ']', *offset, "Unexpected end of input, expected ']' to close array");

          ++ *offset;
        } break;


        case '{': {
          ++ *offset;

          item.type = JSONType::Object;
          item.object = { };

          while (str[*offset] != '}' && str[*offset] != '\0') {
            if (item.object.items.count > 0) {
              root->asset_assert(str[*offset] == ',', *offset, "Expected a value separator ',' or end of object '}', not '%c'", str[*offset]);
              ++ *offset;
            }


            String key = root->unescape_string_from_source(str, offset);


            while (char_is_whitespace(str[*offset])) ++ *offset;

            root->asset_assert(str[*offset] == ':', *offset, "Expected a key/value pair separator ':', not '%c'", str[*offset]);
            ++ *offset;
            

            auto obj_value = JSONItem::from_str(root, str, offset);
            item.set_object_item_unique_string_key(obj_value, key);


            while (char_is_whitespace(str[*offset])) ++ *offset;
          }

          root->asset_assert(str[*offset] == '}', *offset, "Unexpected end of input, expected '}' to close object");

          ++ *offset;
        } break;


        default: err: item.asset_error("Unexpected character '%c' while parsing JSONItem from str", str[*offset]);
      }
    } catch (Exception& exception) {
      item.destroy();
      throw exception;
    }

    return item;
  }


  void JSONItem::destroy () const {
    switch (type) {
      case JSONType::String: memory::deallocate_const(static_cast<void*>(string.value)); break;
      case JSONType::Array: {
        for (auto [ i, item ] : array) item.destroy();
        memory::deallocate_const(static_cast<void*>(array.elements));
      } break;
      case JSONType::Object: {
        for (auto [ i, key ] : object.keys) memory::deallocate_const(static_cast<void*>(key.value));
        memory::deallocate_const(static_cast<void*>(object.keys.elements));
        for (auto [ i, item ] : object.items) item.destroy();
        memory::deallocate_const(static_cast<void*>(object.items.elements));
      } break;
      default: break;
    }

    // type = JSONType::Invalid;
  }


  void JSONItem::to_string (String* out_string, size_t indent) const {
    switch (type) {
      case JSONType::Boolean: return out_string->fmt_append("%s", boolean? "true" : "false");

      case JSONType::Number: return out_string->fmt_append("%g", number);

      case JSONType::String: return JSON::escape_string_to_source(&string, out_string);

      case JSONType::Array: {
        out_string->append("[\n");
        ++ indent;

        for (auto [ i, item ] : array) {
          JSON::indent_source(out_string, indent);

          item.to_string(out_string, indent);

          if (i < array.count - 1) out_string->append(",\n");
        }

        -- indent;

        out_string->append("\n");
        JSON::indent_source(out_string, indent);
        out_string->append("]");
      } break;

      case JSONType::Object: {
        out_string->append("{\n");
        ++ indent;

        for (size_t i = 0; i < object.items.count; i ++) {
          String& key = object.keys[i];
          JSONItem& item = object.items[i];

          JSON::indent_source(out_string, indent);
          JSON::escape_string_to_source(&key, out_string);
          out_string->append(": ");
          item.to_string(out_string, indent);
          if (i < object.items.count - 1) out_string->append(",\n");
        }

        -- indent;

        out_string->append("\n");
        JSON::indent_source(out_string, indent);
        out_string->append("}");
      } break;
    }
  }








  String JSON::unescape_string_from_source (char const* str, size_t* offset) const {
    while (char_is_whitespace(str[*offset])) ++ *offset;

    asset_assert(str[*offset] == '"', *offset, "Expected a '\"' designating the start of a String, not '%c'", str[*offset]);
    ++ *offset;

    String new_string;

    try {
      while (str[*offset] != '"' && str[*offset] != '\0') {
        if (str[*offset] == '\\') {
          ++ *offset;

          switch (str[*offset]) {
            case 'n': new_string.append("\n"); break;
            case 'f': new_string.append("\f"); break;
            case 'r': new_string.append("\r"); break;
            case 't': new_string.append("\t"); break;
            case 'b': new_string.append("\b"); break;
            case '\\': new_string.append("\\"); break;
            case '/': new_string.append("/"); break;
            case '"': new_string.append("\""); break;
            case 'u': asset_error(*offset, "Error parsing String: Unicode escapes are not yet supported", *offset); break;
            default: asset_error(*offset, "Invalid escape character '%c' in String ", str[*offset]);
          }
        } else {
          new_string.append(str + *offset, 1);
        }
        
        ++ *offset;
      }
      
      asset_assert(str[*offset] == '\"', *offset, "Unexpected end of input, expected '\"' to close String");

      ++ *offset;
    } catch (Exception& exception) {
      new_string.destroy();
      throw exception;
    }

    return new_string;
  }


  void JSON::escape_string_to_source (String const* in_string, String* out_string) {
    out_string->append("\"");

    for (size_t i = 0; i < in_string->length; i ++) {
      char const* curr_char = in_string->value + i;

      switch (*curr_char) {
        case '\n': out_string->append("\\n"); break;
        case '\r': out_string->append("\\r"); break;
        case '\f': out_string->append("\\f"); break;
        case '\t': out_string->append("\\t"); break;
        case '\b': out_string->append("\\b"); break;
        case '\\': out_string->append("\\\\"); break;
        case '"': out_string->append("\\\""); break;
        default: out_string->append(curr_char, 1); break;
      }
    }

    out_string->append("\"");
  }


  JSON JSON::from_str (char const* origin, char const* source, size_t source_length) {
    JSON json { origin, source, source_length };

    try {
      size_t offset = 0;
      json.data = JSONItem::from_str(&json, json.source, &offset);

      json.data.asset_assert(json.data.type == JSONType::Array || json.data.type == JSONType::Object, "Invalid data type, expected Array or Object, not %s", JSONType::name(json.data.type));
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    return json;
  }

  JSON JSON::from_str_ex (char const* origin, char* source) {
    JSON json { str_clone(origin), source, JSONItem { } };
    
    try {
      size_t offset = 0;
      json.data = JSONItem::from_str(&json, json.source, &offset);

      json.data.asset_assert(json.data.type == JSONType::Array || json.data.type == JSONType::Object, "Invalid data type, expected Array or Object, not %s", JSONType::name(json.data.type));
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    return json;
  }

  JSON JSON::from_file (char const* path) {
    auto [ data, length ] = load_file(path);
    
    m_asset_assert(data != NULL, path, "Failed to load source file");

    return from_str_ex(path, static_cast<char*>(data));
  }




  pair_t<s32_t, s32_t> JSON::get_line_column_info (size_t origin_offset) const {
    if (origin_offset == 0 || source == NULL) return { -1, -1 };

    s32_t line = 1;
    s32_t column = 0;

    for (size_t i = 0; i < origin_offset; i ++) {
      if (source[i] == '\n') {
        ++ line;
        column = 0;
      } else if (source[i] == '\0') {
        column = 0;
        break;
      } else ++ column;
    }

    return { line, column };
  }
}