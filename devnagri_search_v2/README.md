- Problem Statement : Given Terabytes of .txt documents of devnagri .txt documents create some kind on index using which use can later answer some queries quickly
	- types of queries : ( Given sequence of words : give documents cloasely related to those words)


- Files Description
  makefile : file to run the program
    make clean : remove residual files
    make : compile and run
	make_index.cpp : used to make index data structure
		created Folders
			- ds_folder : 
				files 
				1. doc_id_start_point : keep array of 16 bytes structure 
				  - structure 
				  - {
					  - unsigned long long int doc_id ; //document id
					  - unsigned long long int file_start ; //location from which filename starts in doc_id_file_name
				  - }
				2. doc_id_file_name : keeps file names one follwed by another

      - header_folder : folder for header files
        files
        1. params.h : arguemnts to give to make_index.cpp 
        2. my_typedefs.h : just some typedefs to make life easy
        3. print_word_map.h : contains a function to print the in memory map used in make_index.cpp
			
      -	ids_folder : for each word contains a file named word + "_ids" which holds following info
        - structure (12 bytes)
        - {
        -   unsigned long long int doc_id ; // document id
        -   unsigned long long int list_pos_start ; // position from which list of this document starts 
                                                    // mult position by 4 to get byte no 
                                                    // to get total number of elements subtract list_pos_start of current document from next one
        - }
  
      - positions_folder : for each word contains a file named word + "_positions" which holds list of positions on which that word has occured this [positions] can be divided into [[positions]] based on info from word + "_ids" file from ids_folder/

      - logs_folder : 
      files
      logs : contains logs of make_index.cpp

      - txt_folder : contains txt files used for making index  
