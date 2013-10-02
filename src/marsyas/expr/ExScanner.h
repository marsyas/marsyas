/*
** Copyright (C) 1998-2007 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#if !defined(MARSYAS_EX_SCANNER_H)
#define MARSYAS_EX_SCANNER_H

#include <marsyas/common_header.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstring>

#if _MSC_VER >= 1400
#define coco_sprintf sprintf_s
#elif _MSC_VER >= 1300
#define coco_sprintf _snprintf
#elif defined __GNUC__
#define coco_sprintf snprintf
#else
#error unknown compiler!
#endif

#define COCO_WCHAR_MAX 65535
#define MAX_BUFFER_LENGTH (64*1024)
#define HEAP_BLOCK_SIZE (64*1024)

// string handling, wide character
char* coco_string_create(const char* value);
char* coco_string_create(const char* value , int startIndex, int length);
char* coco_string_create_upper(char* data);
char* coco_string_create_lower(char* data);
char* coco_string_create_append(const char* data1, const char* data2);
char* coco_string_create_append(const char* data, const char value);
void  coco_string_delete(char* &data);
int   coco_string_length(const char* data);
bool  coco_string_endswith(char* data, char* value);
int   coco_string_indexof(char* data, char value);
int   coco_string_lastindexof(char* data, char value);
void  coco_string_merge(char* &data, char* value);
bool  coco_string_equal(char* data1, char* data2);
int   coco_string_compareto(char* data1, char* data2);
int   coco_string_hash(char* data);


namespace Marsyas {

class Token
{
public:
  int kind;     // token kind
  int pos;      // token position in the source text (starting at 0)
  int col;      // token column (starting at 0)
  int line;     // token line (starting at 1)
  char* val; // token value
  Token *next;  // ML 2005-03-11 Peek tokens are kept in linked list

  Token();
  ~Token();

};

class Buffer {
private:
  char *buf;          // input buffer
  int bufStart;       // position of first byte in buffer relative to input stream
  int bufLen;         // length of buffer
  int fileLen;        // length of input stream
  int pos;            // current position in buffer
  FILE* stream;      // input stream (seekable)
  bool isUserStream;  // was the stream opened by the user?

public:
  static const int EoF = COCO_WCHAR_MAX + 1;

  Buffer(FILE* s, bool isUserStream);
  Buffer(const char* s);
  Buffer(Buffer *b);
  virtual ~Buffer();

  virtual void Close();
  virtual int Read();
  virtual int Peek();
  virtual char* GetString(int beg, int end);
  virtual int GetPos();
  virtual void SetPos(int value);
};

class UTF8Buffer : public Buffer {
public:
  UTF8Buffer(Buffer *b) : Buffer(b) {};
  virtual int Read();
};

//-----------------------------------------------------------------------------------
// StartStates  -- maps charactes to start states of tokens
//-----------------------------------------------------------------------------------
class StartStates {
private:
  class Elem {
  public:
    int key, val;
    Elem *next;
    Elem(int key, int val) { this->key = key; this->val = val; next = NULL; }
  };

  Elem **tab;

public:
  StartStates() { tab = new Elem*[128]; memset(tab, 0, 128 * sizeof(Elem*)); }
  virtual ~StartStates() {
    for (int i = 0; i < 128; ++i) {
      Elem *e = tab[i];
      while (e != NULL) {
        Elem *next = e->next;
        delete e;
        e = next;
      }
    }
    delete [] tab;
  }

  void set(int key, int val) {
    Elem *e = new Elem(key, val);
    int k = key % 128;
    e->next = tab[k]; tab[k] = e;
  }

  int state(int key) {
    Elem *e = tab[key % 128];
    while (e != NULL && e->key != key) e = e->next;
    return e == NULL ? 0 : e->val;
  }
};

//-------------------------------------------------------------------------------------------
// KeywordMap  -- maps strings to integers (identifiers to keyword kinds)
//-------------------------------------------------------------------------------------------

class KeywordMap {
private:
  class Elem {
  public:
    char* key;
    int val;
    Elem *next;
    Elem(char* key, int val) { this->key = coco_string_create(key); this->val = val; next = NULL; }
    virtual ~Elem() { coco_string_delete(key); }
  };

  Elem **tab;

public:
  KeywordMap() { tab = new Elem*[128]; memset(tab, 0, 128 * sizeof(Elem*)); }
  virtual ~KeywordMap() {
    for (int i = 0; i < 128; ++i) {
      Elem *e = tab[i];
      while (e != NULL) {
        Elem *next = e->next;
        delete e;
        e = next;
      }
    }
    delete [] tab;
  }

  void set(char* key, int val) {
    Elem *e = new Elem(key, val);
    int k = coco_string_hash(key) % 128;
    e->next = tab[k]; tab[k] = e;
  }

  int get(char* key, int defaultVal) {
    Elem *e = tab[coco_string_hash(key) % 128];
    while (e != NULL && !coco_string_equal(e->key, key)) e = e->next;
    return e == NULL ? defaultVal : e->val;
  }
};

class ExScanner {
private:
  char EOL;
  int eofSym;
  int noSym;
  int maxT;
  int charSetSize;
  StartStates start;
  KeywordMap keywords;

  Token *t;         // current token
  char* tval;    // text of current token
  int tvalLength;   // length of text of current token
  int tlen;         // length of current token

  Token *tokens;    // list of tokens already peeked (first token is a dummy)
  Token *pt;        // current peek token
  Token* tail;      // first token in list for deleting
  int ch;           // current input character

  int pos;          // byte position of current character
  int line;         // line number of current character
  int col;          // column number of current character
  int oldEols;      // EOLs that appeared in a comment;

  Token* CreateToken();

  void Init();
  void NextCh();
  void AddCh();
  bool Comment0();
  bool Comment1();

  Token* NextToken();


public:
  Buffer *buffer;   // scanner buffer
  ExScanner(char* data, int i);
  ExScanner(char* fileName);
  ExScanner(FILE* s);
  ExScanner();
  ~ExScanner();
  Token* Scan();
  Token* Peek();
  void ResetPeek();
  void setString(const char* s);

}; // end ExScanner

}; // namespace

#endif // !defined(MARSYAS_EX_SCANNER_H)

