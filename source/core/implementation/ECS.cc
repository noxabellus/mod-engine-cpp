#include "../include/ECS.hh"
#include "../include/String.hh"
#include "../include/Hierarchy.hh"

namespace mod {
  void* EntityHandle::create_component_by_id (ComponentType::ID type_id) {
    return ecs->create_component_by_id(*this, type_id);
  }

  void* EntityHandle::create_component_by_name (char const* name) {
    return ecs->create_component_by_name(*this, name);
  }

  void* EntityHandle::get_component_by_id (ComponentType::ID type_id) {
    return ecs->get_component_by_id(*this, type_id);
  }

  void* EntityHandle::get_component_by_name (char const* name) {
    return ecs->get_component_by_name(*this, name);
  }

  void EntityHandle::destroy_component_by_id (ComponentType::ID type_id) {
    ecs->destroy_component_by_id(*this, type_id);
  }

  void EntityHandle::destroy_component_by_name (char const* name) {
    ecs->destroy_component_by_name(*this, name);
  }

  void EntityHandle::destroy_entity () {
    ecs->destroy_entity(*this);
    id = 0;
  }

  
  bool EntityHandle::update () {
    if (id == 0) return false;
    
    Entity* entity = index < ecs->entity_count? ecs->entities + index : NULL;

    if (entity != NULL && entity->id == id) return true;
    else {
      for (uint64_t i = 0; i < ecs->entity_count; i ++) {
        entity = ecs->entities + i;

        if (entity->id == id) {
          index = i;
          return true;
        }
      }

      return false;
    }
  }


  EntityHandle& EntityHandle::verified () {
    m_assert(update(), "Failed to verify EntityHandle: the Handle's ID (%" PRIu64 ") was invalid", static_cast<u64_t>(id));
    return *this;
  }

  Entity* EntityHandle::get_pointer () {
    if (update()) return ecs->entities + index;
    else return NULL;
  }

  Entity* EntityHandle::get_verified_pointer () {
    Entity* ptr = get_pointer();
    m_assert(ptr != NULL, "Could not get verified pointer from EntityHandle: the Handle's ID (%" PRIu64 ") was invalid", static_cast<u64_t>(id));
    return ptr;
  }

  Entity& EntityHandle::dereference () {
    return *get_verified_pointer();
  }

  Entity& EntityHandle::operator * () {
    return dereference();
  }

  Entity* EntityHandle::operator -> () {
    return get_verified_pointer();
  }




  void System::iterator_execution_instance (SystemIteratorArg* arg) {
    for (u32_t i = arg->range_base; i < arg->range_ext; i ++) {
      if (arg->ecs->entities[i].enabled_components.match_subset(arg->sys->required_components)) {
        arg->sys->iterator_callback(arg->ecs, i);
      }
    }
  }

  void System::execute_parallel (ECS* ecs) const {
    u32_t entities_per_job = ecs->entity_count / ecs->max_iterators;

    if (ecs->entity_count % ecs->max_iterators != 0) ++ entities_per_job;

    u32_t arg_base = 0;

    for (u32_t i = 0; i < ecs->max_iterators; i ++) {
      u32_t remaining_ents = ecs->entity_count - arg_base;
      u32_t arg_ents = num::min(entities_per_job, remaining_ents);
      u32_t arg_ext = arg_base + arg_ents;

      SystemIteratorArg& arg = ecs->system_iterator_args[i];

      arg.sys = const_cast<System*>(this);
      arg.range_base = arg_base;
      arg.range_ext = arg_ext;

      ecs->thread_pool->queue(reinterpret_cast<Job::Callback>(System::iterator_execution_instance), &arg);

      arg_base = arg_ext;
    }

    ecs->thread_pool->await_all();
  }

  void System::execute_sequential (ECS* ecs) const {
    SystemIteratorArg arg = {
      ecs, const_cast<System*>(this),
      0, ecs->entity_count
    };
    
    iterator_execution_instance(&arg);
  }

  void System::execute (ECS* ecs) const {
    if (enabled) {
      if (custom) {
        custom_callback(ecs);
      } else {
        if (parallel && ecs->thread_pool != NULL) {
          execute_parallel(ecs);
        } else {
          execute_sequential(ecs);
        }
      }
    }
  }


  ECS::ECS (u32_t in_entity_capacity, u32_t in_entity_thread_threshold, uint8_t in_max_threads, uint8_t thread_iterator_ratio)
  : entities(memory::allocate<Entity>(in_entity_capacity))
  , entity_count(0)
  , entity_capacity(in_entity_capacity)
  , entity_id_counter(1)
  , component_type_count(0)
  , system_count(0)
  , system_id_counter(1)
  , system_iterator_args(NULL)
  , max_threads(in_max_threads)
  , max_iterators(thread_iterator_ratio * in_max_threads)
  , entity_thread_threshold(in_entity_thread_threshold)
  , thread_pool(NULL)
  {
    memory::clear(systems, System::max_systems);
    m_assert(entities != NULL, "Out of memory or other null pointer error while allocating ECS entities with starting capacity %" PRIu32, entity_capacity);
    create_component_type<Transform3D>();
    create_component_type<Child>();
    create_component_type<Parent>();
    create_component_type<SkeletonState>();
  }


