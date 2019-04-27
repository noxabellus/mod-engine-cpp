#include "../include/XML.hh"



namespace mod {
  void XMLItem::destroy_value () {
    switch (type) {
      case XMLType::Comment:
      case XMLType::ProcessingInstruction: break;

      case XMLType::CDATA:
      case XMLType::DocumentType:
      case XMLType::Text: return text.destroy();

      case XMLType::Array: {
        for (auto [ i, element ] : array) element.destroy();
        return array.destroy();
      }
    }

    type = XMLType::Empty;
  }

  void XMLItem::destroy () {
    name.destroy();

    for (auto [ i, attribute ] : attributes) attribute.destroy();

    attributes.destroy();

    destroy_value();
  }


  void XMLItem::destroy () const {
    free((void*) name.value);

    for (auto [ i, attribute ] : attributes) {
      free((void*) attribute.name.value);
      free((void*) attribute.value.value);
    }

    free((void*) attributes.elements);
    
    switch (type) {
      case XMLType::Comment:
      case XMLType::ProcessingInstruction: break;

      case XMLType::CDATA:
      case XMLType::DocumentType:
      case XMLType::Text: free(text.value); break;

      case XMLType::Array: {
        for (auto [ i, element ] : array) ((XMLItem const&) element).destroy();
        free((void*) array.elements);
        break;
      }
    }
  }





  inline bool validate_name_char (char ch) {
    return char_is_alpha_numeric(ch)
        || ch == '-'
        || ch == '_'
        || ch == '.';
  }

  inline bool check_for_comment (char const* str) {
    return str[0] == '!' && str[1] == '-' && str[2] == '-';
  }

  inline void parse_comment_body (XML const* xml, char const* str, size_t* offset) {
    while (str[*offset] != '\0') {
      if (str[*offset] == '-' && str[*offset + 1] == '-' && str[*offset + 2] == '>') {
        *offset += 3;
        return;
      } else ++ *offset;
    }

    xml->asset_error("Unexpected end of input while parsing comment");
  }

  inline u8_t parse_item_name (XMLItem& item, char const* str, size_t* offset) {
    if (check_for_comment(str + *offset)) {
      item.type = XMLType::Comment;
      
      *offset += 3;

      parse_comment_body(item.root, str, offset);

      return 2;
    } else if (str[*offset] == '?') {
      item.type = XMLType::ProcessingInstruction;
      ++ *offset;
    } else if (str[*offset] == '!') {
      ++ *offset;
      if (strncmp(str + *offset, "[CDATA[", 7) == 0) {
        item.type = XMLType::CDATA;
        item.name = { "CDATA" };

        *offset += 7;

        char const* cdata_start = str + *offset;
        size_t offset_start = *offset;

        while (str[*offset] != '\0') {
          if (str[*offset] == ']' && str[*offset + 1] == ']' && str[*offset + 2] == '>') {
            size_t cdata_length = *offset - offset_start;
            item.text = { cdata_start, cdata_length };
            
            *offset += 3;

            return 2;
          } else ++ *offset;
        }

        item.asset_error("Unexpected end of input while parsing CDATA value");
      } else {
        item.asset_assert(strncmp(str + *offset, "DOCTYPE", 7) == 0, "Unexpected delimiter, with '<!' tag opening expected either CDATA or DOCTYPE to follow");

        item.type = XMLType::DocumentType;
        item.name = { "DOCTYPE" };

        *offset += 7;

        while (char_is_whitespace(str[*offset])) ++ *offset;

        char const* doctype_start = str + *offset;
        size_t offset_start = *offset;

        while (str[*offset] != '\0') {
          if (str[*offset] == ']' && str[*offset + 1] == '>') {
            ++ *offset;
            
            size_t doctype_length = *offset - offset_start;
            ++ *offset;

            item.text = { doctype_start, doctype_length };
            
            return 2;
          } else ++ *offset;
        }

        item.asset_error("Unexpected end of input while parsing DOCTYPE value");
      }
    }

    char const* start = str + *offset;
    size_t offset_start = *offset;

    item.root->asset_assert(validate_name_char(*start), *offset, "Expected a name character [a-zA-Z0-9] or [-_.], not '%c'", *start);

    while (validate_name_char(str[*offset])) ++ *offset;

    size_t length = *offset - offset_start;

    item.name = { start, length };


    while (char_is_whitespace(str[*offset])) ++ *offset;

    if (str[*offset] == '>') {
      ++ *offset;
      return 1;
    } else if (str[*offset] == '/') {
      item.root->asset_assert(item.type != XMLType::ProcessingInstruction, *offset, "Expected processing instruction terminator '?' for element '%s'", item.name);
      ++ *offset;
      item.root->asset_assert(str[*offset] == '>', *offset, "Expected '>' to follow empty-tag terminator '/' for element '%s' but found '%c'", item.name, str[*offset]);
      ++ *offset;
      return 2;
    } else if (str[*offset] == '?') {
      item.root->asset_assert(item.type == XMLType::ProcessingInstruction, *offset, "Unexpected processing instruction terminator '?' for element '%s'", item.name);
      ++ *offset;
      item.root->asset_assert(str[*offset] == '>', *offset, "Expected '> to follow processing instruction terminator '?' for element '%s' but found '%c'", item.name, str[*offset]);
      ++ *offset;
      return 2;
    } else return 0;
  }

