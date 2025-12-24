#include "common_params.h"
#define doc_id_size 8 // in bytes
#define doc_vector_max_len_bytes 4 // in bytes
#define BUFFER_SIZE 4096
#define max_docs_in_mem 2 //think and reason about it

//typedefs used by extract_words_from_file
#define extract_words_from_file_buffer_size 4096

//typedefs used by save map
// buffer size used will be save_map_locations_buffer_struct_size * save_map_locations_buffer_members
#define save_map_locations_buffer_members 256
#define save_map_locations_buffer_size (save_map_locations_buffer_struct_size * save_map_locations_buffer_members)
#define save_map_positions_buffer_size 1024 // 4* this memory will be used
