CC = g++

run: compile
	./make_index

compile: make_index.cpp
	$(CC) make_index.cpp -o make_index -g `pkg-config --cflags --libs icu-uc icu-i18n`
	mkdir ds_folder ids_folder positions_folder logs_folder

clean:
	rm -f make_index
	find ds_folder/ -name "*" -delete
	find ids_folder/ -name "*" -delete
	find positions_folder/ -name "*" -delete
	find logs_folder/ -name "*" -delete
