#ifndef XML_H
#define XML_H

#include "cstd.hh"
#include "util.hh"
#include "Array.hh"
#include "String.hh"



namespace mod {
  namespace XMLType {
    enum: u8_t {
      Text,
      Array,
      ProcessingInstruction,
      Empty,
      CDATA,
      DocumentType,

      Comment,

      total_type_count,

      Invalid = (u8_t) -1
    };

    static constexpr char const* names [total_type_count] = {
      "Text",
      "Array",
      "ProcessingInstruction",
      "Empty",
      "CDATA",
      "DocumentType",

      "Comment"
    };

    /* Get the name of an XMLType as a str */
    static constexpr char const* name (u8_t type) {
      if (type < total_type_count) return names[type];
      else return "Invalid";
    }

    /* Get an XMLType from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t type = 0; type < total_type_count; type ++) {
        if ((max_length == SIZE_MAX || strlen(names[type]) == max_length) && str_cmp_caseless(name, names[type], max_length) == 0) return type;
      }

      return Invalid;
    }

    /* Determine if a value is a valid XMLType */
    static constexpr bool validate (u8_t type) {
      return type < Comment;
    }

    /* Determine if a value is a valid XMLType which supports attributes */
    static constexpr bool validate_attribute_support (u8_t type) {
      return type < CDATA;
    }
  };





  struct XML;
  struct XMLItem;

  using XMLArray = Array<XMLItem>;

  struct XMLAttribute {
    String name;
    String value;

    void destroy () {
      name.destroy();
      value.destroy();
    }
  };

  using XMLAttributeArray = Array<XMLAttribute>;





  struct XMLItem {
    XML* root = NULL;
    size_t origin_offset = 0;

    String name;

    XMLAttributeArray attributes;
    
    u8_t type = XMLType::Empty;
    union {
      String text;
      XMLArray array = { };
    };



