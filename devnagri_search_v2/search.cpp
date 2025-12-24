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
#include <sys/stat.h>

#include <vector>
#include <unordered_map>
#include <map>

#include "header_folder/search_params.h"
#include "header_folder/my_typedefs.h"

using namespace std;

vector<u64> get_docs(const vector<string> &words);
void add_word_docs(unordered_map<u64,u16> &docs_map,const string &word);
void remove_docs(unordered_map<u64,u16> &docs_map,u16 threshold);

vector<u64> get_docs(const vector<string> &words,u16 threshold)
{
  unordered_map<u64,u16> docs_map;
  u16 total_words = words.size();
  u16 words_processed = 0 ;
  #define min(a,b) (a < b ? a : b)
  while (words_processed != total_words)
  {
    add_word_docs(docs_map,words[words_processed]);
    words_processed += 1;
    if(words_processed > (total_words - threshold + 1))
    {
      remove_docs(docs_map,(u16)(words_processed - (total_words - threshold)));
      cout << "here 2" << endl;
    }
  }
  // getting vector of documents 
  vector<u64> output;
  output.reserve(docs_map.size());
  unordered_map<u64,u16>::iterator itr = docs_map.begin();
  while(itr != docs_map.end())
  {
    output.push_back(itr->first);
    itr++;
  }
  return output;
}

void add_word_docs(unordered_map<u64,u16> &docs_map,const string &word)
{
    u8 docs_buffer[get_docs_buffer_size];
    u16 members_read;
    u16 members_preprocessed;
    string file_name = string(ids_folder) + word + string("_ids");    
    FILE *fp = fopen(file_name.c_str(),"r");
    if (fp == NULL)
    {
      cerr << "Failed to open file: " << file_name << "\n";
      return;
    }
    while(!feof(fp))
    {
      members_preprocessed = 0; 
      members_read = fread(docs_buffer,get_docs_buffer_struct_size,get_docs_buffer_members,fp); 
      cout << "members_read = " << members_read << endl;
      //members_read /= get_docs_buffer_struct_size;
      //cout << "members_read = " << members_read << endl;
      while(members_preprocessed != members_read) 
      {
        docs_map[(*(u64 *)&docs_buffer[(members_preprocessed * get_docs_buffer_struct_size)])] += 1;
        cout << "doc_id = " << (*(u64 *)&docs_buffer[(members_preprocessed * get_docs_buffer_struct_size)]) << endl;
        members_preprocessed += 1;
      }
    }
    fclose(fp);
    return;
}

void remove_docs(unordered_map<u64,u16> &docs_map,u16 threshold)
{
      cout << "here 1" << endl;
  unordered_map<u64,u16>::iterator itr = docs_map.begin();
  while(itr != docs_map.end())
  {
    if(itr->second < threshold)
    {
      itr = docs_map.erase(itr);
    }
    else if(itr->second >= threshold)
    {
      itr++;
    }
  }
  return;
}

int main()
{
  std::vector<u64> result = get_docs(std::vector<std::string>{"Hii", "hello","how"}, 2);
  cout << "Here main" << endl;
  int itr = 0;
  while(itr != result.size())
  {
    cout << result[itr] << endl;
    itr += 1;
  }
  return 0;
}

