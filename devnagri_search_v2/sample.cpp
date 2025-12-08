#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <memory>

#include <unicode/unistr.h>
#include <unicode/brkiter.h>
#include <unicode/uchar.h>

using namespace std;

// ----------------------------------------------------------------------
// Combined logic: read lines via fgets + extract Devanagari words
// Returns vector<string> of all words in file
// ----------------------------------------------------------------------
vector<string> extract_words_from_file(const string &filename)
{
  vector<string> result;

  FILE *fp = fopen(filename.c_str(), "r");
  if (fp == NULL)
  {
    cerr << "Failed to open file: " << filename << "\n";
    return result;
  }

  char buffer[4096];

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

// ----------------------------------------------------------------------
// Wrapper function: Just calls extract_words_from_file
// Returns vector of words
// ----------------------------------------------------------------------
vector<string> process_file(const string &filename)
{
  vector<string> words = extract_words_from_file(filename);
  return words;
}

// ----------------------------------------------------------------------
int main()
{
  vector<string> words = process_file("sample.txt");

  bool printing = true;
  size_t i = 0;
  size_t sz = words.size();

  while (printing)
  {
    if (i < sz)
    {
      cout << words[i] << "\n";
      i = i + 1;
    }
    if (i >= sz)
    {
      printing = false;
    }
  }

  return 0;
}