    /* Create a new uninitialized XMLItem with an optional root and offset from its origin */
    XMLItem (XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    { }

    /* Create a new empty XMLItem with a name and type, no attributes, and an optional root and offset from its origin.
     * Takes ownership of the name String */
    XMLItem (String in_name, u8_t in_type, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , type(in_type)
    { m_assert(XMLType::validate(type), "Cannot create XMLItem with %s type (%" PRIu8 ")", XMLType::name(type), type); }


    /* Create a new Text XMLItem with a name, no attributes, and an optional root and offset from its origin.
     * Takes ownership of the name and text Strings */
    XMLItem (String in_name, String in_text, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , type(XMLType::Text)
    , text(in_text)
    { }

    /* Create a new Text XMLItem with a name and no attributes by copying from a str, with an optional root and offset from its origin.
     * Takes ownership of the name String */
    XMLItem (String in_name, char const* in_text_str, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , type(XMLType::Text)
    , text(in_text_str)
    { }

    /* Create a new Array XMLItem with a name, no attributes, and an optional root and offset from its origin.
     * Takes ownership of the name String and the XMLArray */
    XMLItem (String in_name, XMLArray in_array, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , type(XMLType::Array)
    , array(in_array)
    { }

    /* Create a new empty XMLItem with a name and type, attributes, and an optional root and offset from its origin.
     * Takes ownership of the name String */
    XMLItem (String in_name, u8_t in_type, XMLAttributeArray in_attributes, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , attributes(in_attributes)
    , type(in_type)
    { m_assert(XMLType::validate(type), "Cannot create XMLItem with %s type (%" PRIu8 ")", XMLType::name(type), type); }


    /* Create a new Text XMLItem with a name, attributes, and an optional root and offset from its origin.
     * Takes ownership of the name and text Strings as well as the XMLAttributeArray */
    XMLItem (String in_name, String in_text, XMLAttributeArray in_attributes, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , attributes(in_attributes)
    , type(XMLType::Text)
    , text(in_text)
    { }

    /* Create a new Text XMLItem with a name and attributes by copying from a str, with an optional root and offset from its origin.
     * Takes ownership of the name String and the XMLAttributeArray */
    XMLItem (String in_name, char const* in_text_str, XMLAttributeArray in_attributes, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , attributes(in_attributes)
    , type(XMLType::Text)
    , text(in_text_str)
    { }

    /* Create a new Array XMLItem with a name, attributes, and an optional root and offset from its origin.
     * Takes ownership of the name String, XMLArray and XMLAttributeArray  */
    XMLItem (String in_name, XMLArray in_array, XMLAttributeArray in_attributes, XML* in_root = NULL, size_t in_origin_offset = 0)
    : root(in_root)
    , origin_offset(in_origin_offset)
    , name(in_name)
    , attributes(in_attributes)
    , type(XMLType::Array)
    , array(in_array)
    { }


    /* Create a special DOCTYPE type XMLItem */
    static XMLItem doctype (String in_text, XML* in_root = NULL, size_t in_origin_offset = 0) {
      XMLItem out = { "DOCTYPE", XMLType::DocumentType, in_root, in_origin_offset };
      out.text = in_text;
      return out;
    }

    /* Create a special CDATA type XMLItem */
    static XMLItem cdata (String in_text, XML* in_root = NULL, size_t in_origin_offset = 0) {
      XMLItem out = { "CDATA", XMLType::CDATA, in_root, in_origin_offset };
      out.text = in_text;
      return out;
    }

    /* Create a special processing instruction type XMLItem */
    static XMLItem processing_instruction (String in_name, XMLAttributeArray in_attributes, XML* in_root = NULL, size_t in_origin_offset = 0) {
      return { in_name, XMLType::ProcessingInstruction, in_attributes, in_root, in_origin_offset };
    }



    /* Destroy the value of an XMLItem, making it an Empty */
    ENGINE_API void destroy_value ();

    /* Destroy an XMLItem, its attributes, and its descendant items or text */
    ENGINE_API void destroy ();

    /* Destroy an XMLItem, its attributes, and its descendant items or text, without modifying any of the structure */
    ENGINE_API void destroy () const;



    /* Decode any generic XMLItem from an offset within a textual representation
     * Increments `offset` by the amount of `str` consumed */
    ENGINE_API static XMLItem from_str (XML* root, char const* str, size_t* offset);


    /* Encode any generic XMLItem into an output source String, with optional indentation */
    ENGINE_API void to_string (String* out_string, size_t indent = 0) const;



    /* Get a pointer to an XMLAttribute in an XMLItem by name.
     * Returns NULL if no XMLAttribute with the given name exists, or if the XMLItem type does not support attributes */
    ENGINE_API XMLAttribute* get_attribute_pointer (char const* name, size_t name_length = 0) const;

    /* Get a reference to an XMLAttribute in an XMLItem by name.
     * Throws if an item with the given name does not exist, or if the XMLItem type does not support attributes */
    ENGINE_API XMLAttribute& get_attribute (char const* name, size_t name_length = 0) const;


    /* Set the value of an XMLItem's attribute by name.
     * Creates a new attribute if one does not exist with the given name.
     * Takes ownership of the value String.
     * Throws if the XMLItem's type does not support attributes, and destroys the value String immediately */
    ENGINE_API void set_attribute (String value, char const* name, size_t name_length = 0);

    /* Set the value of an XMLItem's attribute by name.
     * Throws if an item with the given name already exists ,
     * or if the XMLItem's type does not support attributes
     * Takes ownership of the value String, and destroys it immediately on error */
    ENGINE_API void set_attribute_unique (String value, char const* name, size_t name_length = 0);


    /* Remove an XMLItem's attribute by name.
     * Does nothing if no attribute with the given name is found or if the XMLItem type does not support attributes */
    ENGINE_API void unset_attribute (char const* name, size_t name_length = 0);



    /* Assume an XMLItem is an array and get a reference to its list.
     * Throws if the XMLItem is not actually an array */
    ENGINE_API XMLArray& get_array () const;

    /* Assume an XMLItem is text and get a reference to its body.
     * Throws if the XMLItem is not actually text */
    ENGINE_API String& get_text () const;
    
    /* Assume an XMLItem is CDATA and get a reference to its body.
     * Throws if the XMLItem is not actually CDATA */
    ENGINE_API String& get_cdata () const;
    
    /* Assume an XMLItem is a DOCTYPE and get a reference to its body.
     * Throws if the XMLItem is not actually a DOCTYPE */
    ENGINE_API String& get_doctype () const;


    /* Set the value of an XMLItem to an array and destroy its old value.
     * Takes ownership of the XMLArray provided */
    ENGINE_API void set_array (XMLArray arr);

    /* Set the value of an XMLItem to text and destroy its old value.
     * Takes ownership of the String provided */
    ENGINE_API void set_text (String txt);

    /* Set the value of an XMLItem to CDATA and destroy its old value.
     * Takes ownership of the String provided */
    ENGINE_API void set_cdata (String txt);

    /* Set the value of an XMLItem to DOCTYPE and destroy its old value.
     * Takes ownership of the String provided */
    ENGINE_API void set_doctype (String txt);


    /* Assume an XMLItem is an array and get a pointer to a subitem at the given index in its list.
     * Throws if the XMLItem is not actually an array.
     * Returns NULL if the index is out of range */
    ENGINE_API XMLItem* item_pointer (size_t index) const;

    /* Assume an XMLItem is an array and get a pointer to the nth subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array.
     * Returns NULL if n is out of range */
    ENGINE_API XMLItem* nth_named_pointer (size_t n, char const* name, size_t name_length = 0) const;

    /* Assume an XMLItem is an array and get a pointer to the first subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array.
     * Returns NULL if there is no item with the given name */
    ENGINE_API XMLItem* first_named_pointer (char const* name, size_t name_length = 0) const;

    /* Assume an XMLItem is an array and get a pointer to the first subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array.
     * Returns NULL if there is no item with the given name */
    ENGINE_API XMLItem* last_named_pointer (char const* name, size_t name_length = 0) const;

    /* Assume an XMLItem is an array and get the count of subitems with a given name.
     * Throws if the XMLItem is not actually an array */
    ENGINE_API size_t count_of_name (char const* name, size_t name_length = 0) const;


    /* Assume an XMLItem is an array and get a pointer to the nth subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array, or if the given type is invalid
     * Returns NULL if n is out of range */
    ENGINE_API XMLItem* nth_of_type_pointer (size_t n, u8_t type) const;

    /* Assume an XMLItem is an array and get a pointer to the first subitem with the given type in its list.
     * Throws if the XMLItem is not actually an array, or if the given type is invalid
     * Returns NULL if there is no item with the given type */
    ENGINE_API XMLItem* first_of_type_pointer (u8_t type) const;

    /* Assume an XMLItem is an array and get a pointer to the first subitem with the given type in its list.
     * Throws if the XMLItem is not actually an array, or if the given type is invalid
     * Returns NULL if there is no item with the given type */
    ENGINE_API XMLItem* last_of_type_pointer (u8_t type) const;

    /* Assume an XMLItem is an array and get the count of subitems with a given type.
     * Throws if the XMLItem is not actually an array */
    ENGINE_API size_t count_of_type (u8_t type) const;


    /* Assume an XMLItem is an array and get reference to a subitem at the given index in its list.
     * Throws if the XMLItem is not actually an array,
     * or if the index is out of range */
    ENGINE_API XMLItem& item (size_t index) const;

    /* Assume an XMLItem is an array and get reference to the nth subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array,
     * or if n is out of range */
    ENGINE_API XMLItem& nth_named (size_t n, char const* name, size_t name_length = 0) const;

    /* Assume an XMLItem is an array and get reference to the first subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array,
     * or if there is no item with the given name */
    ENGINE_API XMLItem& first_named (char const* name, size_t name_length = 0) const;

    /* Assume an XMLItem is an array and get reference to the first subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array,
     * or if there is no item with the given name */
    ENGINE_API XMLItem& last_named (char const* name, size_t name_length = 0) const;


    /* Assume an XMLItem is an array and get reference to the nth subitem with the given name in its list.
     * Throws if the XMLItem is not actually an array,
     * if the given type is invalid,
     * or if n is out of range */
    ENGINE_API XMLItem& nth_of_type (size_t n, u8_t type) const;

    /* Assume an XMLItem is an array and get reference to the first subitem with the given type in its list.
     * Throws if the XMLItem is not actually an array,
     * if the given type is invalid,
     * or if there is no item with the given type */
    ENGINE_API XMLItem& first_of_type (u8_t type) const;

    /* Assume an XMLItem is an array and get reference to the first subitem with the given type in its list.
     * Throws if the XMLItem is not actually an array,
     * if the given type is invalid,
     * or if there is no item with the given type */
    ENGINE_API XMLItem& last_of_type (u8_t type) const;


    /* Assume an XMLItem is an array and add a subitem to the end of its list.
     * Takes ownership of the subitem.
     * Throws if the XMLItem is not actually an array, and destroys the subitem immediately */
    ENGINE_API void append_item (XMLItem const* item);

    /* Assume an XMLItem is an array and add a subitem to the end of its list.
     * Takes ownership of the subitem.
     * Throws if the XMLItem is not actually an array, and destroys the subitem immediately */
    void append_item (XMLItem const& item) {
      return append_item(&item);
    }

    /* Assume an XMLItem is an array and insert a subitem at the given index in its list.
     * Takes ownership of the subitem.
     * Throws if the XMLItem is not actually an array, and destroys the subitem immediately */
    ENGINE_API void insert_item (size_t index, XMLItem const* item);
    
    /* Assume an XMLItem is an array and insert a subitem at the given index in its list.
     * Takes ownership of the subitem.
     * Throws if the XMLItem is not actually an array, and destroys the subitem immediately */
    void insert_item(size_t index, XMLItem const& item) {
      return insert_item(index, &item);
    }

    /* Assume an XMLItem is an array and remove a subitem at the given index in its list.
     * Throws if the XMLItem is not actually an array */
    ENGINE_API void remove_item (size_t index);

    /* Assume an XMLItem is an array and add a named value to the end of its list.
     * Takes ownership of the name and the value.
     * Throws if the XMLItem is not actually an array, and destroys the inputs immediately */
    template <typename T> void append_value (String new_name, T value, size_t new_origin_offset = 0) {
      return append_item(XMLItem { new_name, value, root, new_origin_offset == 0? origin_offset : new_origin_offset });
    }

    /* Assume an XMLItem is an array and insert a named value at the given index in its list.
     * Takes ownership of the name and the value.
     * Throws if the XMLItem is not actually an array, and destroys the inputs immediately */
    template <typename T> void insert_value (size_t index, String new_name, T value, size_t new_origin_offset = 0) {
      return insert_item(index, XMLItem { new_name, value, root, new_origin_offset == 0? origin_offset : new_origin_offset });
    }



    /* Throw an exception using the location information of this item along with its root */
    template <typename ... A> NORETURN void asset_error (char const* fmt, A ... args) const {
      if (root != NULL) {
        root->asset_error(origin_offset != 0? origin_offset + 1 : 0, fmt, args...);
      } else {
        m_asset_error(origin_offset != 0? origin_offset + 1 : 0, fmt, args...);
      }
    }

    /* Throw an exception using the location information of this item along with its root, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, char const* fmt, A ... args) const {
      if (!cond) asset_error(fmt, args...);
    }
  };







  struct XML {
    char* origin;
    char* source;
    XMLArray data;


    /* Create a new XML root and optionally initialize its origin and source */
    XML (char const* in_origin = NULL, char const* in_source = NULL, size_t in_source_length = 0)
    : origin(str_clone(in_origin))
    , source(str_clone(in_source, in_source_length))
    { }

    /* Create a new XML root by explicitly initializing all members */
    XML (char* in_origin, char* in_source, XMLArray const& in_data)
    : origin(in_origin)
    , source(in_source)
    , data(in_data)
    { }

    /* Create a new XML root from an XMLItem, and optional origin and source.
     * Panics if the XMLItem is not of type Array */
    XML (XMLItem& in_item, char const* in_origin = NULL, char const* in_source = NULL, size_t in_source_length = 0)
    : origin(str_clone(in_origin))
    , source(str_clone(in_source, in_source_length))
    {
      m_assert(in_item.type == XMLType::Array, "Expected an XML Array, not %s", XMLType::name(in_item.type));

      data.copy(in_item.array);

      in_item.destroy();
    }


    /* Clean up an XML root and its descendents */
    ENGINE_API void destroy ();



    /* Create an XML root from a source str */
    ENGINE_API static XML from_str (char const* origin, char const* source, size_t source_length = 0);

    /* Create an XML root by taking ownership of a source str */
    ENGINE_API static XML from_str_ex (char const* origin, char* source);

    
    /* Create an XML root from a source file */
    ENGINE_API static XML from_file (char const* origin);



    /* Get an ArrayIterator representing the beginning of an XML root */
    ArrayIterator<XMLItem> begin () const {
      return data.begin();
    }

    /* Get an ArrayIterator representing the end of an XML root */
    ArrayIterator<XMLItem> end () const {
      return data.end();
    }
    

    
    /* Get a pointer to an element in an XML root, at the given index
     * Returns NULL if the index is out of range */
    ENGINE_API XMLItem* get_pointer (size_t index) const;

    /* Get a reference to an element in an XML root, at the given index
     * Throws if the index is out of range */
    ENGINE_API XMLItem& get (size_t index) const;


    /* Set the value of an element in an XML root, at the given index.
     * Takes ownership of the value.
     * Throws if the index is out of range, and immediately destroys the value */
    ENGINE_API void set (size_t index, XMLItem const* item);

    /* Set the value of an element in an XML root, at the given index.
     * Takes ownership of the value.
     * Throws if the index is out of range, and immediately destroys the value */
    void set (size_t index, XMLItem const& item) {
      return set(index, &item);
    }

    /* Append an element to the end of an XML root.
     * Takes ownership of the element value */
    void append (XMLItem const* item) {
      return data.append(item);
    }

    /* Append an element to the end of an XML root.
     * Takes ownership of the element value */
    void append (XMLItem const& item) {
      return data.append(item);
    }

    /* Append an element to the given index of an XML root.
     * Takes ownership of the element value */
    void append (size_t index, XMLItem* item) {
      return data.insert(index, item);
    }

    /* Append an element to the given index of an XML root.
     * Takes ownership of the element value */
    void append (size_t index, XMLItem& item) {
      return data.insert(index, item);
    }

    /* Remove an element at the given index of an XML root.
     * Does nothing if the index is out of range */
    void remove (size_t index) {
      return data.remove(index);
    }



    /* Get a pointer to the nth element in an XML root matching the given name.
     * Returns NULL if no element with the given name is found or if n is out of range */
    ENGINE_API XMLItem* nth_named_pointer (size_t n, char const* name, size_t name_length = 0) const;

    /* Get a pointer to the first element in an XML root matching the given name.
     * Returns NULL if no element with the given name is found */
    ENGINE_API XMLItem* first_named_pointer (char const* name, size_t name_length = 0) const;

    /* Get a pointer to the last element in an XML root matching the given name.
     * Returns NULL if no element with the given name is found */
    ENGINE_API XMLItem* last_named_pointer (char const* name, size_t name_length = 0) const;

    /* Get the count of items in an XML root matching the given name */
    ENGINE_API size_t count_of_name (char const* name, size_t name_length = 0) const;


    
    /* Get a pointer to the nth element in an XML root matching the given type.
     * Returns NULL if no element with the given type is found or if n is out of range */
    ENGINE_API XMLItem* nth_of_type_pointer (size_t n, u8_t type) const;

    /* Get a pointer to the first element in an XML root matching the given type.
     * Returns NULL if no element with the given type is found */
    ENGINE_API XMLItem* first_of_type_pointer (u8_t type) const;

    /* Get a pointer to the last element in an XML root matching the given type.
     * Returns NULL if no element with the given type is found */
    ENGINE_API XMLItem* last_of_type_pointer (u8_t type) const;

    /* Get the count of items in an XML root matching the given type */
    ENGINE_API size_t count_of_type (u8_t type) const;



    /* Get a reference to the nth element in an XML root matching the given name.
     * Throws if no element with the given name is found or if n is out of range */
    ENGINE_API XMLItem& nth_named (size_t n, char const* name, size_t name_length = 0) const;

    /* Get a reference to the first element in an XML root matching the given name.
     * Throws if no element with the given name is found */
    ENGINE_API XMLItem& first_named (char const* name, size_t name_length = 0) const;

    /* Get a reference to the last element in an XML root matching the given name.
     * Throws if no element with the given name is found */
    ENGINE_API XMLItem& last_named (char const* name, size_t name_length = 0) const;

    
    /* Get a reference to the nth element in an XML root matching the given type.
     * Throws if no element with the given type is found or if n is out of range */
    ENGINE_API XMLItem& nth_of_type (size_t n, u8_t type) const;

    /* Get a reference to the first element in an XML root matching the given type.
     * Throws if no element with the given type is found */
    ENGINE_API XMLItem& first_of_type (u8_t type) const;

    /* Get a reference to the last element in an XML root matching the given type.
     * Throws if no element with the given type is found */
    ENGINE_API XMLItem& last_of_type (u8_t type) const;



    /* Create a source String from an XML root */
    ENGINE_API String to_string () const;

    /* Create a source String version of an XML root and save that string as a file at the given path.
     * Returns true if the file was saved successfully */
    ENGINE_API bool to_file (char const* path) const;
    


    ENGINE_API String unescape_text_from_source (char const* str, size_t* offset, bool is_attribute) const;


    ENGINE_API static void escape_text_to_source (String const* in_string, String* out_string, bool is_attribute);

    /* Add a series of double spaces to an output String, depending on the level of `indent` */
    static void indent_source (String* out_string, size_t indent) {
      for (size_t i = 0; i < indent; i ++) out_string->append("  ");
    }


    /* Translate a XMLItem's origin_offset to a line and column number from its source */
    ENGINE_API pair_t<s32_t, s32_t> get_line_column_info (size_t origin_offset) const;

    /* Throw an asset exception associated with a JSON's origin, after looking up line and column information from a XMLItem's origin_offset */
    template <typename ... A> NORETURN void asset_error (size_t origin_offset, char const* fmt, A ... args) const {
      pair_t lc = get_line_column_info(origin_offset);

      m_asset_error(origin, lc.a, lc.b, fmt, args...);
    }

    /* Throw an asset exception associated with a JSON's origin, after looking up line and column information from a XMLItem's origin_offset, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, size_t origin_offset, char const* fmt, A ... args) const {
      if (!cond) {
        pair_t lc = get_line_column_info(origin_offset);

        m_asset_error(origin, lc.a, lc.b, fmt, args...);
      }
    }

    /* Throw an asset exception associated with an XML's origin */
    template <typename ... A> NORETURN void asset_error (char const* fmt, A ... args) const {
      m_asset_error(origin, -1, -1, fmt, args...);
    }

    /* Throw an asset exception associated with an XML's origin, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, char const* fmt, A ... args) const {
      m_asset_assert(cond, origin, -1, -1, fmt, args...);
    }
  };
}

#endif