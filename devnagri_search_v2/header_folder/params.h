#define doc_id_size 8 // in bytes
#define doc_vector_max_len_bytes 4 // in bytes
#define BUFFER_SIZE 4096
#define folder "/home/yogiraj/Desktop/SemIII/devnagri_search/devnagri_search_v2/"
#define ds_folder folder "ds_folder/"
#define txt_folder folder "txt_folder/"
#define log_file_name folder "logs_folder/logs"
#define doc_id_start_point folder "ds_folder/doc_id_start_point"
#define doc_id_file_name folder "ds_folder/doc_id_file_name"
#define max_docs_in_mem 200 //think and reason about it

//typedefs used by extract_words_from_file
#define extract_words_from_file_buffer_size 4096

//typedefs used by save map
// buffer size used will be save_map_locations_buffer_struct_size * save_map_locations_buffer_members
#define save_map_locations_buffer_members 256
#define save_map_locations_buffer_struct_size 12 // in bytes
#define save_map_locations_buffer_size (save_map_locations_buffer_struct_size * save_map_locations_buffer_members)
#define save_map_locations_ids_folder folder "ids_folder/"
#define save_map_locations_positions_folder folder "positions_folder/"
#define save_map_positions_buffer_size 1024 // 4* this memory will be used
