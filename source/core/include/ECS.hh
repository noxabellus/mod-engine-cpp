#pragma once

#include "cstd.hh"
#include "Bitmask.hh"
#include "ThreadPool.hh"


namespace mod {
  #ifndef CUSTOM_COMPONENT_BITMASK
    using ComponentBitmask = Bitmask<1>;
  #else
    using ComponentBitmask = CUSTOM_COMPONENT_BITMASK;
  #endif


  struct Entity;
  struct EntityHandle;
  struct ComponentType;
  struct System;
  struct SystemIteratorArg;
  struct ECS;
  

  struct Entity {
    using ID = u32_t;

    ID id;
    ComponentBitmask enabled_components;


    Entity () { };


    private: friend ECS;
      Entity (ID in_id, ComponentBitmask in_enabled_components)
      : id(in_id)
      , enabled_components(in_enabled_components)
      { }
  };

  

  struct ComponentType {
    #ifndef CUSTOM_COMPONENT_TYPE_ID
      using ID = u8_t;
    #else
      using ID = CUSTOM_COMPONENT_TYPE_ID;
    #endif


    static constexpr size_t max_component_types = num::min(ComponentBitmask::bit_count, std::numeric_limits<ID>::max());
    

    ID id;
    char const* name;
    void* instances;
    size_t instance_size;
    size_t hash_code;


    ComponentType () { }


    template <typename T> T& get_instance (u32_t index) const {
      m_assert(typeid(T).hash_code() == hash_code, "Cannot get ComponentType %s instance as type %s, the type hash codes do not match", name, str_get_unscoped_type_name(typeid(T).name()));

      return ((T*) instances)[index];
    }

    void* get_instance_by_id (u32_t index) const {
      return ((u8_t*) instances) + (index * instance_size);
    }


    void swap_instances (u32_t dest, u32_t src) {
      memcpy(get_instance_by_id(dest), get_instance_by_id(src), instance_size);
    }


    private: friend ECS;
      ComponentType (u32_t capacity, ID in_id, char const* in_name, size_t in_instance_size, size_t in_hash_code)
      : id(in_id)
      , name(in_name)
      , instances(malloc(capacity * in_instance_size))
      , instance_size(in_instance_size)
      , hash_code(in_hash_code)
      { }


      void reallocate (u32_t new_capacity) {
        instances = realloc(instances, new_capacity * instance_size);

        m_assert(
          instances != NULL,
          "Out of memory or other null pointer error while attempting to reallocate ComponentType %s instances with capacity %" PRIu64,
          name, (u64_t) new_capacity
        );
      }


      void destroy () {
        free(instances);
      }
  };



  struct System {
    using IteratorCallback = std::function<void (ECS*, u32_t)>;
    using CustomCallback = std::function<void (ECS*)>;

    using ID = u8_t;


    static constexpr size_t max_systems = std::numeric_limits<ID>::max();


    char* name;
    ID id;

    bool enabled;

    bool custom;
    union {
      struct {
        bool parallel;
        ComponentBitmask required_components;
        IteratorCallback iterator_callback;
      };
      CustomCallback custom_callback;
    };



    System () { }
    
    System& operator = (System const& other) {
      name = other.name;
      id = other.id;
      enabled = other.enabled;
      custom = other.custom;
      if (custom) {
        custom_callback = other.custom_callback;
      } else {
        parallel = other.parallel;
        required_components = other.required_components;
        iterator_callback = other.iterator_callback;
      }
      other.~System(); // avoid memory leak during shift_systems
      return *this;
    }

    ~System () {
      if (id != 0) {
        printf("Destructing system %u", id);
        if (custom) custom_callback.~CustomCallback();
        else iterator_callback.~IteratorCallback();
        id = 0;
      }
    }
    

    ENGINE_API void execute (ECS* ecs) const;


    private: friend ECS;
      System (char const* in_name, ID in_id, CustomCallback in_custom_callback)
      : name (str_clone(in_name))
      , id(in_id)
      , enabled(true)
      , custom(true)
      , custom_callback(in_custom_callback)
      { }

