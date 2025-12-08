- Problem Statement : Brain , Ear , Mouth Problem
    Brain - acts as central control unit , provides port and files to ear & mouth .
    Ear - listens at a given port and writes whatever listened in given file
    Mouth - reads a given file and sends it at provided port

- Files Description
    brain.c : program for brain
    ear.c : program for ear (expects port_no and file_name)
    mouth.c : program for mouth (expects port_no and file_name)
    makefile : can be used by make to comile all files 
    *.txt : files required by brain to give to ear and mouth ( don't necessasarily have to be given files only)*

- User Interface :
    : For brain 
        open 2 terminals 
            on first terminal
                execute: $./brain port_no1 port_no2 ear_file1 mouth_file1
            on second terminal
                execute: $./brain port_no2 port_no1 ear_file2 mouth_file2

            eg. 
                execute: $./brain 1234 1235 a.txt b.txt
                execute: $./brain 1235 1234 c.txt d.txt
            
            here argument order should be exactly same as given or else behaviour is undefined. 
        
        this will make both processes run in endless loop unless stopped externally.
