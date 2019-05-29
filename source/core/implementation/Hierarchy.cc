#include "../include/Hierarchy.hh"


namespace mod {
  void Child::destroy () {
    if (parent_handle.id != 0) get_parent_references().remove_child(own_entity);
  }


  Parent& Child::get_parent_references () {
    return parent_handle.get_component<Parent>();
  }

  Matrix4 Child::compute_hierarchical_matrix () {
    Matrix4 parent_mat;

    if (parent_handle->enabled_components.match_index(parent_handle.ecs->get_component_type_by_instance_type<Transform3D>().id)) {
      parent_mat = parent_handle.get_component<Transform3D>().compose();
    } else {
      parent_mat = Constants::Matrix4::identity;
    }

    if (parent_handle->enabled_components.match_index(parent_handle.ecs->get_component_type_by_instance_type<Child>().id)) {
      parent_mat = parent_handle.get_component<Child>().compute_hierarchical_matrix() * parent_mat;
    }

    if (slot_index > -1 && parent_handle->enabled_components.match_index(parent_handle.ecs->get_component_type_by_instance_type<SkeletonState>().id)) {
      SkeletonState& ss = parent_handle.get_component<SkeletonState>();
      parent_mat = (ss.pose[slot_index] * ss.skeleton->bones[slot_index].bind_matrix) * parent_mat;
    }

    return parent_mat;
  }



  void Parent::destroy () {
    for (auto [ i, ch ] : child_handles) {
      ch.get_component<Child>().parent_handle.id = 0;
      ch.destroy_component<Child>();
    }
    child_handles.destroy();
  }


  void Parent::add_child (EntityHandle c, s32_t slot_index) {
    if (c->enabled_components.match_index(c.ecs->get_component_type_by_instance_type<Child>().id)) {
      Child& cpr = c.get_component<Child>();
      EntityHandle cp = cpr.parent_handle;

      if (cp != own_entity) {
        c.ecs->get_component<Parent>(cp).remove_child(c);
      } else {
        cpr.slot_index = slot_index;
        return;
      }
    }

    child_handles.append(c);
    c.add_component(Child { c, own_entity, slot_index });
  }

  void Parent::remove_child (EntityHandle c) {
    for (auto [ i, ch ] : child_handles) {
      if (ch == c) {
        c.get_component<Child>().parent_handle.id = 0;
        c.destroy_component<Child>();
        child_handles.remove(i);
        return;
      }
    }
  }
}