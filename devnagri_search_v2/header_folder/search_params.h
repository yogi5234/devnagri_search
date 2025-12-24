#include "common_params.h"

#define get_docs_buffer_struct_size save_map_locations_buffer_struct_size 
#define get_docs_buffer_size (get_docs_buffer_struct_size * get_docs_buffer_members)
#define get_docs_buffer_members 256 // get_docs_buffer_struct_size * get_docs_buffer_members memory will be used
