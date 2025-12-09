CC = g++

run: make_index.cpp
	$(CC) make_index.cpp -o make_index `pkg-config --cflags --libs icu-uc icu-i18n`

clean:
	rm -f make_index
	rm ds_folder/*
	rm ids_folder/*
	rm positions_folder/*
