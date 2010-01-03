/*-------------------------------*- C++ -*---------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than
Coco/R itself) does not fall under the GNU General Public License.
-----------------------------------------------------------------------*/


#ifndef COCO_SCANNER_H__
#define COCO_SCANNER_H__

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <string>
#include <fstream>
#include <iostream>

#if _MSC_VER >= 1400
#define coco_swprintf swprintf_s
#elif _MSC_VER >= 1300
#define coco_swprintf _snwprintf
#else
// assume every other compiler knows swprintf
#define coco_swprintf swprintf
#endif


#define COCO_WCHAR_MAX    65535


namespace Coco {



// * * * * * * * * * *  Wide Character String Routines * * * * * * * * * * * //

//
// string handling, wide character
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! Create by copying str
wchar_t* coco_string_create(const wchar_t* str);

//! Create a substring of str starting at index and length characters long
wchar_t* coco_string_create(const wchar_t* str, int index, int length);

//! Create a lowercase string from str
wchar_t* coco_string_create_lower(const wchar_t* str);

//! Create a lowercase substring of str starting at index and length characters long
wchar_t* coco_string_create_lower(const wchar_t* str, int index, int length);

//! Create a string by concatenating str1 and str2
wchar_t* coco_string_create_append(const wchar_t* str1, const wchar_t* str2);

//! Create a string by concatenating a character to the end of str
wchar_t* coco_string_create_append(const wchar_t* str, const wchar_t ch);

//! Free storage and nullify the argument
void  coco_string_delete(wchar_t* &str);

//! The length of the str, or 0 if the str is NULL
int   coco_string_length(const wchar_t* str);

//! Return the index of the first occurrence of ch.
//  Return -1 if nothing is found.
int   coco_string_indexof(const wchar_t* str, const wchar_t ch);

//! Compare strings, return true if they are equal
bool  coco_string_equal(const wchar_t* str1, const wchar_t* str2);

//! Simple string hashing function
int   coco_string_hash(const wchar_t* str);

//
// String conversions
// ~~~~~~~~~~~~~~~~~~

//! Convert wide string to double
double coco_string_toDouble(const wchar_t* str);

//! Convert wide string to float
float coco_string_toFloat(const wchar_t* str);

//
// String handling, byte character
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! Create a byte string by copying str
char* coco_string_create_char(const wchar_t* str);

//! Create a byte substring of str starting at index and length characters long
char* coco_string_create_char(const wchar_t* str, int index, int length);

//! Free storage and nullify the argument
void  coco_string_delete(char* &str);


// * * * * * * * * * End of Wide Character String Routines * * * * * * * * * //


//! Scanner Token
class Token
{
public:
	int kind;       //!< token kind
	int pos;        //!< token position in the source text (starting at 0)
	int col;        //!< token column (starting at 1)
	int line;       //!< token line (starting at 1)
	wchar_t* val;   //!< token value
	Token *next;    //!< Peek tokens are kept in linked list

	Token();        //!< Construct null
	~Token();       //!< Destructor - cleanup allocated val??
};


//! Scanner Buffer
//
//! This Buffer supports the following cases:
//! -# seekable stream (file)
//!    -# whole stream in buffer
//!    -# part of stream in buffer
//! -# non seekable stream (network, console)
class Buffer {
private:
	unsigned char *buf; //!< input buffer
	int bufCapacity;    //!< capacity of buf
	int bufLen;         //!< length of buffer
	int bufPos;         //!< current position in buffer
	int bufStart;       //!< position of first byte in buffer relative to input stream
	int fileLen;        //!< length of input stream (may change if the stream is no file)
	FILE* cStream;      //!< input stdio stream (normally seekable)
	std::istream* stdStream;  //!< STL std stream (seekable)
	bool isUserStream_;  //!< was the stream opened by the user?

	int ReadNextStreamChunk();
	bool CanSeek() const; //!< true if stream can be seeked otherwise false

protected:
	Buffer(Buffer*);    //!< for the UTF8Buffer

public:
	static const int EoF = COCO_WCHAR_MAX + 1;

	//! Attach buffer to a stdio stream.
	//! User streams are not closed in the destructor
	Buffer(FILE*, bool isUserStream = true);

	//! Attach buffer to an STL std stream
	//! User streams are not closed in the destructor
	explicit Buffer(std::istream*, bool isUserStream = true);

	//! Copy buffer contents from constant string
	//! Handled internally as an istringstream
	explicit Buffer(std::string&);

	//! Copy buffer contents from constant character string
	Buffer(const unsigned char* chars, int len);
	//! Copy buffer contents from constant character string
	Buffer(const char* chars, int len);

	//! Close stream (but not user streams) and free buf (if any)
	virtual ~Buffer();

	virtual void Close();   //!< Close stream (but not user streams)
	virtual int Read();     //!< Get character from stream or buffer
	virtual int Peek();     //!< Peek character from stream or buffer

