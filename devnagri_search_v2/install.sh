sudo apt install libicu-dev
#complie with this
#g++ sample.cpp -o sample `pkg-config --cflags --libs icu-uc icu-i18n`
g++ make_index.cpp -o make_index `pkg-config --cflags --libs icu-uc icu-i18n`