      System (char const* in_name, ID in_id, bool in_parallel, ComponentBitmask in_required_components, IteratorCallback in_iterator_callback)
      : name (str_clone(in_name))
      , id(in_id)
      , enabled(true)
      , custom(false)
      , parallel(in_parallel)
      , required_components(in_required_components)
      , iterator_callback(in_iterator_callback)
      { }


      void destroy () {
        free(name);
      }

      ENGINE_API static void iterator_execution_instance (SystemIteratorArg* arg);

      ENGINE_API void execute_parallel (ECS* ecs) const;

      ENGINE_API void execute_sequential (ECS* ecs) const;
  };


  
  struct EntityHandle {
    ECS* ecs;
    u32_t index;
    Entity::ID id;


    EntityHandle () { }


    ENGINE_API void* create_component_by_id (ComponentType::ID type_id);

    ENGINE_API void* create_component_by_name (char const* name);

    template <typename T, typename ... A> T& create_component (A ... args) {
      return ecs->create_component<T>(*this, args...);
    }


    ENGINE_API void* get_component_by_id (ComponentType::ID type_id);

    ENGINE_API void* get_component_by_name (char const* name);

    template <typename T> T& get_component () {
      return ecs->get_component<T>(*this);
    }


    ENGINE_API void destroy_component_by_id (ComponentType::ID type_id);

    ENGINE_API void destroy_component_by_name (char const* name);

    template <typename T> void destroy_component () {
      ecs->destroy_component<T>(*this);
    }


    ENGINE_API void destroy ();


    ENGINE_API bool update ();

    ENGINE_API EntityHandle& verified ();

    ENGINE_API Entity* get_pointer ();

    ENGINE_API Entity* get_verified_pointer ();

    ENGINE_API Entity& dereference ();

    ENGINE_API Entity& operator * ();

    ENGINE_API Entity* operator -> ();

    private: friend ECS;
      EntityHandle (ECS* in_ecs, u32_t in_index, Entity::ID in_id)
      : ecs(in_ecs)
      , index(in_index)
      , id(in_id)
      { }
  };



  class SystemIteratorArg {
    friend ECS;
    friend System;

    ECS* ecs;
    System* sys;
    u32_t range_base;
    u32_t range_ext;

    SystemIteratorArg (ECS* in_ecs, System* in_sys, u32_t in_range_base, u32_t in_range_ext)
    : ecs(in_ecs)
    , sys(in_sys)
    , range_base(in_range_base)
    , range_ext(in_range_ext)
    { }

    public:
      SystemIteratorArg () { }
  };


  struct ECS {
    #ifndef CUSTOM_ECS_DEFAULT_ENTITY_CAPACITY
      static constexpr u32_t default_entity_capacity = 0xffffu;
    #else
      static constexpr u32_t default_entity_capacity = CUSTOM_ECS_DEFAULT_ENTITY_CAPACITY;
    #endif

    #ifndef CUSTOM_ECS_DEFAULT_ENTITY_THREAD_THRESHOLD
      static constexpr u32_t default_entity_thread_threshold = 10000;
    #else
      static constexpr u32_t default_entity_thread_threshold = CUSTOM_ECS_DEFAULT_ENTITY_THREAD_THRESHOLD;
    #endif


    Entity* entities;
    u32_t entity_count;
    u32_t entity_capacity;
    u32_t entity_id_counter;

    ComponentType component_types [ComponentType::max_component_types];
    ComponentType::ID component_type_count;

    System systems [System::max_systems];
    System::ID system_count;
    System::ID system_id_counter;

    SystemIteratorArg* system_iterator_args;
    u32_t max_threads;
    u32_t max_iterators;

    u32_t entity_thread_threshold;

    ThreadPool* thread_pool;



    ENGINE_API ECS (u32_t entity_capacity = default_entity_capacity, u32_t entity_thread_threshold = default_entity_thread_threshold, uint8_t max_threads = 8, uint8_t thread_iterator_ratio = 1);
    


    ENGINE_API void enable_thread_pool ();

    ENGINE_API void disable_thread_pool ();


    ENGINE_API void destroy ();


    ENGINE_API void grow_allocation (u32_t additional_count = 1);


    ENGINE_API EntityHandle create_entity ();


