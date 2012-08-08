// MemoryEdit.h
// by iago
// Created 7/28/2003
//
// This is a class that will allow a user to easily edit a process's memory and restore it 
// when the program ends
//
// USAGE:
// First, create a new instance of ME, setting the processes to the target process.
// If you're doing this with the current process, then you can use the default 
// constructor.  
//
// Next, you make patchs to the memory of the process using this function:
//  bool PatchMem(void *AddressToEdit, void *FunctionToCall, int BytesToOverwrite, bool GenerateWrapper = true, BYTE TempRegister = ESI);
// AddressToEdit is the address to begin editing at.
// FunctionToCall is the function that the patched call will be.  AddressToEdit will be overwritten
//  with either a jmp or a call to FunctionToCall.
// BytesToOverwrite is the number of bytes that will be overwritten.  
//  **IMPORTANT**
//  The number of bytes overwritten MUST NOT divide any instructions in the middle.  Besure to 
//  patch over boundries between instructions
// GenerateWrapper is a bool that represents whether or not to copy the code being overwritten to
//  a temporary buffer.  The wrapper will be executed before the FunctionToCall is called
// TempRegister is the register we use to store the return address in if we're using a wrapper.
//  ESI should always work, since functions, by default, preserve it.  But I allow it to be
//  specified just in case.
//
// Notes:
// DO NOT patch over a relative call or jmp, unless that patch includes the destination.
// If you don't specify TempRegister, make sure that ESI isn't holding anything important.
// If you do specify a TempRegister, make sure that whatever you specify isn't holding anything important.

#ifndef MEMORYEDIT_H
#define MEMORYEDIT_H

#include <windows.h>
#include <assert.h>
#include <windows.h>
#include <iostream>
#include <string>
#include "Buffer.h"

using namespace std;

// This struct holds the data that can restore exactly 1 patch
struct Restore
{
	void *OriginalData;  // Pointer to the original data (PatchLength bytes)
	void *PatchLocation; // Pointer to where the patch was applied
	DWORD PatchLength; // The number of bytes that were patched
};

class MemoryEdit
{
public:
	BYTE Wrappers[10000]; // The buffer that will hold the wrappers
	Restore RestoreData[2000]; // The data used to restore the patch
	DWORD NumberOfPatches; // The number of places that have been patched
	void *NextWrapper; // The next free wrapper
	HANDLE Process; // Handle to the process (assigned using =, so it won't know if it's closed)
public:
	static const BYTE POP =  0x58;
	static const BYTE PUSH = 0x50;

	static const BYTE EAX = 0x00;
	static const BYTE EBX = 0x03;
	static const BYTE ECX = 0x01;
	static const BYTE EDX = 0x02;
	static const BYTE ESI = 0x06;
	static const BYTE EDI = 0x07;
	static const BYTE ESP = 0x04;
	static const BYTE EBP = 0x05;

	static const BYTE NOP =  0x90;
	static const BYTE RET =  0xC3;
	static const BYTE CALL = 0xE8;
	static const BYTE LONGJMP =  0xE9;
	static const BYTE SHORTJMP =  0xEB;

	// 5 bytes for the call, 1+1+1 for the pop, push, and ret
	static const EXTRA_DATA = 8;

	MemoryEdit();
	MemoryEdit(HANDLE Process);
	~MemoryEdit();
	
	// BytesToOverwrite must be at least 5 and has to be an exact number of machine code commands,
	// overwriting part of a command will cause problems.  Also, don't overwrite a jump or call that
	// goes outside of the overwritten text (or any offset-operator that goes outside of the patch)
	// because it will cause a problem :)
	// Also, patching over a push will cause problems.. you'll get over it :P
	bool PatchMem(void *AddressToEdit, void *FunctionToCall, DWORD BytesToOverwrite, bool GenerateWrapper = true, BYTE TempRegister = ESI);

};

#endif