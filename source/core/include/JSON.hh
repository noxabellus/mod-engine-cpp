#ifndef JSON_H
#define JSON_H


#include "cstd.hh"

#include "Array.hh"
#include "String.hh"
#include "Exception.hh"


namespace mod {
  namespace JSONType {
    enum: s8_t {
      Invalid = -1,
      Boolean = 0,
      Number,
      String,
      Array,
      Object
    };

    /* Get the name of a JSONType as a str */
    static constexpr char const* name (u8_t type) {
      switch (type) {
        case Boolean: return "Boolean";
        case Number: return "Number";
        case String: return "String";
        case Array: return "Array";
        case Object: return "Object";
        default: return "Invalid";
      }
    }

    /* Get a JSONType from its name in str form */
    static s8_t from_name (char const* str, size_t max_length = SIZE_MAX) {
      if (str_cmp_caseless(str, "boolean", max_length) == 0) return Boolean;
      else if (str_cmp_caseless(str, "number", max_length) == 0) return Number;
      else if (str_cmp_caseless(str, "string", max_length) == 0) return String;
      else if (str_cmp_caseless(str, "array", max_length) == 0) return Array;
      else if (str_cmp_caseless(str, "object", max_length) == 0) return Object;
      else return Invalid;
    }

    /* Determine if a value is a valid JSONType */
    static constexpr bool validate (u8_t type) {
      return type >= Boolean
          && type <= Object;
    }
  }

  struct JSONItem;
  
  struct JSON;

  using JSONArray = Array<JSONItem>;
  
  struct JSONObject {
    Array<String> keys;
    Array<JSONItem> items;


    /* Create a new zero-initialized JSONObject */
    JSONObject () { };


    /* Destroy a JSONObject and clean up its arrays */
    ENGINE_API void destroy ();


    /* Get the index of a key within a JSONObject.
     * Returns -1 if no key matching the input is found */
    ENGINE_API s64_t get_index (char const* key_value, size_t key_length = 0) const;

    /* Get the key matching the input within a JSONObject.
     * Returns NULL if no key matching the input is found */
    ENGINE_API String* get_key (char const* key_value, size_t key_length = 0) const;

    /* Get a pointer to the value associated with a key in a JSONObject.
     * Returns NULL if no key matching the input is found */
    ENGINE_API JSONItem* get (char const* key_value, size_t key_length = 0) const;


    /* Copy a JSONItem into a JSONObject, overwriting existing JSONItem or creating a new entry where necessary.
     * Returns the index of the key/item pair */
    ENGINE_API size_t set (JSONItem const* item, char const* key_value, size_t key_length = 0);

    
    /* Copy a JSONItem into a JSONObject, by reference, overwriting existing JSONItem or creating a new entry where necessary.
     * Returns the index of the key/item pair */
    size_t set (JSONItem const& item, char const* key_value, size_t key_length = 0) {
      return set(&item, key_value, key_length);
    }

    
    /* Copy a JSONItem into a JSONObject, if none exists at the given key.
     * Returns the index of the key/item pair, or -1 if an item already existed and it was not set */
    ENGINE_API s64_t set_unique (JSONItem const* item, char const* key_value, size_t key_length = 0);
    
    /* Copy a JSONItem into a JSONObject, by reference, if none exists at the given key.
     * Returns the index of the key/item pair, or -1 if an item already existed and it was not set */
    s64_t set_unique (JSONItem const& item, char const* key_value, size_t key_length = 0) {
      return set_unique(&item, key_value, key_length);
    }


    /* Copy a JSONItem into a JSONObject, if none exists at the given key.
     * Takes ownership of the given key if the item is set.
     * Returns the index of the key/item pair, or -1 if an item already existed and it was not set */
    ENGINE_API s64_t set_unique_string_key (JSONItem const* item, String key);
    
    /* Copy a JSONItem into a JSONObject, by reference, if none exists at the given key.
     * Takes ownership of the given key if the item is set.
     * Returns the index of the key/item pair, or -1 if an item already existed and it was not set */
    s64_t set_unique_string_key (JSONItem const& item, String key) {
      return set_unique_string_key(&item, key);
    }


    /* Copy a value into a JSONObject, overwriting existing JSONItem or creating a new entry where necessary.
     * Returns the index of the key/item pair */
    template <typename T> size_t set (T value, char const* key_value, JSON* root = NULL, size_t value_origin_offset = -1) {
      return set(JSONItem { value, root, value_origin_offset }, key_value, 0);
    }

