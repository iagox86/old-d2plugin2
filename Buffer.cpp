// Buffer.h
// by iago
// Created: 04/11/2003
//
// This is a fairly simple buffer class that can have various types
// of variables inserted to it (at the back/right) and extracted from
// it (at the back). 

#include "Buffer.h"
#include <strstream>
#include <iostream>
using namespace std;

// This will make sure there are size bytes of room or more in the buffer.
void Buffer::_ensureBufferSize(const DWORD size)
{
	// If it's already the right size, just return
	if(this->_maxLength >= size)
		return;

	// First, get the new size we need
	while(this->_maxLength < size)
	{
		this->_maxLength += this->_chunksize;
	}

	// Then create a new buffer to hold the data
	BYTE *newBuffer = (BYTE*)malloc(sizeof(BYTE) * this->_maxLength);
	// Copy the old data to the new struct
	memcpy(newBuffer, this->_buf, sizeof(BYTE) * this->_length); 
	// Destroy the old data
	free(this->_buf);

	// Point buf to the new buffer
	this->_buf = newBuffer;	
}

// This adds bytes to the end of the buffer
void Buffer::_AddToBuffer(const BYTE *add, const DWORD size)
{
	// First make sure there's enough room
	this->_ensureBufferSize(this->_length + size);

	// Now, either way, append the new data.  We're guarenteed to have enough room.
	memcpy(this->_buf + this->_length, add, size);

	// Increment the length
	this->_length += size;	

	// Call onChange()
	onChange();
}

void Buffer::_copy(const Buffer &src)
{
	this->_maxLength = src._maxLength;
	this->_length = src._length;

	if(this->_buf)
	{
		free(this->_buf);
	}

	this->_buf = (BYTE*)malloc(src._maxLength);

	for(DWORD i = 0; i < src._length; i++)
	{
		this->_buf[i] = src._buf[i];
	}

	// call onChange()
	onChange();
}


// This is called everytime the buffer is changed (does nothing, but
// can be extended)
void Buffer::onChange()
{
	// Do nothing
}

// Initialize the variables to default values
void Buffer::initializeVariables()
{
	// Set the max length to chunksize
	this->_maxLength = this->_chunksize;
	// Set the current length to nothing
	this->_length = 0;
	// Allocate enough memory for chunksize
	this->_buf = (BYTE*)malloc(sizeof(BYTE) * this->_chunksize);
}

// Default constructor, sets empty buffer with default size
Buffer::Buffer()
{
	this->initializeVariables();
}

// Copy constructor, will copy the data from the original buffer
// into the new buffer
Buffer::Buffer(const Buffer &c)
{
	this->initializeVariables();
	_copy(c);
}

// These constructors will initialize the buffer
Buffer::Buffer(const DWORD c)
{
	this->initializeVariables();
	this->_AddToBuffer((BYTE*)&c, sizeof(DWORD));
	onChange();
}
Buffer::Buffer(const WORD c)
{
	this->initializeVariables();
	this->_AddToBuffer((BYTE*)&c, sizeof(WORD));
	onChange();
}
Buffer::Buffer(const BYTE c)
{
	this->initializeVariables();
	this->_AddToBuffer((BYTE*)&c, sizeof(BYTE));
	onChange();
}
Buffer::Buffer(const string &c)
{
	this->initializeVariables();
	this->_AddToBuffer((BYTE*)(c.c_str()), (DWORD)c.length());
	onChange();
}
Buffer::Buffer(const char *c)
{
	this->initializeVariables();
	if(c)
		this->_AddToBuffer((BYTE*)c, (DWORD)strlen(c));
	onChange();
}
Buffer::Buffer(const BYTE *c, const DWORD len)
{
	this->initializeVariables();
	this->_AddToBuffer((BYTE*)c, len);
	onChange();
}

Buffer::~Buffer()
{
	if(_buf)
		free(_buf);
}

// This will return a string object which might look like this:
// FF 03 05 11 12 13 14 15 15 16 12 45 BC 46 48 FF	...........D...
// 69 FF FF FF										i...
string Buffer::toString(int LineLen)
{
	string returnString;
	char TempString[4];
	DWORD i, j; // Loop variables
	for(i = 0; i < this->_length; i++)
	{
		
		if(i && (i % LineLen == 0))
		{
			// If it's a multiple of 16 and i isn't null, show the ascii
			returnString += '\t';
			for(j = i - LineLen; j < i; j++)
			{
				if(this->_buf[j] < 0x20 || this->_buf[j] > 0x7F)
					returnString += '.';
				else
					returnString += this->_buf[j];
			}
			// Add a linefeed after the string
			returnString += '\n';
		}

		// Put the next 2 hex values into a string and add them to the returnString
		sprintf(TempString, "%02X ", 0xFF & this->_buf[i]);
		returnString += TempString;
	}
	
	// Add padding spaces if it's not a multiple of 16
	if(i % LineLen != 0)
	{
		for(j = 0; j < ((LineLen - (i % LineLen)) * 3); j++)
		{
			returnString += ' ';
		}
	}
	// Add the tab for alignment
	returnString += '\t';

	// Add final chararacters at right, after padding

	// If it was at the end of a line, print out the full line
	if((i % LineLen) == 0)
	{
		j = i - LineLen;
	}	
	else
	{
		j = (i - (i % LineLen));
	}

	for(; j < i; j++)
	{
		if(this->_buf[j] < 0x20 || this->_buf[j] > 0x7F)
			returnString += '.';
		else
			returnString += this->_buf[j];
	}

	// Finally, tidy it all up with a newline
	returnString += '\n';

	return returnString;
}

