void print_word_map(const unordered_map<string, vector<map_helper>> &word_map)
{
  unordered_map<string, vector<map_helper>>::const_iterator it = word_map.begin();
  unordered_map<string, vector<map_helper>>::const_iterator end = word_map.end();
  while (it != end)
  {
    const string &word = it->first;
    const vector<map_helper> &doc_list = it->second;
    cout << "Word: " << word << "\n";
    size_t di = 0;
    size_t dsz = doc_list.size();
    while (di != dsz)
    {
      const map_helper &h = doc_list[di];
      cout << "  DocID: " << h.doc_id << " Locations: ";
      size_t li = 0;
      size_t lsz = h.locations.size();
      while (li != lsz)
      {
        cout << h.locations[li];
        if (li + 1 != lsz)
        {
          cout << ", ";
        }
        li += 1;
      }
      cout << "\n";
      di += 1;
    }
    it++;
  }
  return;
}
