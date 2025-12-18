#include <iostream>
#include <fstream>
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

#define header_folder "header_folder/"
#define params_file header_folder "parmas.h"
#define my_typedefs header_folder "my_typedefs.h" 
#define print_word_map_file header_folder "print_word_map.h"
#include "header_folder/my_typedefs.h"
#include "header_folder/params.h" // use this file for giving all parameters

using namespace std;
namespace fs = filesystem;

ofstream log_file(log_file_name,ios::app);

void log_msg(const string &msg)
{
  log_file << msg << "\n" ;
  log_file.flush();
  return;
}

// structure required for adding doc_id:doc_name entry
typedef struct doc_entry {
  FILE *fp_doc_id_start_point;
  FILE *fp_doc_id_file_name;
}doc_entry;

//structure for holding document_id and vector of positions in that document for a particular word
struct helper {
  vector<u64> doc_ids;                     // doc_ids[i]
  vector<vector<u32>> positions;      // positions[i] = list of positions for doc_ids[i]
};

#include "header_folder/print_word_map.h"
void init_doc_entry(doc_entry *var_doc_entry);
void add_docid_name_entry(u64 document_id,string *doc_name,doc_entry *var_doc_entry);
void add_docs_map(u64 document_id,string *doc_name,unordered_map<string, helper>* word_map);
vector<string> extract_words_from_file(const string &filename);
void add_word_to_map(const string &word,u64 document_id,size_t location,unordered_map<string, helper>* word_map);
void save_map(unordered_map<string, helper>* word_map);
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
  fflush(var_doc_entry->fp_doc_id_start_point);                                                               
  //cout << "File name : " << *doc_name << endl;
  fwrite(doc_name->c_str(),doc_name->length(),1,var_doc_entry->fp_doc_id_file_name);//writing file name
  fflush(var_doc_entry->fp_doc_id_file_name);                                                               
  return ;
}

void add_word_to_map(const string &word,u64 document_id,size_t location,unordered_map<string, helper>* word_map)
{
  helper &h = (*word_map)[word];
  // If this word has no entries or the last doc_id is different, add a new entry
  if (h.doc_ids.empty() || h.doc_ids.back() != document_id)
  {
    h.doc_ids.push_back(document_id);
    h.positions.emplace_back();      // create an empty vector for this document
    h.positions.back().push_back(location);
  }
  else
  {
    // Same document : append position to last positions list
    h.positions.back().push_back(location);
  }
}

//function of adding individual document to in-memory map 
void add_docs_map(u64 document_id,string *doc_name,unordered_map<string, helper>* word_map)
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
  string act_filename = string(txt_folder) + filename;
  FILE *fp = fopen(act_filename.c_str(), "r");
  if (fp == NULL)
  {
    cerr << "Failed to open file: " << filename << "\n";
    cerr << "errno = " << errno << endl;
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

void save_map(unordered_map<string, helper>* word_map)
{
  unordered_map <string,helper>::iterator it = word_map->begin();
  while(it != word_map->end())
  {
    string word = it->first;
    string word_doc_id_file_name = string(save_map_locations_ids_folder) + word + "_ids";
    FILE *fp = fopen(word_doc_id_file_name.c_str(),"a");
    if (fp == NULL)
    {
      cerr << "Failed to open file: " << word_doc_id_file_name << "\n";
      return ;
    }
    helper &h = it->second;
    u64 docs_preprocessed = 0;
    u64 total_docs = h.doc_ids.size();
    u8 locations_buffer[save_map_locations_buffer_size];
    u32 loc_in_buffer = 0 ;
    u32 temp_vector_len;
    // this while block takes care of saving document_id : no of occurances
    while(docs_preprocessed != total_docs)
    {
      memcpy((locations_buffer + (save_map_locations_buffer_struct_size * docs_preprocessed)),&h.doc_ids[docs_preprocessed],doc_id_size); // copying doc_id;
      temp_vector_len = h.positions[docs_preprocessed].size();
      memcpy((locations_buffer + (save_map_locations_buffer_struct_size * docs_preprocessed)+ doc_id_size),&temp_vector_len,doc_vector_max_len_bytes); // copying number of elements;
      loc_in_buffer += 1;
      //log_msg("ids saved " + to_string(docs_preprocessed));
      docs_preprocessed += 1;
      if((loc_in_buffer == save_map_locations_buffer_members) || (docs_preprocessed == total_docs))
      {
        fwrite(locations_buffer,(save_map_locations_buffer_struct_size * loc_in_buffer),1,fp);
        fflush(fp);
        loc_in_buffer = 0;
      }
    }

    fclose(fp);
    //string word = it->first;
    string word_doc_locations_file_name = string(save_map_locations_positions_folder) + word + "_positions";
    FILE *fp_1 = fopen(word_doc_locations_file_name.c_str(),"a");
    if (fp_1 == NULL)
    {
      cerr << "Failed to open file: " << word_doc_locations_file_name << "\n";
      return ;
    }

    u32 doc_locations_buffer[save_map_positions_buffer_size];
    u32 doc_visited = 0;
    u32 total_doc_size = h.positions[0].size();
    u32 buffer_filled = 0;
    docs_preprocessed = 0;
    total_docs = h.doc_ids.size();
    u32 next_batch;
#define min(x,y) ((x) < (y)  ? (x): (y))
    while(docs_preprocessed != total_docs)
    {
      next_batch = min(((total_doc_size) - (doc_visited)),((save_map_positions_buffer_size)-(buffer_filled)));
      //copy next batch
      memcpy((doc_locations_buffer + buffer_filled),(h.positions[docs_preprocessed].data() + doc_visited),next_batch * (doc_vector_max_len_bytes));
      doc_visited += next_batch;
      buffer_filled += next_batch;
      if(doc_visited == total_doc_size)
      {
        doc_visited = 0;
        //log_msg("mem saved" + to_string(docs_preprocessed));
        docs_preprocessed += 1;
        if(docs_preprocessed != total_docs)
        {
          total_doc_size = h.positions[docs_preprocessed].size();
        }
      }
      if((buffer_filled == save_map_positions_buffer_size) || (docs_preprocessed == total_docs))
      {
        fwrite(doc_locations_buffer,doc_vector_max_len_bytes,buffer_filled,fp_1);
        fflush(fp_1);
        //cout << "write : " << word << "\n" << endl;
        buffer_filled = 0;
      }
    }
    fclose(fp_1); 
    ++it;
  }
  return;
}
int main()
{
  int docs_read = 0 ;
  char buffer[BUFFER_SIZE];
  doc_entry var_doc_entry;
  init_doc_entry(&var_doc_entry);// initlizes the doc entry file pointers
  fs::directory_iterator it(txt_folder);
  fs::directory_iterator end;  // required for termination after reading all files 
  unordered_map<string, helper> word_map;
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
    log_msg("doc " + to_string(docs_read) + " done");
    it++;  
    if(((docs_read % max_docs_in_mem) == 0) || (it == end))
    {
      save_map(&word_map);
      word_map.clear();
      log_msg("docs saved " + to_string(docs_read));
    }
  }
  //print_word_map(word_map);

  fclose(var_doc_entry.fp_doc_id_start_point);
  fclose(var_doc_entry.fp_doc_id_file_name);

  return 0;
}