  void ECS::enable_thread_pool () {
    if (thread_pool == NULL) {
      system_iterator_args = memory::allocate<SystemIteratorArg>(max_iterators);

      for (u32_t i = 0; i < max_iterators; i ++) {
        system_iterator_args[i].ecs = this;
      }

      thread_pool = new ThreadPool { max_threads };
    }
  }

  void ECS::disable_thread_pool () {
    if (thread_pool != NULL) {
      thread_pool->destroy();
      delete thread_pool;

      thread_pool = NULL;
    }
  }


  void ECS::destroy () {
    disable_thread_pool();

    for (ComponentType::ID i = 0; i < component_type_count; i ++) {
      for (size_t j = 0; j < entity_count; j ++) {
        if (entities[j].enabled_components[i]) component_types[i].destroy_instance(j);
      }

      component_types[i].destroy();
    }

    memory::deallocate(entities);

    for (System::ID i = 0; i < system_count; i ++) {
      systems[i].destroy();
    }

    delete this;
  }


  void ECS::grow_allocation (u32_t additional_count) {
    u32_t new_count = entity_count + additional_count;
    u32_t new_capacity = entity_capacity;

    while (new_capacity < new_count) new_capacity *= 2;

    if (new_capacity > entity_capacity) {
      memory::reallocate(entities, new_capacity);

      m_assert(entities != NULL, "Out of memory or other null pointer error while reallocating ECS entities for capacity %" PRIu32, new_capacity);

      entity_capacity = new_capacity;

      for (ComponentType::ID i = 0; i < component_type_count; i ++) {
        component_types[i].reallocate(entity_capacity);
      }
    }
  }


  EntityHandle ECS::create_entity () {
    grow_allocation();

    Entity* entity = entities + entity_count;

    *entity = {
      entity_id_counter,
      { }
    };

    EntityHandle h = { this, entity_count, entity_id_counter };

    ++ entity_id_counter;
    ++ entity_count;

    return h;
  }

  void ECS::destroy_entity (u32_t index) {
    if (index > entity_count) return;

    u32_t last_index = entity_count - 1;

    if (index != last_index) {
      Entity* last_entity = entities + last_index;

      for (ComponentType::ID i = 0; i < ComponentType::max_component_types; i ++) {
        if (last_entity->enabled_components.match_index(i)) {
          component_types[i].swap_instances(index, last_index);
        }
      }

      entities[index] = *last_entity;
    }

    -- entity_count;
  }

  void ECS::destroy_entity (EntityHandle& handle) {
    Entity& entity = *handle;
    
    u32_t last_index = entity_count - 1;

    if (handle.index != last_index) {
      Entity* last_entity = entities + last_index;

      for (ComponentType::ID i = 0; i < ComponentType::max_component_types; i ++) {
        if (last_entity->enabled_components.match_index(i)) {
          component_types[i].destroy_instance(handle.index);
          component_types[i].swap_instances(handle.index, last_index);
        }
      }

      entity = *last_entity;
    }

    -- entity_count;
  }


  ComponentType& ECS::get_component_type_by_name (char const* name) const {
    for (ComponentType::ID i = 0; i < component_type_count; i ++) {
      if (str_cmp_caseless(component_types[i].name, name) == 0) return const_cast<ComponentType&>(component_types[i]);
    }
    
    String types;
    for (ComponentType::ID i = 0; i < component_type_count; i ++) types.fmt_append("'%s', \n", component_types[i].name);
    m_error("Could not find ComponentType matching name %s. Registered component type names are:\n%s", name, types.value);
  }


  void* ECS::create_component_by_id (u32_t index, ComponentType::ID type_id) {
    m_assert(type_id < component_type_count, "Cannot get out of range ComponentType with id %" PRIu64, static_cast<u64_t>(type_id));

    Entity& entity = get_entity(index);
    ComponentType& type = component_types[type_id];

    m_assert(
      !entity.enabled_components.match_index(type_id),
      "Cannot create Component of type %s on Entity with ID %" PRIu64 " because a Component of this type already exists",
      type.name, static_cast<u64_t>(entity.id)
    );

    entity.enabled_components.set(type_id);
    
    void* ptr = type.get_instance_by_id(index);

    memory::clear(ptr, type.instance_size);

    return ptr;
  }

