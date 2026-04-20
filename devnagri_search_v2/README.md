# Devnagri Search

Full-text search engine for Devnagri (Hindi) text documents. Builds an inverted index from a corpus of `.txt` files and enables fast multi-word queries with threshold-based document retrieval.

## Problem Statement

Given terabytes of Devnagri `.txt` documents, create an index that enables fast query responses. Query type: given a sequence of words, return documents closely related to those words.

## Features

- Unicode-aware tokenization using ICU library (Hindi locale)
- Inverted index with word position tracking
- Threshold-based multi-word search
- Memory-efficient batch processing (handles large corpora)
- Binary serialization for compact storage

## Prerequisites

- C++ compiler with C++17 support (g++)
- ICU library (libicu-dev)
- pkg-config

## Installation

```bash
sudo apt install libicu-dev
```

## Build

```bash
make clean    # Remove residual files
make          # Compile and run indexing
```

Manual compilation:
```bash
g++ make_index.cpp -o make_index `pkg-config --cflags --libs icu-uc icu-i18n`
g++ search.cpp -o search `pkg-config --cflags --libs icu-uc icu-i18n`
```

## Usage

### 1. Indexing

Place your `.txt` files in `txt_folder/` and update the path in `header_folder/common_params.h`:

```cpp
#define txt_folder folder "txt_folder/your_corpus/"
```

Run indexing:
```bash
./make_index
```

This creates index files in:
- `ds_folder/` - Document metadata
- `ids_folder/` - Word-to-document mappings
- `positions_folder/` - Word position lists
- `logs_folder/` - Indexing logs

### 2. Searching

Modify `search.cpp` main function with your query:

```cpp
vector<u64> result = get_docs(vector<string>{"word1", "word2", "word3"}, threshold);
```

Example query (already in search.cpp):
```cpp
vector<u64> result = get_docs(vector<string>{"mules", "seemed", "aroused", "sight", "pillar", "atomity"}, 6);
```

This searches for documents containing at least 6 of the 6 query words.

Compile and run:
```bash
g++ search.cpp -o search `pkg-config --cflags --libs icu-uc icu-i18n`
./search
```

Output:
```
Here main
24
```

Returns document IDs containing at least `threshold` of the query words. In the example above, document ID 24 contains all 6 words.

## Project Structure

```
.
├── make_index.cpp          # Indexing engine
├── search.cpp              # Search engine
├── makefile                # Build automation
├── install.sh              # Dependency installation
├── header_folder/          # Configuration headers
│   ├── common_params.h     # Shared parameters
│   ├── make_index_params.h # Indexing parameters
│   ├── search_params.h     # Search parameters
│   ├── my_typedefs.h       # Type definitions
│   └── print_word_map.h    # Debug utilities
├── txt_folder/             # Input corpus
├── ds_folder/              # Document metadata (generated)
├── ids_folder/             # Word-document index (generated)
├── positions_folder/       # Word positions (generated)
└── logs_folder/            # Indexing logs (generated)
```

## Configuration

Edit parameters in `header_folder/`:

**common_params.h**
- Folder paths
- Buffer sizes

**make_index_params.h**
- `max_docs_in_mem` - Documents per batch (default: 200)
- Buffer sizes for serialization

**search_params.h**
- Search buffer sizes

---

## Technical Specification

### Architecture

Two-phase system:
1. **Indexing Phase**: Processes documents, builds inverted index, serializes to disk
2. **Search Phase**: Queries pre-built index, returns matching document IDs

### Data Structures

#### In-Memory Index (Indexing Phase)

```cpp
unordered_map<string, helper>
  └─ word (string)
     └─ helper struct
        ├─ doc_ids: vector<u64>           // Document IDs
        └─ positions: vector<vector<u32>> // Word positions per document
```

#### Persistent Index (Binary Format)

**ds_folder/**

1. `doc_id_start_point` - 16-byte entries:
```
struct {
  u64 doc_id;        // Document ID
  u64 file_start;    // Byte offset in doc_id_file_name
}
```

2. `doc_id_file_name` - Concatenated filenames (no delimiters)

**ids_folder/**

One file per word: `{WORD}_ids` - 12-byte entries:
```
struct {
  u64 doc_id;           // Document ID
  u32 list_pos_start;   // Starting position in positions file
                        // Multiply by 4 for byte offset
                        // Count = next_doc.list_pos_start - current.list_pos_start
}
```

**positions_folder/**

One file per word: `{WORD}_positions` - 4-byte entries:
```
u32 position;  // Word position within document
```

Positions are grouped by document. Use `ids_folder/{WORD}_ids` to determine boundaries.

### Indexing Algorithm

1. Initialize document metadata files
2. For each document in `txt_folder/`:
   - Assign sequential doc_id
   - Store filename in `doc_id_file_name`
   - Record doc_id and file offset in `doc_id_start_point`
3. Tokenize document using ICU BreakIterator (Hindi locale)
4. Add words to in-memory map with positions
5. Every `max_docs_in_mem` documents:
   - Serialize word_map to `ids_folder/` and `positions_folder/`
   - Clear in-memory map
6. Final batch serialization

### Search Algorithm

1. Initialize document counter map
2. For each query word:
   - Read `ids_folder/{word}_ids`
   - Increment counter for each document containing word
3. Progressive filtering:
   - After processing enough words, prune documents below threshold
4. Return surviving document IDs

### Performance Characteristics

- **Indexing**: O(N × M) where N = documents, M = avg words per document
- **Search**: O(K × W) where K = documents per word, W = query words
- **Memory**: Bounded by `max_docs_in_mem` parameter
- **Storage**: ~6MB per 1,000 unique words (76K words = 620MB)

### Limitations

- Search returns document IDs only (no ranking or snippets)
- No phrase search (word order ignored)
- Threshold is exact match count (no fuzzy matching)
- Requires full re-indexing for corpus updates
