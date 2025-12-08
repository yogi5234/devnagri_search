#include <iostream>
#include <filesystem>
#include <cstdio>
#include <string>
#include <cstring>
#include <memory>
#include <unicode/unistr.h>
#include <unicode/brkiter.h>
#include <unicode/uchar.h>

#include <vector>
#include <unordered_map>
#include <map>

#include "params.cpp" // use this file for giving all parameters
#include "my_typedefs.h"

using namespace std;
namespace fs = std::filesystem;

// structure required for adding doc_id:doc_name entry
typedef struct doc_entry {
  FILE *fp_doc_id_start_point;
  FILE *fp_doc_id_file_name;
}doc_entry;

//structure for holding document_id and vector of positions in that document for a particular word
typedef struct map_helper {
  u64 doc_id ;
  vector<u32> locations;
}map_helper;

#include "print_word_map.cpp"
void init_doc_entry(doc_entry *var_doc_entry);
void add_docid_name_entry(u64 document_id,string *doc_name,doc_entry *var_doc_entry);
void add_docs_map(u64 document_id,string *doc_name,unordered_map<string,vector<map_helper> > * word_map);
vector<string> extract_words_from_file(const string &filename);
void add_word_to_map(const string &word,u64 document_id,size_t location,unordered_map<string, vector<map_helper>> *word_map);
void save_map(unordered_map<string,vector<map_helper> > * word_map);
//function for initlizing doc_entry structure
//here files given as doc_id_start_point and doc_id_file_name are created or truncated (error handling is also done)
void init_doc_entry(doc_entry *var_doc_entry)
{
  var_doc_entry->fp_doc_id_start_point = fopen(doc_id_start_point,"wb");
  if(!(var_doc_entry->fp_doc_id_start_point))
  {
    cerr << "can't open the doc_id_start_point file" << endl;
    exit(1);
  }
  var_doc_entry->fp_doc_id_file_name = fopen(doc_id_file_name,"w");
  if(!(var_doc_entry->fp_doc_id_file_name))
  {
    cerr << "can't open the doc_id_file_name file" << endl;
    exit(1);
  }
  return;
}

//function for adding a document_id : start pos entry in doc_id_start_point file and doc_name in doc_id_file_name file
void add_docid_name_entry(u64 document_id,string *doc_name,doc_entry *var_doc_entry)
{
  u64 data_to_add[2];
  data_to_add[0] = document_id;
  x64 fp_doc_id_file_name_len = ftell(var_doc_entry->fp_doc_id_file_name);
  if(fp_doc_id_file_name_len == -1)
  {
    cerr << "error while getting file in function length add_docid_name_entry";
    exit(1);
  }
  data_to_add[1] = (u64)fp_doc_id_file_name_len;
  fwrite(data_to_add,8,2,var_doc_entry->fp_doc_id_start_point); // adding doc_id start pos entry
  cout << "File name : " << *doc_name << endl;
  fwrite(doc_name->c_str(),doc_name->length(),1,var_doc_entry->fp_doc_id_file_name);//writing file name
  return ;
}

//function for adding word to map
void add_word_to_map(const string &word,u64 document_id,size_t location,unordered_map<string, vector<map_helper>> *word_map)
{
    auto &vec = (*word_map)[word];
    if ((vec.empty()) || (vec.back().doc_id != document_id))
    {
        map_helper h;
        h.doc_id = document_id;
        cout << "doc_id = " << h.doc_id << "\n" << endl;
        h.locations.push_back(location);
        vec.push_back(h);
    }
    else
    {
        vec.back().locations.push_back(location);
    }
}

//function of adding individual document to in-memory map 
void add_docs_map(u64 document_id,string *doc_name, unordered_map<string,vector<map_helper> > * word_map)
{
  vector<string> words = extract_words_from_file(*doc_name);
  size_t i = 0;
  size_t sz = words.size();
  while (i != sz)
  {
      add_word_to_map(words[i], document_id, i, word_map);
      i += 1;
  }
  return ;
}

//function for tokenizeing the given document returns vector of words in it
vector<string> extract_words_from_file(const string &filename)
{
  vector<string> result;
  FILE *fp = fopen(filename.c_str(), "r");
  if (fp == NULL)
  {
    cerr << "Failed to open file: " << filename << "\n";
    return result;
  }
  char buffer[extract_words_from_file_buffer_size];
  UErrorCode status = U_ZERO_ERROR;
  icu::Locale hindi("hi");

  unique_ptr<icu::BreakIterator> wordIter(
      icu::BreakIterator::createWordInstance(hindi, status)
      );

  bool iter_ok = U_SUCCESS(status);
  if (!iter_ok)
  {
    cerr << "ICU BreakIterator creation failed\n";
    fclose(fp);
    return result;
  }

  bool reading = true;
  while (reading)
  {
    char *p = fgets(buffer, sizeof(buffer), fp);
    if (p == NULL)
    {
      reading = false;
    }
    if (p != NULL)
    {
      string line(buffer);
      icu::UnicodeString uline = icu::UnicodeString::fromUTF8(line);
      wordIter->setText(uline);
      int32_t start = wordIter->first();
      int32_t end   = wordIter->next();
      bool word_running = true;
      while (word_running)
      {
        bool done = (end == icu::BreakIterator::DONE);
        if (!done)
        {
          icu::UnicodeString token = uline.tempSubStringBetween(start, end);
          icu::UnicodeString trimmed = token;
          trimmed.trim();
          bool empty = trimmed.isEmpty();
          if (!empty)
          {
            UChar32 c = trimmed.char32At(0);
            bool is_alpha = u_isalpha(c);
            if (is_alpha)
            {
              string out;
              trimmed.toUTF8String(out);
              result.push_back(out);
            }
          }
          start = end;
          end = wordIter->next();
        }
        if (done)
        {
          word_running = false;
        }
      }
    }
  }
  fclose(fp);
  return result;
}

void save_map(unordered_map<string,vector<map_helper> > * word_map)
{
  return;
}
int main() 
{
  int docs_read = 0 ;
  char buffer[BUFFER_SIZE];
  doc_entry var_doc_entry;
  init_doc_entry(&var_doc_entry);// initlizes the doc entry file pointers
  fs::directory_iterator it(folder);
  fs::directory_iterator end;  // required for termination after reading all files 
  unordered_map<string,vector<map_helper> > word_map;
  while (it != end) 
  {
    const fs::directory_entry &entry = *it;
    if (entry.is_regular_file() && entry.path().extension() == ".txt")
    {
      // if .txt needed 
      // if (entry.path().extension() == ".txt")
      string filename = entry.path().filename().string();
      add_docid_name_entry(docs_read,&filename,&var_doc_entry); // adding docid : name entry
      add_docs_map(docs_read,&filename,&word_map);
      docs_read += 1;
    }
    it++;  
    if((docs_read % max_docs_in_mem) == 0)
    {
      save_map(&word_map);
    }
  }
  save_map(&word_map);
  print_word_map(word_map);
  return 0;
}
