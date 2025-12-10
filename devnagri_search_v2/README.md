- Problem Statement : Given Terabytes of .txt documents of devnagri .txt documents create some kind on index using which use can later answer some queries quickly
	- types of queries : ( Given sequence of words : give documents cloasely related to those words)


- Files Description
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
			
	params.cpp : arguemnts to give to make_index.cpp 
	my_typedefs.h : just some typedefs to make life easy
