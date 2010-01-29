/*---------------------------------------------------------------------------*\
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

License
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
\*---------------------------------------------------------------------------*/

#include <sstream>

// io.h and fcntl are used to ensure binary read from streams on windows
#if _MSC_VER >= 1300
#include <io.h>
#include <fcntl.h>
#endif

#include <climits>

#include "Scanner.h"

// values for the file stream buffering
#define MIN_BUFFER_LENGTH 1024        // 1KB
#define MAX_BUFFER_LENGTH (64*MIN_BUFFER_LENGTH)   // 64KB
// value for the heap management
#define HEAP_BLOCK_SIZE   (64*1024)   // 64KB


namespace Coco {

// * * * * * * * * * *  Wide Character String Routines * * * * * * * * * * * //

// string handling, wide character

wchar_t* coco_string_create(const wchar_t* str)
{
	const int len = coco_string_length(str);
	wchar_t* dest = new wchar_t[len + 1];
	if (len)
	{
		wcsncpy(dest, str, len);
	}
	dest[len] = 0;
	return dest;
}


wchar_t* coco_string_create(const wchar_t* str, int index, int length)
{
	const int len = (str && *str) ? length : 0;
	wchar_t* dest = new wchar_t[len + 1];
	if (len)
	{
		wcsncpy(dest, &(str[index]), len);
	}
	dest[len] = 0;
	return dest;
}


wchar_t* coco_string_create_lower(const wchar_t* str)
{
	return str ? coco_string_create_lower(str, 0, wcslen(str)) : NULL;
}


wchar_t* coco_string_create_lower(const wchar_t* str, int index, int len)
{
	if (!str) { return NULL; }
	wchar_t* dest = new wchar_t[len + 1];

	for (int i = 0; i < len; ++i)
	{
		const wchar_t ch = str[index + i];
		if (ch >= 'A' && ch <= 'Z')
		{
			dest[i] = ch + ('a' - 'A');   // lower-case
		}
		else
		{
			dest[i] = ch;
		}
	}
	dest[len] = '\0';
	return dest;
}


bool coco_string_equal(const wchar_t* str1, const char* str2)
{
	const int str1Len = coco_string_length(str1);
	const int str2Len = coco_string_length(str2);

	if (str1Len != str2Len)
	{
		return false;
	}

	for (int i = 0; i < str1Len; ++i)
	{
		if (str1[i] != wchar_t(str2[i]))
		{
			return false;
		}
	}

	return true;
}


//
// string handling, byte character
//

std::string coco_stdString(const wchar_t* str)
{
    return str ? coco_stdString(str, 0, wcslen(str)) : std::string();
}


std::string coco_stdString
(
    const wchar_t* str,
    int index,
    int length
)
{
    const int len = (str && *str) ? length : 0;
    std::string dest;
    dest.reserve(len);

    for (int i = 0; i < len; ++i)
    {
        dest += char(str[index+i] & 0xFF);
    }

    return dest;
}


std::string coco_stdStringUTF8(const wchar_t* str)
{
    return str ? coco_stdStringUTF8(str, 0, wcslen(str)) : std::string();
}


std::string coco_stdStringUTF8
(
    const wchar_t* str,
    int index,
    int length
)
{
    const int len = (str && *str) ? length : 0;
    std::string dest;
    dest.reserve(len);


    for (int i = 0; i < len; ++i)
    {
        wchar_t wc = str[index+i];

        if (!(wc & ~0x0000007F))
        {
            // 0x00000000 - 0x0000007F [min. 8bit storage, 1-byte encoding)
            // 0aaaaaaa
            dest += char(wc);
        }
        else if (!(wc & ~0x000007FF))
        {
            // 0x00000080 - 0x000007FF [min. 16bit storage, 2-byte encoding]
            // 110bbbaa 10aaaaaa
            dest += char(0xC0 | ((wc >> 6) & 0x1F));
            dest += char(0x80 | ((wc) & 0x3F));
        }
        else if (!(wc & ~0x0000FFFF))
        {
            // 0x00000800 - 0x0000FFFF [min. 16bit storage, 3-byte encoding]
            // 1110bbbb 10bbbbaa 10aaaaaa
            dest += char(0xE0 | ((wc >> 12) & 0x0F));
            dest += char(0x80 | ((wc >> 6) & 0x3F));
            dest += char(0x80 | ((wc) & 0x3F));
        }
        else if (!(wc & ~0x001FFFFF))
        {
            // 0x00010000 - 0x001FFFFF [min. 24bit storage, 4-byte encoding]
            // 11110ccc 10ccbbbb 10bbbbaa 10aaaaaa
            dest += char(0xF0 | ((wc >> 18) & 0x07));
            dest += char(0x80 | ((wc >> 12) & 0x3F));
            dest += char(0x80 | ((wc >> 6) & 0x3F));
            dest += char(0x80 | ((wc) & 0x3F));
        }
//
// Not (yet) used - wchar_t storage is limited to 16bit on windows
// This also corresponds to the unicode BMP (Basic Multilingual Plane)
//
//        else if (!(wc & ~0x03FFFFFF))
//        {
//            // 0x00200000 - 0x03FFFFFF [min. 32bit storage, 5-byte encoding]
//            // 111110dd 10cccccc 10ccbbbb 10bbbbaa 10aaaaaa
//            dest += char(0xF8 | ((wc >> 24) & 0x03));
//            dest += char(0x80 | ((wc >> 18) & 0x3F));
//            dest += char(0x80 | ((wc >> 12) & 0x3F));
//            dest += char(0x80 | ((wc >> 6) & 0x3F));
//            dest += char(0x80 | ((wc) & 0x3F));
//        }
//        else if (!(wc & ~0x7FFFFFFF))
//        {
//            // 0x04000000 - 0x7FFFFFFF [min. 32bit storage, 6-byte encoding]
//            // 1111110d 10dddddd 10cccccc 10ccbbbb 10bbbbaa 10aaaaaa
//            dest += char(0xFC | ((wc >> 30) & 0x01));
//            dest += char(0x80 | ((wc >> 24) & 0x3F));
//            dest += char(0x80 | ((wc >> 18) & 0x3F));
//            dest += char(0x80 | ((wc >> 12) & 0x3F));
//            dest += char(0x80 | ((wc >> 6) & 0x3F));
//            dest += char(0x80 | ((wc) & 0x3F));
//        }
//
        else
        {
            // report anything unknown/invalid as replacement character U+FFFD
            dest += char(0xEF);
            dest += char(0xBF);
            dest += char(0xBD);
        }
    }

    return dest;
}


// * * * * * * * * * End of Wide Character String Routines * * * * * * * * * //

Token::Token()
:
    kind(0),
    pos(0),
    col(0),
    line(0),
    val(NULL),
    next(NULL)
{}


// Note: this delete may not be correct if the token was actually
// allocated by the internal heap mechanism
Token::~Token()
{
	coco_string_delete(val);
}


// ----------------------------------------------------------------------------
// Buffer Implementation
// ----------------------------------------------------------------------------

Buffer::Buffer(Buffer* b)
:
	buf(b->buf),
	bufCapacity(b->bufCapacity),
	bufLen(b->bufLen),
	bufPos(b->bufPos),
	bufStart(b->bufStart),
	fileLen(b->fileLen),
	cStream(b->cStream),
	stdStream(b->stdStream),
	isUserStream_(b->isUserStream_)
{
	// avoid accidental deletion on any of these members
	b->buf = NULL;
	b->cStream = NULL;
	b->stdStream = NULL;
}


Buffer::Buffer(FILE* istr, bool isUserStream)
:
	buf(NULL),
	bufCapacity(0),
	bufLen(0),
	bufPos(0),
	bufStart(0),
	fileLen(0),
	cStream(istr),
	stdStream(NULL),
	isUserStream_(isUserStream)
{
// ensure binary read on windows
#if _MSC_VER >= 1300
	_setmode(_fileno(cStream), _O_BINARY);
#endif

	if (CanSeek())
	{
		fseek(cStream, 0, SEEK_END);
		fileLen = ftell(cStream);
		fseek(cStream, 0, SEEK_SET);
		bufLen = (fileLen < MAX_BUFFER_LENGTH) ? fileLen : MAX_BUFFER_LENGTH;
		bufStart = INT_MAX; // nothing in the buffer so far
	}

	bufCapacity = (bufLen > 0) ? bufLen : MIN_BUFFER_LENGTH;
	buf = new unsigned char[bufCapacity];
	if (fileLen > 0) SetPos(0);          // setup buffer to position 0 (start)
	else bufPos = 0; // index 0 is already after the file, thus Pos = 0 is invalid
	if (bufLen == fileLen && CanSeek()) Close();
}


Buffer::Buffer(std::istream* istr, bool isUserStream)
:
	buf(NULL),
	bufCapacity(0),
	bufLen(0),
	bufPos(0),
	bufStart(0),
	fileLen(0),
	cStream(NULL),
	stdStream(istr),
	isUserStream_(isUserStream)
{
	// ensure binary read on windows
#if _MSC_VER >= 1300
	// TODO
#endif
}


Buffer::Buffer(const char* chars, int len)
:
	buf(new unsigned char[len]),
	bufCapacity(len),
	bufLen(len),
	bufPos(0),
	bufStart(0),
	fileLen(len),
	cStream(NULL),
	stdStream(NULL),
	isUserStream_(false)
{
	memcpy(this->buf, chars, len*sizeof(char));
}


Buffer::Buffer(const unsigned char* chars, int len)
:
	buf(new unsigned char[len]),
	bufCapacity(len),
	bufLen(len),
	bufPos(0),
	bufStart(0),
	fileLen(len),
	cStream(NULL),
	stdStream(NULL),
	isUserStream_(false)
{
	memcpy(this->buf, chars, len*sizeof(char));
}


Buffer::~Buffer()
{
	Close();
	if (buf)
	{
		delete[] buf;
		buf = NULL;
	}
}


void Buffer::Close()
{
	if (!isUserStream_)
	{
		if (cStream)
		{
			fclose(cStream);
			cStream = NULL;
		}
		else if (stdStream)
		{
			delete stdStream;
			stdStream = 0;
		}
	}
}


int Buffer::Read()
{
	if (stdStream)
	{
		int ch = stdStream->get();
		if (stdStream->eof())
		{
			return EoF;
		}
		return ch;
	}

	if (bufPos < bufLen) {
		return buf[bufPos++];
	}
	else if (GetPos() < fileLen) {
		SetPos(GetPos()); // shift buffer start to Pos
		return buf[bufPos++];
	}
	else if (cStream && !CanSeek() && (ReadNextStreamChunk() > 0)) {
		return buf[bufPos++];
	}
	else {
		return EoF;
	}
}


int UTF8Buffer::Read()
{
	int ch;
	do {
		ch = Buffer::Read();
		// until we find a utf8 start (0xxxxxxx or 11xxxxxx)
	} while (ch != EoF && ch >= 128 && ((ch & 0xC0) != 0xC0));
	if (ch < 128 || ch == EoF) {
		// nothing to do, first 127 chars are identical in ASCII and UTF8
		// 0xxxxxxx or end of file character
	}
	else if ((ch & 0xF0) == 0xF0) {
		// 0x00010000 - 0x001FFFFF [min. 24bit storage, 4-byte encoding]
		// 11110ccc 10ccbbbb 10bbbbaa 10aaaaaa
		// CAUTION: this should probably be disallowed since it overflows
		// wchar_t on windows and overflows the max (0xFFFF) used here
		int c1 = ch & 0x07; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F; ch = Buffer::Read();
		int c4 = ch & 0x3F;
		ch = (((((c1 << 6) | c2) << 6) | c3) << 6) | c4;
	}
	else if ((ch & 0xE0) == 0xE0) {
		// 0x00000800 - 0x0000FFFF [min. 16bit storage, 3-byte encoding]
		// 1110bbbb 10bbbbaa 10aaaaaa
		int c1 = ch & 0x0F; ch = Buffer::Read();
		int c2 = ch & 0x3F; ch = Buffer::Read();
		int c3 = ch & 0x3F;
		ch = (((c1 << 6) | c2) << 6) | c3;
	}
	else if ((ch & 0xC0) == 0xC0) {
		// 0x00000080 - 0x000007FF [min. 16bit storage, 2-byte encoding]
		// 110bbbaa 10aaaaaa
		int c1 = ch & 0x1F; ch = Buffer::Read();
		int c2 = ch & 0x3F;
		ch = (c1 << 6) | c2;
	}
	return ch;
}


bool Buffer::isUTF8() const
{
	return false;
}


bool UTF8Buffer::isUTF8() const
{
	return true;
}


int Buffer::Peek()
{
	int curPos = GetPos();
	int ch = Read();
	SetPos(curPos);
	return ch;
}


int Buffer::GetPos() const
{
	if (stdStream)
	{
		return stdStream->tellg();
	}

	return bufPos + bufStart;
}


void Buffer::SetPos(int value)
{
	if (stdStream)
	{
		stdStream->seekg(value, std::ios::beg);
		return;
	}

	if ((value >= fileLen) && cStream && !CanSeek())
	{
		// Wanted position is after buffer and the stream
		// is not seek-able e.g. network or console,
		// thus we have to read the stream manually till
		// the wanted position is in sight.
		while ((value >= fileLen) && (ReadNextStreamChunk() > 0))
		{}
	}

	if ((value < 0) || (value > fileLen))
	{
		wprintf(L"--- buffer out of bounds access, position: %d\n", value);
		::exit(1);
	}

	if ((value >= bufStart) && (value < (bufStart + bufLen))) // already in buffer
	{
		bufPos = value - bufStart;
	}
	else if (cStream) // must be swapped in
	{
		fseek(cStream, value, SEEK_SET);
		bufLen = fread(buf, sizeof(char), bufCapacity, cStream);
		bufStart = value; bufPos = 0;
	}
	else
	{
		bufPos = fileLen - bufStart; // make Pos return fileLen
	}
}


// Read the next chunk of bytes from the stream, increases the buffer
// if needed and updates the fields fileLen and bufLen.
// Returns the number of bytes read.
int Buffer::ReadNextStreamChunk()
{
	int freeLen = bufCapacity - bufLen;
	if (freeLen == 0)
	{
		// in the case of a growing input stream
		// we can neither seek in the stream, nor can we
		// foresee the maximum length, thus we must adapt
		// the buffer size on demand.
		bufCapacity = bufLen * 2;
		unsigned char *newBuf = new unsigned char[bufCapacity];
		memcpy(newBuf, buf, bufLen*sizeof(char));
		delete[] buf;
		buf = newBuf;
		freeLen = bufLen;
	}
	int read = fread(buf + bufLen, sizeof(char), freeLen, cStream);
	if (read > 0)
	{
		fileLen = bufLen = (bufLen + read);
		return read;
	}
	// end of stream reached
	return 0;
}


bool Buffer::CanSeek() const
{
	return cStream && (ftell(cStream) != -1);
}


// ----------------------------------------------------------------------------
// Scanner Implementation
// ----------------------------------------------------------------------------

Scanner::Scanner(FILE* istr)
:
	buffer(new Buffer(istr, true))
{
	Init();
}


Scanner::Scanner(std::istream& istr)
:
	buffer(new Buffer(&istr, true))
{
	Init();
}


Scanner::Scanner(const std::string& fileName)
{
	FILE* istr;
	if ((istr = fopen(fileName.c_str(), "rb")) == NULL)
	{
		wprintf(L"--- Cannot open file %s\n", fileName.c_str());
		::exit(1);
	}
	buffer = new Buffer(istr, false);
	Init();
}

#ifdef _WIN32
Scanner::Scanner(const std::wstring& fileName)
{
	FILE* istr;

	if ((istr = _wfopen(fileName.c_str(), L"rb")) == NULL)
	{
		wprintf(L"--- Cannot open file %ls\n", fileName.c_str());
		::exit(1);
	}
	buffer = new Buffer(istr, false);
	Init();
}
#endif


Scanner::Scanner(const unsigned char* buf, int len)
:
	buffer(new Buffer(buf, len))
{
	Init();
}


Scanner::Scanner(const char* buf, int len)
:
	buffer(new Buffer(buf, len))
{
	Init();
}


Scanner::~Scanner()
{
	char* cur = reinterpret_cast<char*>(firstHeap);

	while (cur)
	{
		cur = *(reinterpret_cast<char**>(cur + HEAP_BLOCK_SIZE));
		free(firstHeap);
		firstHeap = cur;
	}
	delete[] tval;
	delete buffer;
}


void Scanner::Init()
{
	for (int i = 65; i <= 90; ++i) start.set(i, 1);
	for (int i = 95; i <= 95; ++i) start.set(i, 1);
	for (int i = 97; i <= 122; ++i) start.set(i, 1);
	for (int i = 48; i <= 57; ++i) start.set(i, 2);
	start.set(34, 12);
	start.set(39, 5);
	start.set(36, 13);
	start.set(91, 80);
	start.set(61, 66);
	start.set(46, 81);
	start.set(43, 67);
	start.set(45, 68);
	start.set(60, 82);
	start.set(62, 70);
	start.set(124, 73);
	start.set(40, 83);
	start.set(41, 74);
	start.set(93, 75);
	start.set(123, 76);
	start.set(125, 77);
	start.set(Buffer::EoF, -1);

	keywords.set(L"COMPILER", 8);
	keywords.set(L"IGNORECASE", 15);
	keywords.set(L"CHARACTERS", 16);
	keywords.set(L"TOKENS", 17);
	keywords.set(L"PRAGMAS", 18);
	keywords.set(L"COMMENTS", 19);
	keywords.set(L"FROM", 20);
	keywords.set(L"TO", 21);
	keywords.set(L"NESTED", 22);
	keywords.set(L"IGNORE", 23);
	keywords.set(L"PRODUCTIONS", 24);
	keywords.set(L"END", 27);
	keywords.set(L"ANY", 31);
	keywords.set(L"WEAK", 37);
	keywords.set(L"SYNC", 44);
	keywords.set(L"IF", 45);
	keywords.set(L"CONTEXT", 46);

	tvalLength = 128;
	tval = new wchar_t[tvalLength]; // text of current token
	tlen = 0;
	tval[tlen] = 0;

	// HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	heap = malloc(HEAP_BLOCK_SIZE + sizeof(void*));
	firstHeap = heap;
	heapEnd =
		reinterpret_cast<void**>
		(reinterpret_cast<char*>(heap) + HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heapTop = heap;
	if (sizeof(Token) > HEAP_BLOCK_SIZE)
	{
		wprintf(L"--- Too small HEAP_BLOCK_SIZE\n");
		::exit(1);
	}

	pos = -1; line = 1; col = 0;
	oldEols = 0;
	NextCh();
	if (ch == 0xEF)   // check optional byte order mark for UTF-8
	{                 // Windows-specific magic
		NextCh(); int ch1 = ch;
		NextCh(); int ch2 = ch;
		if (ch1 != 0xBB || ch2 != 0xBF)
		{
			wprintf(L"Illegal byte order mark at start of file");
			::exit(1);
		}
		Buffer *oldBuf = buffer;
		buffer = new UTF8Buffer(oldBuf); col = 0;
		delete oldBuf; oldBuf = NULL;
		NextCh();
	}
	// likely need this chunk of code for non-windows systems, they don't
	// rely on a byte order mark. Should make it selectable via a directive
	// or command-line option etc.

	pt = tokens = CreateToken(); // first token is a dummy
}


void Scanner::NextCh()
{
	if (oldEols > 0)
	{
		ch = EOL;
		oldEols--;
	}
	else
	{
		pos = buffer->GetPos();
		ch = buffer->Read(); col++;
		// replace isolated '\r' by '\n' in order to make
		// eol handling uniform across Windows, Unix and Mac
		if (ch == '\r' && buffer->Peek() != '\n') ch = EOL;
		if (ch == EOL) { line++; col = 0; }
	}
}


void Scanner::AddCh()
{
	if (tlen >= tvalLength)
	{
		tvalLength *= 2;
		wchar_t *newBuf = new wchar_t[tvalLength];
		memcpy(newBuf, tval, tlen*sizeof(wchar_t));
		delete[] tval;
		tval = newBuf;
	}
	if (ch != Buffer::EoF)
	{
		tval[tlen++] = ch;
		NextCh();
	}
}



bool Scanner::Comment0() {
	int level = 1, pos0 = pos, line0 = line, col0 = col;
	NextCh();
	if (ch == '/') {
		NextCh();
		for(;;) {
			if (ch == 10) {
				level--;
				if (level == 0) { oldEols = line - line0; NextCh(); return true; }
				NextCh();
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0;
	}
	return false;
}

bool Scanner::Comment1() {
	int level = 1, pos0 = pos, line0 = line, col0 = col;
	NextCh();
	if (ch == '*') {
		NextCh();
		for(;;) {
			if (ch == '*') {
				NextCh();
				if (ch == '/') {
					level--;
					if (level == 0) { oldEols = line - line0; NextCh(); return true; }
					NextCh();
				}
			} else if (ch == '/') {
				NextCh();
				if (ch == '*') {
					level++; NextCh();
				}
			} else if (ch == buffer->EoF) return false;
			else NextCh();
		}
	} else {
		buffer->SetPos(pos0); NextCh(); line = line0; col = col0;
	}
	return false;
}

void Scanner::CreateHeapBlock()
{
	char* cur = reinterpret_cast<char*>(firstHeap);

	// release unused blocks
	while
	(
	    (reinterpret_cast<char*>(tokens) < cur)
	 || (reinterpret_cast<char*>(tokens) > (cur + HEAP_BLOCK_SIZE))
	)
	{
		cur = *(reinterpret_cast<char**>(cur + HEAP_BLOCK_SIZE));
		free(firstHeap);
		firstHeap = cur;
	}

	// HEAP_BLOCK_SIZE byte heap + pointer to next heap block
	void* newHeap = malloc(HEAP_BLOCK_SIZE + sizeof(void*));
	*heapEnd = newHeap;
	heapEnd =
		reinterpret_cast<void**>
		(reinterpret_cast<char*>(newHeap) + HEAP_BLOCK_SIZE);
	*heapEnd = 0;
	heap = newHeap;
	heapTop = heap;
}


Token* Scanner::CreateToken()
{
	const int reqMem = sizeof(Token);
	if
	(
	    (reinterpret_cast<char*>(heapTop) + reqMem)
	 >= reinterpret_cast<char*>(heapEnd)
	)
	{
		CreateHeapBlock();
	}
	// token 'occupies' heap starting at heapTop
	Token* tok = reinterpret_cast<Token*>(heapTop);
	// increment past this part of the heap, which is now used
	heapTop =
		reinterpret_cast<void*>
		(reinterpret_cast<char*>(heapTop) + reqMem);
	tok->val  = NULL;
	tok->next = NULL;
	return tok;
}


void Scanner::AppendVal(Token* tok)
{
	const int reqMem = (tlen + 1) * sizeof(wchar_t);
	if
	(
	    (reinterpret_cast<char*>(heapTop) + reqMem)
	 >= reinterpret_cast<char*>(heapEnd)
	)
	{
		if (reqMem > HEAP_BLOCK_SIZE)
		{
			wprintf(L"--- Too long token value\n");
			::exit(1);
		}
		CreateHeapBlock();
	}

	// add text value from heap
	tok->val = reinterpret_cast<wchar_t*>(heapTop);

	// increment past this part of the heap, which is now used
	heapTop =
		reinterpret_cast<void*>
		(reinterpret_cast<char*>(heapTop) + reqMem);

	// copy the currently parsed tval into the token
	wcsncpy(tok->val, tval, tlen);
	tok->val[tlen] = '\0';
}


Token* Scanner::NextToken()
{
	while
	(
	    ch == ' '
	 || (ch >= 9 && ch <= 10) || ch == 13
	) NextCh();
	if ((ch == '/' && Comment0()) || (ch == '/' && Comment1())) return NextToken();
	t = CreateToken();
	t->pos = pos; t->col = col; t->line = line;
	int state = start.state(ch);
	tlen = 0; AddCh();

	switch (state)
	{
		case -1: { t->kind = eofSym; break; } // NextCh already done
		case 0: { t->kind = noSym; break; }   // NextCh already done
		case 1:
			case_1:
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_1;}
			else {t->kind = 1; wchar_t *literal = coco_string_create(tval, 0, tlen); t->kind = keywords.get(literal, t->kind); coco_string_delete(literal); break;}
		case 2:
			case_2:
			if ((ch >= '0' && ch <= '9')) {AddCh(); goto case_2;}
			else {t->kind = 2; break;}
		case 3:
			case_3:
			{t->kind = 3; break;}
		case 4:
			case_4:
			{t->kind = 4; break;}
		case 5:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= '&') || (ch >= '(' && ch <= '[') || (ch >= ']' && ch <= 65535)) {AddCh(); goto case_6;}
			else if (ch == 92) {AddCh(); goto case_7;}
			else {t->kind = noSym; break;}
		case 6:
			case_6:
			if (ch == 39) {AddCh(); goto case_9;}
			else {t->kind = noSym; break;}
		case 7:
			case_7:
			if ((ch >= ' ' && ch <= '~')) {AddCh(); goto case_8;}
			else {t->kind = noSym; break;}
		case 8:
			case_8:
			if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')) {AddCh(); goto case_8;}
			else if (ch == 39) {AddCh(); goto case_9;}
			else {t->kind = noSym; break;}
		case 9:
			case_9:
			{t->kind = 5; break;}
		case 10:
			case_10:
			if ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_10;}
			else {t->kind = 50; break;}
		case 11:
			case_11:
			if ((ch >= '-' && ch <= '.') || (ch >= '0' && ch <= ':') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_11;}
			else {t->kind = 51; break;}
		case 12:
			case_12:
			if (ch <= 9 || (ch >= 11 && ch <= 12) || (ch >= 14 && ch <= '!') || (ch >= '#' && ch <= '[') || (ch >= ']' && ch <= 65535)) {AddCh(); goto case_12;}
			else if (ch == 10 || ch == 13) {AddCh(); goto case_4;}
			else if (ch == '"') {AddCh(); goto case_3;}
			else if (ch == 92) {AddCh(); goto case_14;}
			else {t->kind = noSym; break;}
		case 13:
			if ((ch >= '0' && ch <= '9')) {AddCh(); goto case_10;}
			else if ((ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_15;}
			else {t->kind = 50; break;}
		case 14:
			case_14:
			if ((ch >= ' ' && ch <= '~')) {AddCh(); goto case_12;}
			else {t->kind = noSym; break;}
		case 15:
			case_15:
			if ((ch >= '0' && ch <= '9')) {AddCh(); goto case_10;}
			else if ((ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= 'a' && ch <= 'z')) {AddCh(); goto case_15;}
			else if (ch == '=') {AddCh(); goto case_11;}
			else {t->kind = 50; break;}
		case 16:
			case_16:
			if (ch == 'y') {AddCh(); goto case_17;}
			else {t->kind = noSym; break;}
		case 17:
			case_17:
			if (ch == ']') {AddCh(); goto case_18;}
			else {t->kind = noSym; break;}
		case 18:
			case_18:
			{t->kind = 6; break;}
		case 19:
			case_19:
			if (ch == 'y') {AddCh(); goto case_20;}
			else {t->kind = noSym; break;}
		case 20:
			case_20:
			if (ch == ']') {AddCh(); goto case_21;}
			else {t->kind = noSym; break;}
		case 21:
			case_21:
			{t->kind = 7; break;}
		case 22:
			case_22:
			if (ch == 'n') {AddCh(); goto case_23;}
			else {t->kind = noSym; break;}
		case 23:
			case_23:
			if (ch == 'i') {AddCh(); goto case_24;}
			else {t->kind = noSym; break;}
		case 24:
			case_24:
			if (ch == 't') {AddCh(); goto case_25;}
			else {t->kind = noSym; break;}
		case 25:
			case_25:
			if (ch == 'i') {AddCh(); goto case_26;}
			else {t->kind = noSym; break;}
		case 26:
			case_26:
			if (ch == 'a') {AddCh(); goto case_27;}
			else {t->kind = noSym; break;}
		case 27:
			case_27:
			if (ch == 'l') {AddCh(); goto case_28;}
			else {t->kind = noSym; break;}
		case 28:
			case_28:
			if (ch == 'i') {AddCh(); goto case_29;}
			else {t->kind = noSym; break;}
		case 29:
			case_29:
			if (ch == 'z') {AddCh(); goto case_30;}
			else {t->kind = noSym; break;}
		case 30:
			case_30:
			if (ch == 'e') {AddCh(); goto case_31;}
			else {t->kind = noSym; break;}
		case 31:
			case_31:
			if (ch == ']') {AddCh(); goto case_32;}
			else {t->kind = noSym; break;}
		case 32:
			case_32:
			{t->kind = 9; break;}
		case 33:
			case_33:
			if (ch == 'n') {AddCh(); goto case_34;}
			else {t->kind = noSym; break;}
		case 34:
			case_34:
			if (ch == 'i') {AddCh(); goto case_35;}
			else {t->kind = noSym; break;}
		case 35:
			case_35:
			if (ch == 't') {AddCh(); goto case_36;}
			else {t->kind = noSym; break;}
		case 36:
			case_36:
			if (ch == 'i') {AddCh(); goto case_37;}
			else {t->kind = noSym; break;}
		case 37:
			case_37:
			if (ch == 'a') {AddCh(); goto case_38;}
			else {t->kind = noSym; break;}
		case 38:
			case_38:
			if (ch == 'l') {AddCh(); goto case_39;}
			else {t->kind = noSym; break;}
		case 39:
			case_39:
			if (ch == 'i') {AddCh(); goto case_40;}
			else {t->kind = noSym; break;}
		case 40:
			case_40:
			if (ch == 'z') {AddCh(); goto case_41;}
			else {t->kind = noSym; break;}
		case 41:
			case_41:
			if (ch == 'e') {AddCh(); goto case_42;}
			else {t->kind = noSym; break;}
		case 42:
			case_42:
			if (ch == ']') {AddCh(); goto case_43;}
			else {t->kind = noSym; break;}
		case 43:
			case_43:
			{t->kind = 10; break;}
		case 44:
			case_44:
			if (ch == 'e') {AddCh(); goto case_45;}
			else {t->kind = noSym; break;}
		case 45:
			case_45:
			if (ch == 's') {AddCh(); goto case_46;}
			else {t->kind = noSym; break;}
		case 46:
			case_46:
			if (ch == 't') {AddCh(); goto case_47;}
			else {t->kind = noSym; break;}
		case 47:
			case_47:
			if (ch == 'r') {AddCh(); goto case_48;}
			else {t->kind = noSym; break;}
		case 48:
			case_48:
			if (ch == 'o') {AddCh(); goto case_49;}
			else {t->kind = noSym; break;}
		case 49:
			case_49:
			if (ch == 'y') {AddCh(); goto case_50;}
			else {t->kind = noSym; break;}
		case 50:
			case_50:
			if (ch == ']') {AddCh(); goto case_51;}
			else {t->kind = noSym; break;}
		case 51:
			case_51:
			{t->kind = 11; break;}
		case 52:
			case_52:
			if (ch == 'e') {AddCh(); goto case_53;}
			else {t->kind = noSym; break;}
		case 53:
			case_53:
			if (ch == 's') {AddCh(); goto case_54;}
			else {t->kind = noSym; break;}
		case 54:
			case_54:
			if (ch == 't') {AddCh(); goto case_55;}
			else {t->kind = noSym; break;}
		case 55:
			case_55:
			if (ch == 'r') {AddCh(); goto case_56;}
			else {t->kind = noSym; break;}
		case 56:
			case_56:
			if (ch == 'o') {AddCh(); goto case_57;}
			else {t->kind = noSym; break;}
		case 57:
			case_57:
			if (ch == 'y') {AddCh(); goto case_58;}
			else {t->kind = noSym; break;}
		case 58:
			case_58:
			if (ch == ']') {AddCh(); goto case_59;}
			else {t->kind = noSym; break;}
		case 59:
			case_59:
			{t->kind = 12; break;}
		case 60:
			case_60:
			if (ch == 'e') {AddCh(); goto case_61;}
			else {t->kind = noSym; break;}
		case 61:
			case_61:
			if (ch == ']') {AddCh(); goto case_62;}
			else {t->kind = noSym; break;}
		case 62:
			case_62:
			{t->kind = 13; break;}
		case 63:
			case_63:
			if (ch == 'e') {AddCh(); goto case_64;}
			else {t->kind = noSym; break;}
		case 64:
			case_64:
			if (ch == ']') {AddCh(); goto case_65;}
			else {t->kind = noSym; break;}
		case 65:
			case_65:
			{t->kind = 14; break;}
		case 66:
			{t->kind = 25; break;}
		case 67:
			{t->kind = 28; break;}
		case 68:
			{t->kind = 29; break;}
		case 69:
			case_69:
			{t->kind = 30; break;}
		case 70:
			{t->kind = 33; break;}
		case 71:
			case_71:
			{t->kind = 34; break;}
		case 72:
			case_72:
			{t->kind = 35; break;}
		case 73:
			{t->kind = 36; break;}
		case 74:
			{t->kind = 39; break;}
		case 75:
			{t->kind = 41; break;}
		case 76:
			{t->kind = 42; break;}
		case 77:
			{t->kind = 43; break;}
		case 78:
			case_78:
			{t->kind = 47; break;}
		case 79:
			case_79:
			{t->kind = 48; break;}
		case 80:
			if (ch == 'c') {AddCh(); goto case_84;}
			else if (ch == '/') {AddCh(); goto case_85;}
			else if (ch == 'i') {AddCh(); goto case_22;}
			else if (ch == 'd') {AddCh(); goto case_44;}
			else {t->kind = 40; break;}
		case 81:
			if (ch == '.') {AddCh(); goto case_69;}
			else if (ch == '>') {AddCh(); goto case_72;}
			else if (ch == ')') {AddCh(); goto case_79;}
			else {t->kind = 26; break;}
		case 82:
			if (ch == '.') {AddCh(); goto case_71;}
			else {t->kind = 32; break;}
		case 83:
			if (ch == '.') {AddCh(); goto case_78;}
			else {t->kind = 38; break;}
		case 84:
			case_84:
			if (ch == 'o') {AddCh(); goto case_86;}
			else {t->kind = noSym; break;}
		case 85:
			case_85:
			if (ch == 'c') {AddCh(); goto case_87;}
			else if (ch == 'i') {AddCh(); goto case_33;}
			else if (ch == 'd') {AddCh(); goto case_52;}
			else {t->kind = noSym; break;}
		case 86:
			case_86:
			if (ch == 'p') {AddCh(); goto case_16;}
			else if (ch == 'd') {AddCh(); goto case_60;}
			else {t->kind = noSym; break;}
		case 87:
			case_87:
			if (ch == 'o') {AddCh(); goto case_88;}
			else {t->kind = noSym; break;}
		case 88:
			case_88:
			if (ch == 'p') {AddCh(); goto case_19;}
			else if (ch == 'd') {AddCh(); goto case_63;}
			else {t->kind = noSym; break;}
	}
	AppendVal(t);
	return t;
}


// get the next token (possibly a token already seen during peeking)
Token* Scanner::Scan()
{
	if (tokens->next == NULL) {
		pt = tokens = NextToken();
	}
	else {
		pt = tokens = tokens->next;
	}
	return tokens;
}


// peek for the next token, ignore pragmas
Token* Scanner::Peek()
{
	do
	{
		if (pt->next == NULL)
		{
			pt->next = NextToken();
		}
		pt = pt->next;
	} while (pt->kind > maxT);   // skip pragmas

	return pt;
}


// make sure that peeking starts at the current scan position
void Scanner::ResetPeek()
{
	pt = tokens;
}


int Scanner::Line() const
{
	return line;
}


void Scanner::Line(int lineNo)
{
	line = lineNo;
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace

// ************************************************************************* //