  inline u8_t parse_item_attribute (XMLItem& item, char const* str, size_t* offset) {
    char const* name_start = str + *offset;
    size_t name_offset_start = *offset;

    item.root->asset_assert(validate_name_char(str[*offset]), *offset, "Expected an attribute name character [a-zA-Z0-9] or [-_.], not '%c'", *name_start);

    while (validate_name_char(str[*offset])) ++ *offset;

    size_t name_length = *offset - name_offset_start;

    while (char_is_whitespace(str[*offset])) ++ *offset;

    item.root->asset_assert(str[*offset] == '=', *offset, "Expected an attribute value assignment operator '=', not '%c'", str[*offset]);

    ++ *offset;

    while (char_is_whitespace(str[*offset])) ++ *offset;

    item.set_attribute_unique(item.root->unescape_text_from_source(str, offset, true), name_start, name_length);

    while (char_is_whitespace(str[*offset])) ++ *offset;

    if (validate_name_char(str[*offset])) return 0;
    else if (str[*offset] == '>') {
      ++ *offset;
      return 1;
    } else if (str[*offset] == '/') {
      ++ *offset;
      item.root->asset_assert(str[*offset] == '>', *offset, "Expected '>' to follow empty-tag terminator '/' for element opening tag '%s' but found '%c'", item.name.value, str[*offset]);
      item.type = XMLType::Empty;
      ++ *offset;
      return 2;
    } else if (str[*offset] == '?') {
      item.root->asset_assert(item.type == XMLType::ProcessingInstruction, "Expected '>' or '/>' to end non-processing instruction type element opening tag '%s', not '?'", item.name.value);
      ++ *offset;
      item.root->asset_assert(str[*offset] == '>', *offset, "Expected '>' to follow processing instruction terminator '?' for element opening tag '%s' but found '%c'", item.name.value, str[*offset]);
      ++ *offset;
      return 2;
    } else item.root->asset_error("Unexpected character '%c' while parsing item attributes", str[*offset]);
  }

  inline bool parse_item_attributes (XMLItem& item, char const* str, size_t* offset) {
    u8_t result;
    do result = parse_item_attribute(item, str, offset);
    while (result == 0);

    return result == 1;
  }


  inline bool parse_item_header (XMLItem& item, char const* str, size_t* offset) {
    item.asset_assert(str[*offset] == '<', "Expected an opening brace for a tag '<'");
    ++ *offset;
    
    u8_t name_res = parse_item_name(item, str, offset);
    if (name_res == 0) return parse_item_attributes(item, str, offset);
    else return name_res == 1;
  }

  inline void parse_item_body (XMLItem& item, char const* str, size_t* offset) {
    while (char_is_whitespace(str[*offset])) ++ *offset;

    while (str[*offset] == '<' && check_for_comment(str + *offset + 1)) {
      *offset += 3;
      parse_comment_body(item.root, str, offset);
    }

    if (str[*offset] == '<') {
      while (str[*offset] != '\0') {
        if (str[*offset] == '<' && str[*offset + 1] == '/') return;
        else {
          XMLItem arr_item = XMLItem::from_str(item.root, str, offset);

          if (arr_item.type != XMLType::Comment) {
            item.type = XMLType::Array;
            item.array.append(arr_item);
          }

          while (char_is_whitespace(str[*offset])) ++ *offset;
        }
      }
    } else {
      item.asset_assert(str[*offset] != '\0', "Unexpected end of input while parsing item body");

      item.type = XMLType::Text;
      item.text = item.root->unescape_text_from_source(str, offset, false);
    }
  }

