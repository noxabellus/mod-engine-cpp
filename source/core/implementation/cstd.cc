#include "../include/cstd.hh"



namespace mod {
  namespace memory {
    size_t allocated_size = 0;
    size_t allocation_count = 0;
    
    #ifdef MEMORY_DEBUG_INDEPTH
      void** allocated_addresses = reinterpret_cast<void**>(malloc(sizeof(void*) * 64));
      AllocationName* allocated_types = reinterpret_cast<AllocationName*>(malloc(sizeof(AllocationName) * 64));
      AllocationTracePair* allocated_traces = reinterpret_cast<AllocationTracePair*>(malloc(sizeof(AllocationTracePair) * 64));
      size_t* allocated_type_sizes = reinterpret_cast<size_t*>(malloc(sizeof(size_t) * 64));
      size_t allocated_address_count = 0;
      size_t allocated_address_capacity = 64;
    #endif
  }
}