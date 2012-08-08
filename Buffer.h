// Buffer.h
// by iago
// Created: 04/11/2003
//
// This is a fairly simple buffer class that can have various types
// of variables inserted to it (at the back/right) and extracted from
// it (at the back). 

// Make sure it's not included more than once
#ifndef BUFFER_H
#define BUFFER_H

// Some important typedefs, just in case nobody's done them yet:
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;

// Includes
#include <iostream>
using namespace std;

class Buffer
{
protected:
	BYTE *_buf;
	// _buf + _length will ALWAYS be the next free byte.
	DWORD _length;

	// This adds bytes to the end of the buffer
	void _AddToBuffer(const BYTE *add, const DWORD size);

	// This ensures the buffer is at least "size" bytes long, and extends it if it's not
	void _ensureBufferSize(const DWORD size);

	// This is called everytime the buffer is changed (does nothing, but
	// can be extended)
	virtual void onChange();

private:
	// This is the current length of the buffer.  It changes as the buffer grows, though.
	DWORD _maxLength;

	// If you're going to be using large structs (>128 bytes), I would suggest increasing
	// this so it's at least the size of one struct.
	static const WORD _chunksize = 256;

	// Initializes the variables to the default values
	void initializeVariables();

	// Copies src into "this"
	void _copy(const Buffer &src);
public:
	// Default constructor, sets empty buffer with default size
	Buffer();

	// Copy constructor, will copy the data from the original buffer
	// into the new buffer
	Buffer(const Buffer &c);

	// These constructors will initialize the buffer with 
	Buffer(const DWORD c);
	Buffer(const WORD c);
	Buffer(const BYTE c);
	Buffer(const string &c);
	Buffer(const char *c);
	Buffer(const BYTE *c, const DWORD len);
	~Buffer();

	// This will return a string object which might look like this:
	// FF 03 05 11 12 13 14 15 15 16 12 45 BC 46 48 FF ...........D...
	// 69 FF FF FF                                     i...
	string toString(int LineLen = 16);

	// This will return a pointer to a constant char*
	const BYTE *c_str();

	// This will erase the buffer
	void ClearBuffer();

	// This will get the current number of bytes in the data
	DWORD GetSize();

	// These are overloaded operators for buffers

    // Copies "d" directly into "this".
	void operator=(Buffer d);

	// This returns TRUE if the buffers are equal (ie, same data)
	bool operator==(Buffer d);
	// Returns the BYTE at the specified offset
	BYTE operator[](DWORD offset);


	// This will insert stuff to the end of the buffer
	// Note - when inserting the strings, it doesn't add a null-terminator.  (BYTE)0 should
	// be inserted if the string must be NT.  This goes for string objects and char*'s.
	Buffer &operator<<(const DWORD c);
	Buffer &operator<<(const WORD c);
	Buffer &operator<<(const BYTE c);
	Buffer &operator<<(const string &c);
	Buffer &operator<<(const char *c);
	Buffer &operator<<(const Buffer &c);

	// These are for simple removal from the front of the dealy
	Buffer &operator>>(DWORD &v);
	Buffer &operator>>(WORD &v);
	Buffer &operator>>(BYTE &v);
	// Removes an nt-string (overwrites whatever was in there)
	Buffer &operator>>(string &v);
	// To remove a buffer, you have to use RemoveFromFront and specify
	// a buffer

	// These functions will remove size bytes from the front (or back)
	// of the Buffer and return a pointer the the first.  The 
	// programmer is expected to typecast them to what he wants it
	// to be, or just use the >> operator :-)

	// This removes bytes from the beginning of the buffer (queue-ishly)
	// Returns false 
	bool Buffer::RemoveFromFront(BYTE *buffer, DWORD size);

	// This removes bytes from the end of the buffer (stack-ishly)
	// This is much more efficient, so I would suggest using it.  But
	// nobody listens to me anyway.
	bool RemoveFromBack(BYTE *buffer, DWORD size);
};




#endif // defined BUFFER_H