  inline void parse_item_footer (XMLItem& item, char const* str, size_t* offset) {
    item.root->asset_assert(str[*offset] == '<' && str[*offset + 1] == '/', "Expected closing tag");
    *offset += 2;

    item.root->asset_assert(strncmp(str + *offset, item.name.value, item.name.length) == 0, "Expected closing tag for element '%s'", item.name.value);

    *offset += item.name.length;

    item.root->asset_assert(str[*offset] == '>', "Expected end bracket '>' for closing tag, not '%c'", str[*offset]);
    
    ++ *offset;
  }



  XMLItem XMLItem::from_str (XML* root, char const* str, size_t* offset) {
    while (char_is_whitespace(str[*offset])) ++ *offset;

    XMLItem item = { root, *offset };

    try {
      if (parse_item_header(item, str, offset)) {
        parse_item_body(item, str, offset);
        parse_item_footer(item, str, offset);
      }
    } catch (Exception& exception) {
      item.destroy();
      throw exception;
    }

    return item;
  }


  inline bool stringify_item_header (XMLItem const* item, String* out_string) {
    out_string->append("<");

    if (item->type == XMLType::CDATA) {
      out_string->append("![CDATA[");
      out_string->append(item->text.value, item->text.length);
      out_string->append("]]>");
      return false;
    } else if (item->type == XMLType::DocumentType) {
      out_string->append("!DOCTYPE ");
      out_string->append(item->text.value, item->text.length);
      out_string->append(">");
      return false;
    } else {
      bool is_processing_instruction = item->type == XMLType::ProcessingInstruction;
      
      if (is_processing_instruction) out_string->append("?");

      out_string->append(item->name.value, item->name.length);

      for (auto [ i, attribute ] : item->attributes) {
        out_string->append(" ");

        out_string->append(attribute.name.value, attribute.name.length);

        out_string->append("=");

        XML::escape_text_to_source(&attribute.value, out_string, true);
      }

      bool is_empty = item->type == XMLType::Empty;

      if (is_empty) out_string->append("/");
      else if (is_processing_instruction) out_string->append("?");
      
      out_string->append(">");

      return !(is_empty || is_processing_instruction);
    }
  }

  inline void stringify_item_body (XMLItem const* item, String* out_string, size_t indent) {
    if (item->type == XMLType::Array) {
      out_string->append("\n");
      ++ indent;

      for (auto [ i, element ] : item->array) {
        XML::indent_source(out_string, indent);
        element.to_string(out_string, indent);
        out_string->append("\n");
      }

      -- indent;
      XML::indent_source(out_string, indent);
    } else {
      XML::escape_text_to_source(&item->text, out_string, false);
    }
  }

  inline void stringify_item_footer (XMLItem const* item, String* out_string) {
    out_string->fmt_append("</%s>", item->name.value);
  }


  void XMLItem::to_string (String* out_string, size_t indent) const {
    if (stringify_item_header(this, out_string)) {
      stringify_item_body(this, out_string, indent);

      stringify_item_footer(this, out_string);
    }
  }





  XMLAttribute* XMLItem::get_attribute_pointer (char const* name, size_t name_length) const {
    if (XMLType::validate_attribute_support(type)) {
      if (name_length == 0) name_length = strlen(name);

      for (auto [ i, attribute ] : attributes) {
        if (strncmp(name, attribute.name.value, name_length) == 0) return &attribute;
      }
    }

    return NULL;
  }

  XMLAttribute& XMLItem::get_attribute (char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);

    XMLAttribute* attribute = get_attribute_pointer(name, name_length);
    
    asset_assert(attribute != NULL, "Expected an attribute named '%.*s'", (s32_t) name_length, name);