	virtual int GetPos() const;
	virtual void SetPos(int value);
};


//! A Scanner buffer that handles UTF-8 characters
class UTF8Buffer : public Buffer {
public:
	UTF8Buffer(Buffer* b) : Buffer(b) {}
	virtual int Read();
};


//------------------------------------------------------------------------------
// StartStates
//------------------------------------------------------------------------------
//! maps characters (integers) to start states of tokens
class StartStates {
	class Elem {
	public:
		int key, val;
		Elem *next;
		Elem(int k, int v) :
			key(k), val(v), next(0)
		{}
	};

	Elem **tab;

public:
	StartStates() :
		tab(new Elem*[128])
	{
		memset(tab, 0, 128*sizeof(Elem*));
	}

	virtual ~StartStates() {
		for (int i = 0; i < 128; ++i) {
			Elem *e = tab[i];
			while (e) {
				Elem *next = e->next;
				delete e;
				e = next;
			}
		}
		delete [] tab;
	}

	void set(int key, int val) {
		Elem *e = new Elem(key, val);
		const int k = unsigned(key) % 128;
		e->next = tab[k];
		tab[k] = e;
	}

	int state(int key) {
		Elem *e = tab[unsigned(key) % 128];
		while (e && e->key != key) e = e->next;
		return e ? e->val : 0;
	}
};


//------------------------------------------------------------------------------
// KeywordMap
//------------------------------------------------------------------------------
//! maps strings to integers (identifiers to keyword kinds)
class KeywordMap {
	class Elem {
	public:
		wchar_t *key;
		int val;
		Elem *next;
		Elem(const wchar_t *k, int v) :
			key(coco_string_create(k)), val(v), next(0)
		{}
		virtual ~Elem() {
			coco_string_delete(key);
		}
	};

	Elem **tab;

public:
	KeywordMap() :
		tab(new Elem*[128])
	{
		memset(tab, 0, 128*sizeof(Elem*));
	}

	virtual ~KeywordMap() {
		for (int i = 0; i < 128; ++i) {
			Elem *e = tab[i];
			while (e) {
				Elem *next = e->next;
				delete e;
				e = next;
			}
		}
		delete [] tab;
	}

	void set(const wchar_t *key, int val) {
		Elem *e = new Elem(key, val);
		const int k = coco_string_hash(key) % 128;
		e->next = tab[k];
		tab[k] = e;
	}

	int get(const wchar_t *key, int defaultVal) {
		Elem *e = tab[coco_string_hash(key) % 128];
		while (e && !coco_string_equal(e->key, key)) e = e->next;
		return e ? e->val : defaultVal;
	}
};


//! A Coco/R Scanner
class Scanner {
private:
	static const int maxT = 43;
	static const int noSym = 43;

	static const int eofSym = 0;    //!< end-of-file token id
	static const char EOL = '\n';   //!< end-of-line character

	void *firstHeap;  //!< the start of the heap management
	void *heap;       //!< the currently active block
	void *heapTop;    //!< the top of the heap
	void **heapEnd;   //!< the end of the last heap block

	StartStates start;   //!< A map of start states for particular characters
	KeywordMap keywords; //!< A hash of keyword literals to token kind

	Token *t;         //!< current token
	wchar_t *tval;    //!< text of current token
	int tvalLength;   //!< maximum capacity (length) for tval
	int tlen;         //!< length of tval

	Token *tokens;    //!< list of tokens already peeked (first token is a dummy)
	Token *pt;        //!< current peek token

	int ch;           //!< current input character

	int pos;          //!< byte position of current character
	int line;         //!< line number of current character
	int col;          //!< column number of current character
	int oldEols;      //!< the number of EOLs that appeared in a comment

	void CreateHeapBlock();       //!< add a heap block, freeing unused ones
	Token* CreateToken();         //!< fit token on the heap
	void AppendVal(Token* tok);   //!< adjust tok->val to point to the heap and copy tval into it

	void Init();      //!< complete the initialization for the constructors
	void NextCh();    //!< get the next input character into ch
	void AddCh();     //!< append the character ch to tval
	bool Comment0();
	bool Comment1();

	Token* NextToken();  //!< get the next token

public:
	//! The scanner buffer
	Buffer *buffer;

	//! Using an existing open file handle for the scanner
	Scanner(FILE*);

	//! Using an existing open STL std stream
	explicit Scanner(std::istream&);

	//! Open a file for reading and attach scanner
	explicit Scanner(const wchar_t* fileName);

	//! Attach scanner to an existing character buffer
	Scanner(const unsigned char* chars, int len);
	//! Attach scanner to an existing character buffer
	Scanner(const char* chars, int len);

	~Scanner();        //!< free heap and allocated memory
	Token* Scan();     //!< get the next token (possibly a token already seen during peeking)
	Token* Peek();     //!< peek for the next token, ignore pragmas
	void ResetPeek();  //!< ensure that peeking starts at the current scan position

}; // end Scanner

} // End namespace


#endif // COCO_SCANNER_H__