const BYTE *Buffer::c_str()
{
	return _buf;
}

// This will erase the buffer
void Buffer::ClearBuffer()
{
	free(this->_buf);
	this->initializeVariables();
	onChange();
}

// This will get the current number of bytes in the data
DWORD Buffer::GetSize()
{
	return this->_length;
}

// These are overloaded operators for buffers

// Copies "d" directly into "this".
void Buffer::operator=(Buffer d)
{
	_copy(d);
}

// This returns TRUE if each byte is equal
bool Buffer::operator==(Buffer d)
{
	// First, check the size
	if(this->_length != d._length)
		return false;

	// If they are the same size, step through each byte and check it
	if(memcmp(this->_buf, d._buf, this->_length) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// Returns the BYTE at the specified offset
BYTE Buffer::operator[](DWORD offset)
{
	return (BYTE)this->_buf[offset];
}

// This will insert stuff to the end of the buffer
Buffer &Buffer::operator<<(const DWORD c)
{
	this->_AddToBuffer((BYTE*)&c, sizeof(DWORD));
	return *this;
}
Buffer &Buffer::operator<<(const WORD c)
{
	this->_AddToBuffer((BYTE*)&c, sizeof(WORD));
	return *this;
}
Buffer &Buffer::operator<<(const BYTE c)
{
	this->_AddToBuffer((BYTE*)&c, sizeof(BYTE));
	return *this;
}
Buffer &Buffer::operator<<(const string &c)
{
	this->_AddToBuffer((BYTE*)c.c_str(), (DWORD)c.length());
	return *this;
}
Buffer &Buffer::operator<<(const char *c)
{
	DWORD Length = (DWORD)strlen(c);
	this->_AddToBuffer((BYTE*)c, Length);
	return *this;
}
Buffer &Buffer::operator<<(const Buffer &c)
{
	this->_AddToBuffer((BYTE*)c._buf, c._length);
	return *this;
}

Buffer &Buffer::operator>>(DWORD &v)
{
	this->RemoveFromFront((BYTE*)&v, sizeof(DWORD));
	return *this;
}
Buffer &Buffer::operator>>(WORD &v)
{
	this->RemoveFromFront((BYTE*)&v, sizeof(WORD));
	return *this;
}
Buffer &Buffer::operator>>(BYTE &v)
{
	this->RemoveFromFront((BYTE*)&v, sizeof(BYTE));
	return *this;
}
// Removes an nt-string (overwrites whatever was in there) including the null
// In the future, it would be nice to write a strnlen() function to use since
// strlen will crash if it goes over the end of the buffer.
Buffer &Buffer::operator>>(string &v)
{
	// Allocate enough space to return it in
	char *tempReturn = (char*)malloc(strlen((char*)this->_buf) * sizeof(char) + 1);

	// Fill the string
	this->RemoveFromFront((BYTE*)tempReturn, (DWORD)strlen((char*)this->_buf) + 1);
	
	// Put it into the string
	v = tempReturn;

	// Delete the temp string to avoid memory leaks
	free(tempReturn);

	return *this;
}
// To remove a buffer, you have to use RemoveFromFront and specify
// a buffer

// These functions will remove size bytes from the front (or back)
// of the Buffer and return a pointer the the first.  The 
// programmer is expected to typecast them to what he wants it
// to be, or just use the >> operator :-)

// This removes bytes from the beginning of the buffer (queue-ishly)
bool Buffer::RemoveFromFront(BYTE *buffer, DWORD size)
{
	// Make sure there's that much data available
	if(this->_length < size)
	{
		return false;
	}

	// Copy the memory into the return buffer
	memcpy((void*)buffer, this->_buf, size);

	// Decrement the size variables
	this->_length -= size;

	// Move everything else left (yes, this is a little costly, that's why it's
	// best to remove from the back).  An alternative might be to move up the pointer
	// to the beginning, but that would probably be even worse since it would require
	// a malloc() sooner and have to copy even more memory.
	memcpy((void*)this->_buf, (void*)(this->_buf + size), this->_length);

	onChange();
	return true;
}

// This removes bytes from the end of the buffer (stack-ishly)
// This is much more efficient, so I would suggest using it.  But
// nobody listens to me anyway.
bool Buffer::RemoveFromBack(BYTE *buffer, DWORD size)
{
	// Make sure there's enough to copy
	if(this->_length < size)
	{
		buffer = (BYTE*)-1;
		return false;
	}

	// Ok we know there's enough to copy, so move it into the return buffer
	memcpy(buffer, this->_buf + (this->_length - size), size);

	// Decrement the length (could zero out the memory, but that doesn't matter)
	this->_length -= size;

	onChange();
	return true;
}