    return *attribute;
  }


  void XMLItem::set_attribute (String value, char const* name, size_t name_length) {
    if (name_length == 0) name_length = strlen(name);

    if (!XMLType::validate_attribute_support(type)) {
      value.destroy();
      asset_error("Cannot create attribute '%.*s' for XMLItem of type %s", (s32_t) name_length, name, XMLType::name(type));
    }

    XMLAttribute* existing_attribute = get_attribute_pointer(name, name_length);

    if (existing_attribute != NULL) {
      existing_attribute->value.destroy();
      existing_attribute->value = value;
    } else {
      attributes.append({ { name, name_length }, value });
    }
  }

  void XMLItem::set_attribute_unique (String value, char const* name, size_t name_length) {
    if (name_length == 0) name_length = strlen(name);

    if (!XMLType::validate_attribute_support(type)) {
      value.destroy();
      asset_error("Cannot create attribute '%.*s' for XMLItem of type %s", (s32_t) name_length, name, XMLType::name(type));
    }

    XMLAttribute* existing_attribute = get_attribute_pointer(name, name_length);

    if (existing_attribute != NULL) {
      value.destroy();
      asset_error("Attribute with name '%.*s' already exists", (s32_t) name_length, name);
    }

    attributes.append({{ name, name_length }, value });
  }

  void XMLItem::unset_attribute (char const* name, size_t name_length) {
    if (XMLType::validate_attribute_support(type)) {
      if (name_length == 0) name_length = strlen(name);

      for (auto [ i, attribute ] : attributes) {
        if (strncmp(name, attribute.name.value, name_length) == 0) {
          attribute.destroy();
          attributes.remove(i);
          break;
        }
      }
    }
  }



  XMLArray& XMLItem::get_array () const {
    asset_assert(type == XMLType::Array, "Expected an Array, not %s", XMLType::name(type));
    return (XMLArray&) array;
  }
  
  String& XMLItem::get_text () const {
    asset_assert(type == XMLType::Text, "Expected Text, not %s", XMLType::name(type));
    return (String&) text;
  }
  
  String& XMLItem::get_cdata () const {
    asset_assert(type == XMLType::CDATA, "Expected CDATA, not %s", XMLType::name(type));
    return (String&) text;
  }
  
  String& XMLItem::get_doctype () const {
    asset_assert(type == XMLType::DocumentType, "Expected DOCTYPE, not %s", XMLType::name(type));
    return (String&) text;
  }
  

  void XMLItem::set_array (XMLArray arr) {
    destroy_value();
    type = XMLType::Array;
    array = arr;
  }
  
  void XMLItem::set_text (String txt) {
    destroy_value();
    type = XMLType::Text;
    text = txt;
  }
  
  void XMLItem::set_cdata (String txt) {
    destroy_value();
    type = XMLType::CDATA;
    text = txt;
  }
  
  void XMLItem::set_doctype (String txt) {
    destroy_value();
    type = XMLType::DocumentType;
    text = txt;
  }
  

  
  XMLItem* XMLItem::item_pointer (size_t index) const {
    if (type == XMLType::Array) return array.get_element(index);
    else asset_error("Expected an Array, not %s", XMLType::name(type));
  }


  XMLItem* XMLItem::nth_named_pointer (size_t n, char const* name, size_t name_length) const {
    if (type == XMLType::Array) {
      size_t x = 0;

      if (name_length == 0) name_length = strlen(name);

      for (auto [ i, item ] : array) {
        if (strncmp(name, item.name.value, name_length) == 0) {
          if (x == n) return &item;
          else ++ x;
        }
      }

      return NULL;
    } else asset_error("Expected an Array, not %s", XMLType::name(type));
  }

  XMLItem* XMLItem::first_named_pointer (char const* name, size_t name_length) const {
    return nth_named_pointer(0, name, name_length);
  }

  XMLItem* XMLItem::last_named_pointer (char const* name, size_t name_length) const {
    if (type == XMLType::Array) {
      if (name_length == 0) name_length = strlen(name);

      XMLItem* last = NULL;

      for (auto [ i, item ] : array) {
        if (strncmp(name, item.name.value, name_length) == 0) {
          last = &item;
        }
      }

      return last;
    } else asset_error("Expected an Array, not %s", XMLType::name(type));
  }

  size_t XMLItem::count_of_name (char const* name, size_t name_length) const {
    asset_assert(type == XMLType::Array, "Expected an Array");

    if (name_length == 0) name_length = strlen(name);

    size_t count = 0;
    
    for (auto [ i, item ] : array) {
      if (strncmp(item.name.value, name, name_length) == 0) ++ count;
    }

    return count;
  }


  XMLItem* XMLItem::nth_of_type_pointer (size_t n, u8_t seek_type) const {
    asset_assert(XMLType::validate(seek_type), "Cannot seek %s XML type", XMLType::name(seek_type));
    if (type == XMLType::Array) {
      size_t x = 0;

      for (auto [ i, item ] : array) {
        if (item.type == seek_type) {
          if (x == n) return &item;
          else ++ x;
        }
      }

      return NULL;
    } else asset_error("Expected an Array, not %s", XMLType::name(type));
  }

  XMLItem* XMLItem::first_of_type_pointer (u8_t seek_type) const {
    return nth_of_type_pointer(0, type);
  }

  XMLItem* XMLItem::last_of_type_pointer (u8_t seek_type) const {
    asset_assert(XMLType::validate(seek_type), "Cannot seek %s XML type", XMLType::name(seek_type));
    if (type == XMLType::Array) {
      XMLItem* last = NULL;

      for (auto [ i, item ] : array) {
        if (item.type == seek_type) {
          last = &item;
        }
      }

      return last;
    } else asset_error("Expected an Array, not %s", XMLType::name(type));
  }

  size_t XMLItem::count_of_type (u8_t seek_type) const {
    asset_assert(seek_type == XMLType::Array, "Expected an Array");

    size_t count = 0;
    
    for (auto [ i, item ] : array) {
      if (item.type == seek_type) ++ count;
    }

    return count;
  }



  XMLItem& XMLItem::item (size_t index) const {
    XMLItem* ptr = item_pointer(index);
    asset_assert(ptr != NULL, "Expected an item at index %zu, but count is only %zu", index, array.count);
    return *ptr;
  }


  XMLItem& XMLItem::nth_named (size_t n, char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);
    XMLItem* ptr = nth_named_pointer(n, name, name_length);
    asset_assert(ptr != NULL, "Expected at least %zu items named %.*s", n, (s32_t) name_length, name);
    return *ptr;
  }

  XMLItem& XMLItem::first_named (char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);
    XMLItem* ptr = first_named_pointer(name, name_length);
    asset_assert(ptr != NULL, "Expected an item named %.*s", (s32_t) name_length, name);
    return *ptr;
  }

  XMLItem& XMLItem::last_named (char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);
    XMLItem* ptr = last_named_pointer(name, name_length);
    asset_assert(ptr != NULL, "Expected an item named %.*s", (s32_t) name_length, name);
    return *ptr;
  }


  XMLItem& XMLItem::nth_of_type (size_t n, u8_t seek_type) const {
    XMLItem* ptr = nth_of_type_pointer(n, seek_type);
    asset_assert(ptr != NULL, "Expected at least %zu items with type %s", n, XMLType::name(seek_type));
    return *ptr;
  }

  XMLItem& XMLItem::first_of_type (u8_t seek_type) const {
    XMLItem* ptr = first_of_type_pointer(seek_type);
    asset_assert(ptr != NULL, "Expected an item with type %s", XMLType::name(seek_type));
    return *ptr;
  }

  XMLItem& XMLItem::last_of_type (u8_t seek_type) const {
    XMLItem* ptr = last_of_type_pointer(seek_type);
    asset_assert(ptr != NULL, "Expected an item with type %s", XMLType::name(seek_type));
    return *ptr;
  }
  


  void XMLItem::append_item (XMLItem const* item) {
    if (type == XMLType::Array) array.append(item);
    else {
      item->destroy();
      asset_error("Expected an Array, not %s", XMLType::name(type));
    }
  }
  
  void XMLItem::insert_item (size_t index, XMLItem const* item) {
    if (type == XMLType::Array) array.insert(index, item);
    else {
      item->destroy();
      asset_error("Expected an Array, not %s", XMLType::name(type));
    }
  }
  
  void XMLItem::remove_item (size_t index) {
    if (type == XMLType::Array) {
      XMLItem* item = array.get_element(index);
      
      if (item != NULL) {
        item->destroy();
        array.remove(index);
      }
    } else asset_error("Expected an Array, not %s", XMLType::name(type));
  }



  




  String XML::unescape_text_from_source (char const* str, size_t* offset, bool is_attribute) const {
    char opening_symbol = '\0';

    if (is_attribute) {
      opening_symbol = str[*offset];
      asset_assert(opening_symbol == '\'' || opening_symbol == '"', *offset, "Expected an ''' or '\"' character beginning an attribute value, not '%c'", opening_symbol);
      ++ *offset;
    }

    String value;

    try {
      while (str[*offset] != opening_symbol && str[*offset] != '\0') {
        char const* loc = str + *offset;

        if (*loc == '&') {
          if (str_cmp_caseless(loc, "&quot", 5) == 0) {
            value.append("\"", 1);
            *offset += 5;
          } else if (str_cmp_caseless(loc, "&apos", 5) == 0) {
            value.append("'", 1);
            *offset += 5;
          } else if (str_cmp_caseless(loc, "&lt", 3) == 0) {
            value.append("<", 1);
            *offset += 3;
          } else if (str_cmp_caseless(loc, "&gt", 3) == 0) {
            value.append(">", 1);
            *offset += 3;
          } else if (str_cmp_caseless(loc, "&amp", 4) == 0) {
            value.append("&", 1);
            *offset += 4;
          } else asset_error(*offset, "Unexpected escape sequence following &");
        } else if (*loc == '<') {
          if (check_for_comment(loc + 1)) {
            asset_assert(!is_attribute, "Comments are not allowed in this context");
            *offset += 3;
            parse_comment_body(this, str, offset);
          } else {
            asset_assert(!is_attribute, "Unescaped < not marking a comment is not allowed in this context");
            break;
          }
        } else {
          value.append(loc, 1);
          ++ *offset;
        }
      }
    } catch (Exception& exception) {
      value.destroy();
    }

    if (!is_attribute) opening_symbol = '<';

    asset_assert(str[*offset] == opening_symbol, "Expected a '%c' character to close text sequence, not '%c'", opening_symbol, str[*offset]);

    if (is_attribute) ++ *offset;

    return value;
  }


  void XML::escape_text_to_source (String const* in_string, String* out_string, bool is_attribute) {
    if (is_attribute) out_string->append("\"");

    for (size_t i = 0; i < in_string->length; i ++) {
      char const* curr_char = in_string->value + i;

      switch (*curr_char) {
        case '"': out_string->append("&quot"); break;
        case '\'': out_string->append("'"); break;
        case '<': out_string->append("&lt"); break;
        case '>': out_string->append("&gt"); break;
        case '&': out_string->append("&amp"); break;
        default: out_string->append(curr_char, 1); break;
      }
    }

    if (is_attribute) out_string->append("\"");
  }



  void XML::destroy () {
    if (origin != NULL) {
      free(origin);
      origin = NULL;
    }

    if (source != NULL) {
      free(source);
      source = NULL;
    }

    for (auto [ i, element ] : data) element.destroy();

    data.destroy();
  }


  inline void parse_xml (XML& xml) {
    try {
      size_t offset = 0;
      while (xml.source[offset] != '\0') {
        XMLItem item = XMLItem::from_str(&xml, xml.source, &offset);
        if (item.type != XMLType::Comment) xml.data.append(item);
      }
    } catch (Exception& exception) {
      xml.destroy();
      throw exception;
    }
  }


  XML XML::from_str (char const* origin, char const* source, size_t source_length) {
    XML xml { origin, source, source_length };

    parse_xml(xml);

    return xml;
  }

  XML XML::from_str_ex (char const* origin, char* source) {
    XML xml { str_clone(origin), source, XMLArray { } };

    parse_xml(xml);

    return xml;
  }

  XML XML::from_file (char const* origin) {
    auto [ data, length ] = load_file(origin);

    m_asset_assert(data != NULL, origin, "Failed to load source file");

    return from_str_ex(origin, (char*) data);
  }



  String XML::to_string () const {
    String out;

    for (auto [ i, element ] : data) {
      element.to_string(&out);
      out.append("\n");
    }

    return out;
  }

  bool XML::to_file (char const* path) const {
    String out = to_string();

    bool success = out.to_file(path);

    out.destroy();

    return success;
  }


  XMLItem* XML::get_pointer (size_t index) const {
    return data.get_element(index);
  }

  XMLItem& XML::get (size_t index) const {
    asset_assert(index < data.count, "Out of range access for XML root: %zu index, %zu count", index, data.count);
    return data.elements[index];
  }
  
  void XML::set (size_t index, XMLItem const* item) {
    XMLItem* existing_item = get_pointer(index);
    if (existing_item == NULL) {
      item->destroy();
      asset_error("Out of range access for XML root: %zu index, %zu count", index, data.count);
    } else {
      existing_item->destroy();
      *existing_item = *item;
    }
  }







  XMLItem* XML::nth_named_pointer (size_t n, char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);

    size_t x = 0;

    for (auto [ i, element ] : data) {
      if (strncmp(element.name.value, name, name_length) == 0) {
        if (x == n) return &element;
        else ++ x;
      }
    }

    return NULL;
  }

  XMLItem* XML::first_named_pointer (char const* name, size_t name_length) const {
    return nth_named_pointer(0, name, name_length);
  }

  XMLItem* XML::last_named_pointer (char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);

    XMLItem* last = NULL;

    for (auto [ i, element ] : data) {
      if (strncmp(element.name.value, name, name_length) == 0) {
        last = &element;
      }
    }

    return last;
  }

  size_t XML::count_of_name (char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);
    
    size_t count = 0;

    for (auto [ i, item ] : data) {
      if (strncmp(item.name.value, name, name_length) == 0) ++ count;
    }

    return count;
  }



  XMLItem* XML::nth_of_type_pointer (size_t n, u8_t type) const {
    size_t x = 0;

    for (auto [ i, element ] : data) {
      if (element.type == type) {
        if (x == n) return &element;
        else ++ x;
      }
    }

    return NULL;
  }

  XMLItem* XML::first_of_type_pointer (u8_t type) const {
    return nth_of_type_pointer(0, type);
  }

  XMLItem* XML::last_of_type_pointer (u8_t type) const {
    XMLItem* last = NULL;

    for (auto [ i, element ] : data) {
      if (element.type == type) {
        last = &element;
      }
    }

    return last;
  }

  size_t XML::count_of_type (u8_t type) const {
    size_t count = 0;
    
    for (auto [ i, item ] : data) {
      if (type == item.type) ++ count;
    }

    return count;
  }



  XMLItem& XML::nth_named (size_t n, char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);
    XMLItem* ptr = nth_named_pointer(n, name, name_length);
    asset_assert(ptr != NULL, "Expected at least %zu items named %.*s", n, (s32_t) name_length, name);
    return *ptr;
  }

  XMLItem& XML::first_named (char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);
    XMLItem* ptr = first_named_pointer(name, name_length);
    asset_assert(ptr != NULL, "Expected an item named %.*s", (s32_t) name_length, name);
    return *ptr;
  }

  XMLItem& XML::last_named (char const* name, size_t name_length) const {
    if (name_length == 0) name_length = strlen(name);
    XMLItem* ptr = last_named_pointer(name, name_length);
    asset_assert(ptr != NULL, "Expected an item named %.*s", (s32_t) name_length, name);
    return *ptr;
  }


  XMLItem& XML::nth_of_type (size_t n, u8_t type) const {
    XMLItem* ptr = nth_of_type_pointer(n, type);
    asset_assert(ptr != NULL, "Expected at least %zu items of type %s", n, XMLType::name(type));
    return *ptr;
  }

  XMLItem& XML::first_of_type (u8_t type) const {
    XMLItem* ptr = first_of_type_pointer(type);
    asset_assert(ptr != NULL, "Expected an item of type %s", XMLType::name(type));
    return *ptr;
  }

  XMLItem& XML::last_of_type (u8_t type) const {
    XMLItem* ptr = last_of_type_pointer(type);
    asset_assert(ptr != NULL, "Expected an item of type %s", XMLType::name(type));
    return *ptr;
  }





  pair_t<s32_t, s32_t> XML::get_line_column_info (size_t origin_offset) const {
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