  void* ECS::create_component_by_id (EntityHandle& handle, ComponentType::ID type_id) {
    m_assert(type_id < component_type_count, "Cannot get out of range ComponentType with id %" PRIu64, static_cast<u64_t>(type_id));

    Entity& entity = *handle;
    ComponentType& type = component_types[type_id];

    m_assert(
      !entity.enabled_components.match_index(type_id),
      "Cannot create Component of type %s on Entity with ID %" PRIu64 " because a Component of this type already exists",
      type.name, static_cast<u64_t>(entity.id)
    );

    entity.enabled_components.set(type_id);
    
    void* ptr = type.get_instance_by_id(handle.index);

    memory::clear(ptr, type.instance_size);

    return ptr;
  }

  void* ECS::add_component_by_id (u32_t index, ComponentType::ID type_id, void const* data) {
    m_assert(type_id < component_type_count, "Cannot get out of range ComponentType with id %" PRIu64, static_cast<u64_t>(type_id));

    Entity& entity = get_entity(index);
    ComponentType& type = component_types[type_id];

    m_assert(
      !entity.enabled_components.match_index(type_id),
      "Cannot add Component of type %s on Entity with ID %" PRIu64 " because a Component of this type already exists",
      type.name, static_cast<u64_t>(entity.id)
    );

    entity.enabled_components.set(type_id);
    
    void* ptr = type.get_instance_by_id(index);

    memory::copy(ptr, data, type.instance_size);

    return ptr;
  }

  void* ECS::add_component_by_id (EntityHandle& handle, ComponentType::ID type_id, void const* data) {
    m_assert(type_id < component_type_count, "Cannot get out of range ComponentType with id %" PRIu64, static_cast<u64_t>(type_id));

    Entity& entity = *handle;
    ComponentType& type = component_types[type_id];

    m_assert(
      !entity.enabled_components.match_index(type_id),
      "Cannot add Component of type %s on Entity with ID %" PRIu64 " because a Component of this type already exists",
      type.name, static_cast<u64_t>(entity.id)
    );

    entity.enabled_components.set(type_id);
    
    void* ptr = type.get_instance_by_id(handle.index);

    memory::copy(ptr, data, type.instance_size);

    return ptr;
  }


  void* ECS::get_component_by_id (u32_t index, ComponentType::ID type_id) const {
    m_assert(type_id < component_type_count, "Cannot get out of range ComponentType with id %" PRIu64, static_cast<u64_t>(type_id));

    Entity& entity = get_entity(index);
    ComponentType const& type = component_types[type_id];

    m_assert(
      entity.enabled_components.match_index(type_id),
      "Cannot get Component of type %s on Entity with ID %" PRIu64 " because a Component of this type does not exist for the given Entity",
      type.name, static_cast<u64_t>(entity.id)
    );

    return type.get_instance_by_id(index);
  }

  void* ECS::get_component_by_id (EntityHandle& handle, ComponentType::ID type_id) const {
    m_assert(type_id < component_type_count, "Cannot get out of range ComponentType with id %" PRIu64, static_cast<u64_t>(type_id));

    Entity& entity = *handle;
    ComponentType const& type = component_types[type_id];

    m_assert(
      entity.enabled_components.match_index(type_id),
      "Cannot get Component of type %s on Entity with ID %" PRIu64 " because a Component of this type does not exist for the given Entity",
      type.name, static_cast<u64_t>(entity.id)
    );

    return type.get_instance_by_id(handle.index);
  }

  void ECS::destroy_component_by_id (u32_t index, ComponentType::ID type_id) {
    Entity& ent = get_entity(index);

    if (ent.enabled_components.match_index(type_id)) {
      ent.enabled_components.unset(type_id);
      component_types[type_id].destroy_instance(index);
    }
  }

  void ECS::destroy_component_by_id (EntityHandle& handle, ComponentType::ID type_id) {
    Entity& ent = *handle;

    if (ent.enabled_components.match_index(type_id)) {
      ent.enabled_components.unset(type_id);
      component_types[type_id].destroy_instance(handle.index);
    }
  }


  s32_t ECS::get_system_index_by_id (System::ID id) const {
    for (System::ID i = 0; i < system_count; i ++) {
      if (systems[i].id == id) return i;
    }

    return -1;
  }

  s32_t ECS::get_system_index_by_name (char const* name) const {
    for (System::ID i = 0; i < system_count; i ++) {
      if (str_cmp_caseless(systems[i].name, name) == 0) return i;
    }

    return -1;
  }

  System& ECS::get_system_by_id (System::ID id) const {
    for (System::ID i = 0; i < system_count; i ++) {
      if (systems[i].id == id) return const_cast<System&>(systems[i]);
    }

    m_error("Could not find System with id %" PRIu64, static_cast<u64_t>(id));
  }