    /* Copy a value into a JSONObject, overwriting existing JSONItem or creating a new entry where necessary.
     * Returns the index of the key/item pair */
    template <typename T> size_t set (T value, char const* key_value, size_t key_length, JSON* root = NULL, size_t value_origin_offset = -1) {
      return set(JSONItem { value, root, value_origin_offset }, key_value, key_length);
    }



    /* Remove a JSONItem associated with a key from a JSONObject */
    ENGINE_API void remove (char const* key_value, size_t key_length = 0);
  };

  struct JSONItem {
    JSON* root = NULL;
    size_t origin_offset = 0;
    u8_t type = JSONType::Invalid;
    union {
      bool boolean;
      f64_t number;
      String string;
      JSONArray array;
      JSONObject object = { };
    };


    /* Create a new uninitialized JSONItem with an root and offset from its origin */
    JSONItem (JSON* new_root = NULL, size_t new_origin_offset = 0)
    : root(new_root)
    , origin_offset(new_origin_offset)
    { }

    /* Create a new Boolean JSONItem with an optional root and offset from its origin */
    JSONItem (bool new_boolean, JSON* new_root = NULL, size_t new_origin_offset = 0)
    : root(new_root)
    , origin_offset(new_origin_offset)
    , type(JSONType::Boolean)
    , boolean(new_boolean)
    { }

    /* Create a new Number JSONItem with an optional root and offset from its origin */
    JSONItem (f64_t new_number, JSON* new_root = NULL, size_t new_origin_offset = 0)
    : root(new_root)
    , origin_offset(new_origin_offset)
    , type(JSONType::Number)
    , number(new_number)
    { }

    /* Create a new String JSONItem with an optional root and offset from its origin */
    JSONItem (String new_string, JSON* new_root = NULL, size_t new_origin_offset = 0)
    : root(new_root)
    , origin_offset(new_origin_offset)
    , type(JSONType::String)
    , string(new_string)
    { }

    /* Create a new Array JSONItem with an optional root and offset from its origin */
    JSONItem (JSONArray new_array, JSON* new_root = NULL, size_t new_origin_offset = 0)
    : root(new_root)
    , origin_offset(new_origin_offset)
    , type(JSONType::Array)
    , array(new_array)
    { }

    /* Create a new Object JSONItem with an optional root and offset from its origin */
    JSONItem (JSONObject new_object, JSON* new_root = NULL, size_t new_origin_offset = 0)
    : root(new_root)
    , origin_offset(new_origin_offset)
    , type(JSONType::Object)
    , object(new_object)
    { }

    /* Create a new JSONItem from a type identifier with an optional root and offset from its origin */
    JSONItem (u8_t new_type, JSON* new_root = NULL, size_t new_origin_offset = 0)
    : root(new_root)
    , origin_offset(new_origin_offset)
    , type(new_type)
    {
      switch (type) {
        case JSONType::Boolean: boolean = false; break;
        case JSONType::Number: number = 0.0; break;
        case JSONType::String:
        case JSONType::Array:
        case JSONType::Object: break;
        default: m_error("Cannot create JSONItem with Invalid type");
      }
    }


    /* Decode any generic JSONItem from an offset within a textual representation. Increments offset by the amount of str consumed */
    static ENGINE_API JSONItem from_str (JSON* origin, char const* str, size_t* offset);



    /* Destroy a JSONItem and clean up its heap allocation if its type has one */
    ENGINE_API void destroy ();



    /* Assume a JSONItem is a JSONObject and get a reference to a subitem associated with a particular key.
     * Throws if no key matching the input is found, or if the active item is not actually a JSONObject */
    JSONItem& operator [] (char const* key_value) const {
      JSONItem* item = get_object_item(key_value);
      asset_assert(item != NULL, "Could not find item with key '%s'", key_value);
      return *item;
    }

    /* Assume a JSONItem is a JSONArray and get a reference to a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    JSONItem& operator [] (size_t index) const {
      JSONItem* item = get_array_item(index);
      asset_assert(item != NULL, "Out of range access for JSONArray: index %zu, count %zu", index, array.count);
      return *item;
    }



    /* Overwrite a JSONItem's value with a Boolean, changing its type and destroying the old value where necessary */
    void set (bool value) {
      if (type != JSONType::Boolean) destroy();
      type = JSONType::Boolean;
      boolean = value;
    }

