#ifndef HIERARCHY_H
#define HIERARCHY_H

#include "ECS.hh"
#include "math/lib.hh"
#include "graphics/lib.hh"


namespace mod {
  struct Parent;

  struct Child {
    EntityHandle own_entity;
    EntityHandle parent_handle;
    s32_t slot_index;


    Child () { }

    ENGINE_API void destroy ();


    ENGINE_API Parent& get_parent_references ();

    ENGINE_API Matrix4 compute_hierarchical_matrix ();


    private:
      friend Parent;
      Child (EntityHandle in_own_entity, EntityHandle in_parent_handle, s32_t in_slot_index = -1)
      : own_entity(in_own_entity)
      , parent_handle(in_parent_handle)
      , slot_index(in_slot_index)
      { }
  };

  struct Parent {
    EntityHandle own_entity;
    Array<EntityHandle> child_handles;


    Parent () { }
    Parent (EntityHandle in_own_entity)
    : own_entity(in_own_entity)
    { }

    ENGINE_API void destroy ();


    ENGINE_API void add_child (EntityHandle c, s32_t slot_index = -1);

    ENGINE_API void remove_child (EntityHandle c);
  };
}

#endif