    u32_t get_entity_index (Entity const* entity) const {
      return pointer_to_index(entities, entity);
    }

    Entity& get_entity (u32_t index) const {
      m_assert(index < entity_count, "Out of range ECS access for Entity at index %" PRIu64 ", (count is %" PRIu64 ")", (u64_t) index, (u64_t) entity_count);
      return (Entity&) entities[index];
    }
    

    ENGINE_API void destroy_entity (u32_t index);

    ENGINE_API void destroy_entity (EntityHandle& handle);


    template <typename T> ComponentType::ID create_component_type () {
      ComponentType::ID type_id = component_type_count;
      
      type_info const& t_info = typeid(T);
      char const* name = str_get_unscoped_type_name(t_info.name());
      size_t hash_code = t_info.hash_code();

      m_assert(
        type_id < ComponentType::max_component_types,
        "Cannot create new ComponentType for the current ECS, the maximum number of ComponentTypes (%" PRIu64 " have already been created for this ECS",
        (u64_t) ComponentType::max_component_types
      );

      for (ComponentType::ID i = 0; i < component_type_count; i ++) {
        m_assert(
          component_types[i].hash_code != hash_code,
          "Cannot create ComponentType wrapping type %s because a ComponentType has already been registered for this type with id %" PRIu64,
          name, (u64_t) i
        );
      }

      component_types[type_id] = ComponentType(entity_capacity, type_id, name, sizeof(T), hash_code);

      ++ component_type_count;

      return type_id;
    }


    ENGINE_API ComponentType& get_component_type_by_name (char const* name) const;
    
    template <typename T> ComponentType& get_component_type_by_instance_type () const {
      size_t hash_code = typeid(T).hash_code();

      for (ComponentType::ID i = 0; i < component_type_count; i ++) {
        if (component_types[i].hash_code == hash_code) return (ComponentType&) component_types[i];
      }

      String types;
      for (ComponentType::ID i = 0; i < component_type_count; i ++) types.fmt_append("'%s',\n", component_types[i].name);
      m_error("Could not find ComponentType matching instance type %s, registered type names are:\n%s", str_get_unscoped_type_name(typeid(T).name()), types.value);
    }


    ENGINE_API void* create_component_by_id (u32_t index, ComponentType::ID type_id);

    ENGINE_API void* create_component_by_id (EntityHandle& handle, ComponentType::ID type_id);
    
    void* create_component_by_name (u32_t index, char const* name) {
      return create_component_by_id(index, get_component_type_by_name(name).id);
    }

    void* create_component_by_name (EntityHandle& handle, char const* name) {
      return create_component_by_id(handle, get_component_type_by_name(name).id);
    }

    template <typename T, typename ... A> T& create_component (u32_t index, A ... args) {
      Entity& entity = get_entity(index);
      ComponentType& type = get_component_type_by_instance_type<T>();

      m_assert(
        !entity.enabled_components.match_index(type.id),
        "Cannot create Component of type %s on Entity with ID %" PRIu64 " because a Component of this type already exists",
        type.name, (u64_t) entity.id
      );

      entity.enabled_components.set(type_id);

      auto new_instance = (T*) type.get_instance_by_id(index);

      new (new_instance) T { args... };

      return *new_instance;
    }

    template <typename T, typename ... A> T& create_component (EntityHandle& handle, A ... args) {
      Entity& entity = *handle;
      ComponentType& type = get_component_type_by_instance_type<T>();

      m_assert(
        !entity.enabled_components.match_index(type.id),
        "Cannot create Component of type %s on Entity with ID %" PRIu64 " because a Component of this type already exists",
        type.name, (u64_t) entity.id
      );

      entity.enabled_components.set(type.id);

      auto new_instance = (T*) type.get_instance_by_id(handle.index);

      new (new_instance) T { args... };

      return *new_instance;
    }


    ENGINE_API void* get_component_by_id (u32_t index, ComponentType::ID type_id) const;

    ENGINE_API void* get_component_by_id (EntityHandle& handle, ComponentType::ID type_id) const;

    void* get_component_by_name (u32_t index, char const* name) const {
      return get_component_by_id(index, get_component_type_by_name(name).id);
    }