    /* Overwrite a JSONItem's value with a Number, changing its type and destroying the old value where necessary */
    void set (f64_t value) {
      if (type != JSONType::Number) destroy();
      type = JSONType::Number;
      number = value;
    }

    /* Overwrite a JSONItem's value with a String, changing its type and destroying the old value where necessary */
    void set (String value) {
      if (type != JSONType::String) destroy();
      type = JSONType::String;
      string = value;
    }

    /* Overwrite a JSONItem's value with an Array, changing its type and destroying the old value where necessary */
    void set (JSONArray value) {
      if (type != JSONType::Array) destroy();
      type = JSONType::Array;
      array = value;
    }

    /* Overwrite a JSONItem's value with an Object, changing its type and destroying the old value where necessary */
    void set (JSONObject value) {
      if (type != JSONType::Object) destroy();
      type = JSONType::Object;
      object = value;
    }


    /* Assume a JSONItem is a Boolean and get a reference to its value.
     * Throws if the item is not actually a boolean */
    bool& get_boolean () const {
      asset_assert(type == JSONType::Boolean, "Expected a Boolean, not %s", JSONType::name(type));
      return (bool&) boolean;
    }

    /* Assume a JSONItem is a Number and get a reference to its value.
     * Throws if the item is not actually a number */
    f64_t& get_number () const{
      asset_assert(type == JSONType::Number, "Expected a Number, not %s", JSONType::name(type));
      return (f64_t&) number;
    }

    /* Assume a JSONItem is a String and get a reference to its value.
     * Throws if the item is not actually a string */
    String& get_string () const{
      asset_assert(type == JSONType::String, "Expected a String, not %s", JSONType::name(type));
      return (String&) string;
    }

    /* Assume a JSONItem is an Array and get a reference to its value.
     * Throws if the item is not actually an array */
    JSONArray& get_array () const{
      asset_assert(type == JSONType::Array, "Expected an Array, not %s", JSONType::name(type));
      return (JSONArray&) array;
    }

    /* Assume a JSONItem is an Object and get a reference to its value.
     * Throws if the item is not actually an object */
    JSONObject& get_object () const{
      asset_assert(type == JSONType::Object, "Expected an Object, not %s", JSONType::name(type));
      return (JSONObject&) object;
    }



    /* Assume a JSONItem is a JSONObject and get a pointer to a particular key.
     * Returns NULL if no key matching the input is found.
     * Throws if the active item is not actually a JSONObject */
    String* get_object_key (char const* key_value, size_t key_length = 0) const {
      return get_object().get_key(key_value, key_length);
    }


    /* Assume a JSONItem is a JSONObject and get a pointer to a subitem associated with a particular key.
     * Returns NULL if no key matching the input is found.
     * Throws if the active item is not actually a JSONObject */
    JSONItem* get_object_item (char const* key_value, size_t key_length = 0) const {
      return get_object().get(key_value, key_length);
    }