  System& ECS::get_system_by_name (char const* name) const {
    for (System::ID i = 0; i < system_count; i ++) {
      if (str_cmp_caseless(systems[i].name, name) == 0) return const_cast<System&>(systems[i]);
    }

    m_error("Could not find System with name %s", name);
  }


  System::ID ECS::create_system (char const* name, System::CustomCallback callback) {
    validate_system_count();
    
    return init_system(system_count, name, callback);
  }

  System::ID ECS::create_system_before (System::ID before_target, char const* name, System::CustomCallback callback) {
    validate_system_count();

    s32_t index = get_system_index_by_id(before_target);
    
    m_assert(index != -1, "Cannot find System with id %" PRIu64 " in order to add new System %s before it", static_cast<u64_t>(before_target), name);

    shift_systems(index);

    return init_system(index, name, callback);;
  }

  System::ID ECS::create_system_before (char const* before_name, char const* name, System::CustomCallback callback) {
    validate_system_count();

    s32_t index = get_system_index_by_name(before_name);
    
    m_assert(index != -1, "Cannot find System with name %s in order to add new System %s before it", before_name, name);

    shift_systems(index);

    return init_system(index, name, callback);;
  }

  System::ID ECS::create_system_after (System::ID after_target, char const* name, System::CustomCallback callback) {
    validate_system_count();
    
    s32_t index = get_system_index_by_id(after_target);
    
    m_assert(index != -1, "Cannot find System with id %" PRIu64 " in order to add new System %s after it", static_cast<u64_t>(after_target), name);

    ++ index;

    shift_systems(index);

    return init_system(index, name, callback);;
  }

  System::ID ECS::create_system_after (char const* after_name, char const* name, System::CustomCallback callback) {
    validate_system_count();
    
    s32_t index = get_system_index_by_name(after_name);
    
    m_assert(index != -1, "Cannot find System with name %s in order to add new System %s after it", after_name, name);

    ++ index;

    shift_systems(index);

    return init_system(index, name, callback);;
  }


  System::ID ECS::create_system (char const* name, bool parallel, ComponentMask required_components, System::IteratorCallback callback) {
    validate_system_count();

    return init_system(system_count, name, parallel, required_components, callback);
  }

  System::ID ECS::create_system_before (System::ID before_target, char const* name, bool parallel, ComponentMask required_components, System::IteratorCallback callback) {
    validate_system_count();

    s32_t index = get_system_index_by_id(before_target);
    
    m_assert(index != -1, "Cannot find System with id %" PRIu64 " in order to add new System %s before it", static_cast<u64_t>(before_target), name);

    shift_systems(index);

    return init_system(index, name, parallel, required_components, callback);
  }

  System::ID ECS::create_system_before (char const* before_name, char const* name, bool parallel, ComponentMask required_components, System::IteratorCallback callback) {
    validate_system_count();

    s32_t index = get_system_index_by_name(before_name);
    
    m_assert(index != -1, "Cannot find System with name %s in order to add new System %s before it", before_name, name);

    shift_systems(index);

    return init_system(index, name, parallel, required_components, callback);
  }

  System::ID ECS::create_system_after (System::ID after_target, char const* name, bool parallel, ComponentMask required_components, System::IteratorCallback callback) {
    validate_system_count();

    s32_t index = get_system_index_by_id(after_target);
    
    m_assert(index != -1, "Cannot find System with id %" PRIu64 " in order to add new System %s after it", static_cast<u64_t>(after_target), name);

    ++ index;

    shift_systems(index);

    return init_system(index, name, parallel, required_components, callback);
  }

  System::ID ECS::create_system_after (char const* after_name, char const* name, bool parallel, ComponentMask required_components, System::IteratorCallback callback) {
    validate_system_count();

    s32_t index = get_system_index_by_name(after_name);
    
    m_assert(index != -1, "Cannot find System with name %s in order to add new System %s after it", after_name, name);

    ++ index;

    shift_systems(index);

    return init_system(index, name, parallel, required_components, callback);
  }


  void ECS::update () {
    if (entity_count >= entity_thread_threshold) enable_thread_pool();
    
    for (System::ID i = 0; i < system_count; i ++) {
      systems[i].execute(this);
    }
  }



  System::ID ECS::init_system (System::ID index, char const* name, System::CustomCallback callback ) {
    System::ID id = system_id_counter;

    new (systems + index) System { name, id, callback };

    ++ system_id_counter;
    ++ system_count;

    return id;
  }

  System::ID ECS::init_system (System::ID index, char const* name, bool parallel, ComponentMask required_components, System::IteratorCallback callback) {
    System::ID id = system_id_counter;

    new (systems + index) System { name, id, parallel, required_components, callback };

    ++ system_id_counter;
    ++ system_count;

    return id;
  }
}