    void* get_component_by_name (EntityHandle& handle, char const* name) const {
      return get_component_by_id(handle, get_component_type_by_name(name).id);
    }

    template <typename T> T& get_component (u32_t index) const {
      Entity& entity = get_entity(index);
      ComponentType& type = get_component_type_by_instance_type<T>();

      m_assert(
        entity.enabled_components.match_index(type.id),
        "Cannot get Component of type %s on Entity with ID %" PRIu64 " because a Component of this type does not exist for the given Entity",
        type.name, (u64_t) entity.id
      );

      return type.get_instance<T>(index);
    }

    template <typename T> T& get_component (EntityHandle& handle) const {
      Entity& entity = *handle;
      ComponentType& type = get_component_type_by_instance_type<T>();

      m_assert(
        entity.enabled_components.match_index(type.id),
        "Cannot get Component of type %s on Entity with ID %" PRIu64 " because a Component of this type does not exist for the given Entity",
        type.name, (u64_t) entity.id
      );

      return type.get_instance<T>(handle.index);
    }


    void destroy_component_by_id (u32_t index, ComponentType::ID type_id) {
      get_entity(index).enabled_components.unset(type_id);
    }

    void destroy_component_by_id (EntityHandle& handle, ComponentType::ID type_id) {
      handle->enabled_components.unset(type_id);
    }

    void destroy_component_by_name (u32_t index, char const* name) {
      destroy_component_by_id(index, get_component_type_by_name(name).id);
    }

    void destroy_component_by_name (EntityHandle& handle, char const* name) {
      destroy_component_by_id(handle, get_component_type_by_name(name).id);
    }

    template <typename T> void destroy_component (u32_t index) {
      destroy_component_by_id(index, get_component_type_by_instance_type<T>().id);
    }

    template <typename T> void destroy_component (EntityHandle& handle) {
      destroy_component_by_id(handle, get_component_type_by_instance_type<T>().id);
    }


    ENGINE_API s32_t get_system_index_by_id (System::ID id) const;

    ENGINE_API s32_t get_system_index_by_name (char const* name) const;

    ENGINE_API System& get_system_by_id (System::ID id) const;

    ENGINE_API System& get_system_by_name (char const* name) const;


    ENGINE_API System::ID create_system (char const* name, System::CustomCallback callback);

    ENGINE_API System::ID create_system_before (System::ID before_target, char const* name, System::CustomCallback callback);

    ENGINE_API System::ID create_system_before (char const* before_name, char const* name, System::CustomCallback callback);

    ENGINE_API System::ID create_system_after (System::ID after_target, char const* name, System::CustomCallback callback);

    ENGINE_API System::ID create_system_after (char const* after_name, char const* name, System::CustomCallback callback);


    ENGINE_API System::ID create_system (char const* name, bool parallel, ComponentBitmask required_components, System::IteratorCallback callback);

    ENGINE_API System::ID create_system_before (System::ID before_target, char const* name, bool parallel, ComponentBitmask required_components, System::IteratorCallback callback);

    ENGINE_API System::ID create_system_before (char const* before_name, char const* name, bool parallel, ComponentBitmask required_components, System::IteratorCallback callback);

    ENGINE_API System::ID create_system_after (System::ID after_target, char const* name, bool parallel, ComponentBitmask required_components, System::IteratorCallback callback);

    ENGINE_API System::ID create_system_after (char const* after_name, char const* name, bool parallel, ComponentBitmask required_components, System::IteratorCallback callback);


    ENGINE_API void update ();


    private:
      ENGINE_API System::ID init_system (System::ID index, char const* name, System::CustomCallback callback);

      ENGINE_API System::ID init_system (System::ID index, char const* name, bool parallel, ComponentBitmask required_components, System::IteratorCallback callback);

      void validate_system_count () const {
        m_assert(
          system_id_counter < System::max_systems,
          "Cannot create new System because the maximmum number of Systems (%" PRIu64 ") have already been created for this ECS",
          (u64_t) System::max_systems
        );
      }

      void shift_systems (System::ID index) {
        for (System::ID i = system_count; i > index; i --) {
          systems[i] = systems[i - 1];
        }
      }
  };
}