    /* Assume a JSONItem is a JSONObject and get a value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    template <typename T> T& get_object_value (char const* key_value, size_t key_length = 0) const {
      JSONItem* item = get_object_item(key_value, key_length);

      static_assert(
           std::is_same<T, bool>::value
        || std::is_same<T, f64_t>::value
        || std::is_same<T, String>::value
        || std::is_same<T, JSONArray>::value
        || std::is_same<T, JSONObject>::value,
        "JSON::get_object_value only supports values of types serializable to json form"
      );

      if constexpr (std::is_same<T, bool>::value) {
        asset_assert(item != NULL, "Expected a Boolean at key %.*s",  (s32_t) key_length == 0? strlen(key_value) : key_length, key_value);
        return item->get_boolean();
      } else if constexpr (std::is_same<T, f64_t>::value) {
        asset_assert(item != NULL, "Expected a Number at key %.*s",  (s32_t) key_length == 0? strlen(key_value) : key_length, key_value);
        return item->get_number();
      } else if constexpr (std::is_same<T, String>::value) {
        asset_assert(item != NULL, "Expected a String at key %.*s",  (s32_t) key_length == 0? strlen(key_value) : key_length, key_value);
        return item->get_string();
      } else if constexpr (std::is_same<T, JSONArray>::value) {
        asset_assert(item != NULL, "Expected an Array at key %.*s",  (s32_t) key_length == 0? strlen(key_value) : key_length, key_value);
        return item->get_array();
      } else if constexpr (std::is_same<T, JSONObject>::value) {
        asset_assert(item != NULL, "Expected an Object at key %.*s",  (s32_t) key_length == 0? strlen(key_value) : key_length, key_value);
        return item->get_object();
      }
    }

    /* Assume a JSONItem is a JSONObject and get a boolean value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    bool& get_object_boolean (char const* key_value, size_t key_length = 0) const {
      return get_object_value<bool>(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get a number value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    f64_t& get_object_number (char const* key_value, size_t key_length = 0) const {
      return get_object_value<f64_t>(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get a string value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    String& get_object_string (char const* key_value, size_t key_length = 0) const {
      return get_object_value<String>(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get an array value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    JSONArray& get_object_array (char const* key_value, size_t key_length = 0) const {
      return get_object_value<JSONArray>(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get an object value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    JSONObject& get_object_object (char const* key_value, size_t key_length = 0) const {
      return get_object_value<JSONObject>(key_value, key_length);
    }


    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Throws if the active item is not actually a JSONObject */
    void set_object_item (JSONItem const* item, char const* key_value, size_t key_length = 0) {
      get_object().set(item, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key, by reference.
     * Throws if the active item is not actually a JSONObject */
    void set_object_item (JSONItem const& item, char const* key_value, size_t key_length = 0) {
      return set_object_item(&item, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if no key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value (T value, char const* key_value, size_t key_length, size_t value_origin_offset = 0) {
      return set_object_item(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset }, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if the active item is not actually a JSONObject */
    template <typename T> void set_object_value (T value, char const* key_value, size_t value_origin_offset = 0) {
      return set_object_item(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset }, key_value, 0);
    }


    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique (JSONItem const* item, char const* key_value, size_t key_length = 0) {
      if (key_length != 0) {
        asset_assert(get_object().set_unique(item, key_value, key_length) != -1, "Item with key '%.*s' already exists", (s32_t) key_length, key_value);
      } else {
        asset_assert(get_object().set_unique(item, key_value, key_length) != -1, "Item with key '%s' already exists", key_value);
      }
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key, by reference.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique (JSONItem const& item, char const* key_value, size_t key_length = 0) {
      return set_object_item_unique(&item, key_value, key_length);
    }

    
    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value_unique (T value, char const* key_value, size_t key_length, size_t value_origin_offset = 0) {
      return set_object_item_unique(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset }, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value_unique (T value, char const* key_value, size_t value_origin_offset = 0) {
      return set_object_item_unique(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset }, key_value, 0);
    }


    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Takes ownership of the String passed as key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique_string_key (JSONItem const* item, String key) {
      asset_assert(get_object().set_unique_string_key(item, key) != -1, "Item with key '%s' already exists", key.value);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key, by reference.
     * Takes ownership of the String passed as key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique_string_key (JSONItem const& item, String key) {
      return set_object_item_unique_string_key(&item, key);
    }

    
    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Takes ownership of the String passed as key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value_unique_string_key (T value, String key) {
      return set_object_item_unique_string_key(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset }, key);
    }


    /* Assume a JSONItem is a JSONObject and remove a subitem associated with a particular key.
     * Throws if no key matching the input is found, or if the active item is not actually a JSONObject */
    void remove_object_item (char const* key_value, size_t key_length = 0) {
      return get_object().remove(key_value, key_length);
    }



    /* Assume a JSONItem is a JSONArray and get a pointer to a subitem associated with a particular index.
     * Returns NULL if the index is out of range.
     * Throws if the active item is not actually a JSONArray */
    JSONItem* get_array_item (size_t index) const {
      return get_array().get_element(index);
    }

    
    /* Assume a JSONItem is a JSONArray and get the value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    template <typename T> T& get_array_value (size_t index) const {
      JSONItem* item = get_array_item(index);

      static_assert(
           std::is_same<T, bool>::value
        || std::is_same<T, f64_t>::value
        || std::is_same<T, String>::value
        || std::is_same<T, JSONArray>::value
        || std::is_same<T, JSONObject>::value,
        "JSON::get_array_value only supports values of types serializable to json form"
      );

      if constexpr (std::is_same<T, bool>::value) {
        asset_assert(item != NULL, "Expected a Boolean at index %zu", index);
        return item->get_boolean();
      } else if constexpr (std::is_same<T, f64_t>::value) {
        asset_assert(item != NULL, "Expected a Number at index %zu", index);        
        return item->get_number();
      } else if constexpr (std::is_same<T, String>::value) {
        asset_assert(item != NULL, "Expected a String at index %zu", index);        
        return item->get_string();
      } else if constexpr (std::is_same<T, JSONArray>::value) {
        asset_assert(item != NULL, "Expected an Array at index %zu", index);        
        return item->get_array();
      } else if constexpr (std::is_same<T, JSONObject>::value) {
        asset_assert(item != NULL, "Expected an Object at index %zu", index);        
        return item->get_object();
      }
    }

    /* Assume a JSONItem is a JSONArray and get the boolean value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    bool& get_array_boolean (size_t index) const {
      return get_array_value<bool>(index);
    }

    /* Assume a JSONItem is a JSONArray and get the number value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    f64_t& get_array_number (size_t index) const {
      return get_array_value<f64_t>(index);
    }

    /* Assume a JSONItem is a JSONArray and get the string value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    String& get_array_string (size_t index) const {
      return get_array_value<String>(index);
    }

    /* Assume a JSONItem is a JSONArray and get the array value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    JSONArray& get_array_array (size_t index) const {
      return get_array_value<JSONArray>(index);
    }

    /* Assume a JSONItem is a JSONArray and get the object value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    JSONObject& get_array_object (size_t index) const {
      return get_array_value<JSONObject>(index);
    }

    /* Assume a JSONItem is a JSONArray and set the value of a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void set_array_item (JSONItem const* item, size_t index) {
      get_array()[index] = *item;
    }

    /* Assume a JSONItem is a JSONArray and set the value of a subitem associated with a particular index, by reference.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void set_array_item (JSONItem const& item, size_t index) {
      return set_array_item(&item, index);
    }

    /* Assume a JSONItem is a JSONArray and set the value of a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    template <typename T> void set_array_value (T value, size_t index, size_t value_origin_offset = 0) {
      return set_array_item(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset }, index);
    }


    /* Assume a JSONItem is a JSONArray and append an item to the end of the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void append_array_item (JSONItem const* item) {
      return get_array().append(item);
    }

    /* Assume a JSONItem is a JSONArray and append an item to the end of the array, by reference.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void append_array_item (JSONItem const& item) {
      return append_array_item(&item);
    }
      
    /* Assume a JSONItem is a JSONArray and append a value to the end of the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    template <typename T> void append_array_value (T value, size_t value_origin_offset = 0) {
      return append_array_item(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset });
    }


    /* Assume a JSONItem is a JSONArray and insert an item into the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void insert_array_item (JSONItem const* item, size_t index) {
      return get_array().insert(index, item);
    }

    /* Assume a JSONItem is a JSONArray and insert an item into the array, by reference.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void insert_array_item (JSONItem const& item, size_t index) {
      return insert_array_item(&item, index);
    }

    /* Assume a JSONItem is a JSONArray and insert a value into the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    template <typename T> void insert_array_value (T value, size_t index, size_t value_origin_offset = 0) {
      return insert_array_item(JSONItem { value, root, value_origin_offset == 0? origin_offset : value_origin_offset }, index);
    }


    /* Assume a JSONItem is a JSONArray and remove a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void remove_array_item (size_t index) {
      return get_array().remove(index);
    }



    /* Encode a textual representation of a generic JSONItem into a String, with optional indentation level */
    ENGINE_API void to_string (String* out_string, size_t indent = 0) const;


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

  struct JSON {
    char* origin = NULL;
    char* source = NULL;
    JSONItem data;


    /* Decode a String from an offset within a textual JSON representation.
     * Unescape any JSON-safe multiple-character representations of symbols such as \n, \\, etc to their single character representations */
    ENGINE_API String unescape_string_from_source (char const* source, size_t* offset) const;

    /* Encode a textual representation of a String into another String.
     * Escape any symbols like \n, \\, etc to JSON-safe multiple-character representations */
    static ENGINE_API void escape_string_to_source (String const* in_string, String* out_string);

    /* Add a series of double spaces to an output String, depending on the level of `indent` */
    static void indent_source (String* out_string, size_t indent) {
      for (size_t i = 0; i < indent; i ++) out_string->append("  ");
    }



    /* Create a new uninitialized JSON root with optional origin and source */
    JSON (char const* new_origin = NULL, char const* new_source = NULL, size_t new_source_length = 0)
    : origin(str_clone(new_origin))
    , source(str_clone(new_source, new_source_length))
    { }

    /* Create a new JSON root by explicitly initialzing all members */
    JSON (char* new_origin, char* new_source, JSONItem new_data)
    : origin(new_origin)
    , source(new_source)
    , data(new_data)
    { }

    /* Create a new JSON root with type and optional origin */
    JSON (u8_t type, char const* new_origin = NULL)
    : origin(str_clone(new_origin))
    { 
      m_assert(type == JSONType::Object || type == JSONType::Array, "JSON root Item must be either JSONType::Object or JSONType::Array, not %s", JSONType::name(type));
      data = { type, this, 0 };
    }


    /* Create a new JSON root by parsing source from a str */
    static ENGINE_API JSON from_str (char const* origin, char const* source, size_t source_length = 0);
    
    /* Create a new JSON root by parsing source from a str. Take ownership of the str */
    static ENGINE_API JSON from_str_ex (char const* origin, char* source);


    /* Create a new JSON root by parsing source from a file */
    static JSON from_file (char const* path) {
      auto [ data, length ] = load_file(path);

      m_asset_assert(data != NULL, path, "Failed to load source file");

      return from_str_ex(path, (char*) data);
    }


    /* Destroy a JSON root and clean up all of its descendants */
    void destroy () {
      if (origin != NULL) free(origin);
      if (source != NULL) free(source);
      data.destroy();
    }


    /* Create a String encoding a source version of a JSON tree */
    String to_string () const {
      String out;

      data.to_string(&out);

      return out;
    }

    /* Create a String encoding a source version of a JSON tree, and attempt to save that string as a file at the given path
     * Returns true if the file saved successfully */
    bool to_file (char const* path) const {
      String string = to_string();

      bool success = string.to_file(path);

      string.destroy();

      return success;
    }


    /* Translate a JSONItem's origin_offset to a line and column number from its source */
    ENGINE_API pair_t<s32_t, s32_t> get_line_column_info (size_t origin_offset) const;

    /* Throw an asset exception associated with a JSON's origin, after looking up line and column information from a JSONItem's origin_offset */
    template <typename ... A> NORETURN void asset_error (size_t origin_offset, char const* fmt, A ... args) const {
      pair_t lc = get_line_column_info(origin_offset);

      m_asset_error(origin, lc.a, lc.b, fmt, args...);
    }

    /* Throw an asset exception associated with a JSON's origin, after looking up line and column information from a JSONItem's origin_offset, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, size_t origin_offset, char const* fmt, A ... args) const {
      if (!cond) {
        pair_t lc = get_line_column_info(origin_offset);

        m_asset_error(origin, lc.a, lc.b, fmt, args...);
      }
    }


    /* Throw an asset exception associated with a JSON's origin */
    template <typename ... A> NORETURN void asset_error (char const* fmt, A ... args) const {
      m_asset_error(origin, -1, -1, fmt, args...);
    }

    /* Throw an asset exception associated with a JSON's origin, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, char const* fmt, A ... args) const {
      m_asset_assert(cond, origin, -1, -1, fmt, args...);
    }



    /* WRAPPED JSONITEM FUNCTIONS */

    /* Assume a JSONItem is a JSONObject and get a reference to a subitem associated with a particular key.
     * Throws if no key matching the input is found, or if the active item is not actually a JSONObject */
    JSONItem& operator [] (char const* key_value) const {
      JSONItem* item = data.get_object_item(key_value);
      data.asset_assert(item != NULL, "Could not find item with key '%s'", key_value);
      return *item;
    }

    /* Assume a JSONItem is a JSONArray and get a reference to a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    JSONItem& operator [] (size_t index) const {
      JSONItem* item = data.get_array_item(index);
      data.asset_assert(item != NULL, "Out of range access for JSONArray: index %zu, count %zu", index, data.array.count);
      return *item;
    }



    /* Overwrite a JSONItem's value with a Boolean, changing its type and destroying the old value where necessary */
    void set (bool value) {
      data.asset_error("Cannot set JSON root to type Boolean, it must be either Array or Object");
    }

    /* Overwrite a JSONItem's value with a Number, changing its type and destroying the old value where necessary */
    void set (f64_t value) {
      data.asset_error("Cannot set JSON root to type Number, it must be either Array or Object");      
    }

    /* Overwrite a JSONItem's value with a String, changing its type and destroying the old value where necessary */
    void set (String value) {
      data.asset_error("Cannot set JSON root to type String, it must be either Array or Object");      
    }

    /* Overwrite a JSONItem's value with an Array, changing its type and destroying the old value where necessary */
    void set (JSONArray value) {
      if (data.type != JSONType::Array) data.destroy();
      data.type = JSONType::Array;
      data.array = value;
    }

    /* Overwrite a JSONItem's value with an Object, changing its type and destroying the old value where necessary */
    void set (JSONObject value) {
      if (data.type != JSONType::Object) data.destroy();
      data.type = JSONType::Object;
      data.object = value;
    }


    /* Assume a JSONItem is an Array and get a reference to its value.
     * Throws if the item is not actually an array */
    JSONArray& get_array () const {
      data.asset_assert(data.type == JSONType::Array, "Expected an Array, not %s", JSONType::name(data.type));
      return (JSONArray&) data.array;
    }

    /* Assume a JSONItem is an Object and get a reference to its value.
     * Throws if the item is not actually an object */
    JSONObject& get_object () const {
      data.asset_assert(data.type == JSONType::Object, "Expected an Object, not %s", JSONType::name(data.type));
      return (JSONObject&) data.object;
    }



    /* Assume a JSONItem is a JSONObject and get a pointer to a particular key.
     * Returns NULL if no key matching the input is found.
     * Throws if the active item is not actually a JSONObject */
    String* get_object_key (char const* key_value, size_t key_length = 0) const {
      return data.get_object().get_key(key_value, key_length);
    }


    /* Assume a JSONItem is a JSONObject and get a pointer to a subitem associated with a particular key.
     * Returns NULL if no key matching the input is found.
     * Throws if the active item is not actually a JSONObject */
    JSONItem* get_object_item (char const* key_value, size_t key_length = 0) const {
      return data.get_object().get(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get a value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    template <typename T> T& get_object_value (char const* key_value, size_t key_length = 0) const {
      return data.get_object_value<T>(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get a boolean value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    bool& get_object_boolean (char const* key_value, size_t key_length = 0) const {
      return data.get_object_boolean(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get a number value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    f64_t& get_object_number (char const* key_value, size_t key_length = 0) const {
      return data.get_object_number(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get a string value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    String& get_object_string (char const* key_value, size_t key_length = 0) const {
      return data.get_object_string(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get an array value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    JSONArray& get_object_array (char const* key_value, size_t key_length = 0) const {
      return data.get_object_array(key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and get an object value from a subitem associated with a particular key.
     * Throws if no key matching the input is found,
     * or if the active item is not actually a JSONObject,
     * or if the subitem is not the correct type */
    JSONObject& get_object_object (char const* key_value, size_t key_length = 0) const {
      return data.get_object_object(key_value, key_length);
    }


    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Throws if the active item is not actually a JSONObject */
    void set_object_item (JSONItem const* item, char const* key_value, size_t key_length = 0) {
      data.get_object().set(item, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key, by reference.
     * Throws if the active item is not actually a JSONObject */
    void set_object_item (JSONItem const& item, char const* key_value, size_t key_length = 0) {
      return data.set_object_item(&item, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if no key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value (T value, char const* key_value, size_t key_length, size_t value_origin_offset = 0) {
      return data.set_object_item(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset }, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if the active item is not actually a JSONObject */
    template <typename T> void set_object_value (T value, char const* key_value, size_t value_origin_offset = 0) {
      return data.set_object_item(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset }, key_value, 0);
    }


    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique (JSONItem const* item, char const* key_value, size_t key_length = 0) {
      if (key_length != 0) {
        data.asset_assert(data.get_object().set_unique(item, key_value, key_length) != -1, "Item with key '%.*s' already exists", (s32_t) key_length, key_value);
      } else {
        data.asset_assert(data.get_object().set_unique(item, key_value, key_length) != -1, "Item with key '%s' already exists", key_value);
      }
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key, by reference.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique (JSONItem const& item, char const* key_value, size_t key_length = 0) {
      return data.set_object_item_unique(&item, key_value, key_length);
    }

    
    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value_unique (T value, char const* key_value, size_t key_length, size_t value_origin_offset = 0) {
      return data.set_object_item_unique(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset }, key_value, key_length);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key to a value.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value_unique (T value, char const* key_value, size_t value_origin_offset = 0) {
      return set_object_item_unique(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset }, key_value, 0);
    }


    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Takes ownership of the String passed as key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique_string_key (JSONItem const* item, String key) {
      data.asset_assert(data.get_object().set_unique_string_key(item, key) != -1, "Item with key '%s' already exists", key.value);
    }

    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key, by reference.
     * Takes ownership of the String passed as key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    void set_object_item_unique_string_key (JSONItem const& item, String key) {
      return data.set_object_item_unique_string_key(&item, key);
    }

    
    /* Assume a JSONItem is a JSONObject and set the value of a subitem associated with a particular key.
     * Takes ownership of the String passed as key.
     * Throws if a key matching the input is found, or if the active item is not actually a JSONObject */
    template <typename T> void set_object_value_unique_string_key (T value, String key) {
      return data.set_object_item_unique_string_key(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset }, key);
    }


    /* Assume a JSONItem is a JSONObject and remove a subitem associated with a particular key.
     * Throws if no key matching the input is found, or if the active item is not actually a JSONObject */
    void remove_object_item (char const* key_value, size_t key_length = 0) {
      return data.get_object().remove(key_value, key_length);
    }



    /* Assume a JSONItem is a JSONArray and get a pointer to a subitem associated with a particular index.
     * Returns NULL if the index is out of range.
     * Throws if the active item is not actually a JSONArray */
    JSONItem* get_array_item (size_t index) const {
      return data.get_array().get_element(index);
    }

    /* Assume a JSONItem is a JSONArray and get the value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    template <typename T> T& get_array_value (size_t index) const {
      return data.get_array_value<T>(index);
    }

    /* Assume a JSONItem is a JSONArray and get the boolean value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    bool& get_array_boolean (size_t index) const {
      return data.get_array_boolean(index);
    }

    /* Assume a JSONItem is a JSONArray and get the number value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    f64_t& get_array_number (size_t index) const {
      return data.get_array_number(index);
    }

    /* Assume a JSONItem is a JSONArray and get the string value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    String& get_array_string (size_t index) const {
      return data.get_array_string(index);
    }

    /* Assume a JSONItem is a JSONArray and get the array value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    JSONArray& get_array_array (size_t index) const {
      return data.get_array_array(index);
    }

    /* Assume a JSONItem is a JSONArray and get the object value of a subitem associated with a particular index, by reference.
     * Throws if no index matching the input is found,
     * or if the active item is not actually a JSONArray,
     * or if the subitem is not the correct type */
    JSONObject& get_array_object (size_t index) const {
      return data.get_array_object(index);
    }


    /* Assume a JSONItem is a JSONArray and set the value of a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void set_array_item (JSONItem const* item, size_t index) {
      data.get_array()[index] = *item;
    }

    /* Assume a JSONItem is a JSONArray and set the value of a subitem associated with a particular index, by reference.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void set_array_item (JSONItem const& item, size_t index) {
      return data.set_array_item(&item, index);
    }

    /* Assume a JSONItem is a JSONArray and set the value of a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    template <typename T> void set_array_value (T value, size_t index, size_t value_origin_offset = 0) {
      return data.set_array_item(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset }, index);
    }


    /* Assume a JSONItem is a JSONArray and append an item to the end of the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void append_array_item (JSONItem const* item) {
      return data.get_array().append(item);
    }

    /* Assume a JSONItem is a JSONArray and append an item to the end of the array, by reference.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void append_array_item (JSONItem const& item) {
      return data.append_array_item(&item);
    }
      
    /* Assume a JSONItem is a JSONArray and append a value to the end of the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    template <typename T> void append_array_value (T value, size_t value_origin_offset = 0) {
      return data.append_array_item(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset });
    }


    /* Assume a JSONItem is a JSONArray and insert an item into the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void insert_array_item (JSONItem const* item, size_t index) {
      return data.get_array().insert(index, item);
    }

    /* Assume a JSONItem is a JSONArray and insert an item into the array, by reference.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void insert_array_item (JSONItem const& item, size_t index) {
      return data.insert_array_item(&item, index);
    }

    /* Assume a JSONItem is a JSONArray and insert a value into the array.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    template <typename T> void insert_array_value (T value, size_t index, size_t value_origin_offset = 0) {
      return data.insert_array_item(JSONItem { value, this, value_origin_offset == 0? data.origin_offset : value_origin_offset }, index);
    }


    /* Assume a JSONItem is a JSONArray and remove a subitem associated with a particular index.
     * Throws if the index is out of range, or if the active item is not actually a JSONArray */
    void remove_array_item (size_t index) {
      return data.get_array().remove(index);
    }
  };
}

#endif