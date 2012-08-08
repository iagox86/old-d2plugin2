#include "MemoryEdit.h"


MemoryEdit::MemoryEdit()
{
	this->NextWrapper = this->Wrappers;
	this->Process = GetCurrentProcess();
	this->NumberOfPatches = 0;
	memset((void*)this->Wrappers, 0, sizeof(this->Wrappers));
}

MemoryEdit::MemoryEdit(HANDLE Process)
{
	this->NextWrapper = this->Wrappers;
	this->Process = ((Process == 0) ? GetCurrentProcess() : Process);
	this->NumberOfPatches = 0;
	memset((void*)this->Wrappers, 0, sizeof(this->Wrappers));
}

MemoryEdit::~MemoryEdit()
{
	// Get the first original data
	BYTE *OriginalData = this->Wrappers;

	// Loop through each patch
	for(DWORD i = 0; i < this->NumberOfPatches; i++)
	{
		WriteProcessMemory(Process, 
			RestoreData[i].PatchLocation, 
			RestoreData[i].OriginalData, 
			RestoreData[i].PatchLength, 
			NULL);
	}
}

bool MemoryEdit::PatchMem(void *AddressToEdit, void *FunctionToCall, DWORD BytesToOverwrite, bool GenerateWrapper, BYTE TempRegister)
{
	// This will patch the AddressToEdit with a call to somewhere in Wrappers, which will
	// do the stuff from the memory that was overwritten, then jump to our function which
	// should only have to take care of backing up registers

	Buffer Patch;
	Buffer Wrapper;
	SIZE_T BytesRead; 

	assert(BytesToOverwrite >= 5);

	// Allocate space for the bytes that we'll be overwriting
	char *BytesOverwritten = (char*)malloc(BytesToOverwrite);
	if(!ReadProcessMemory(Process, AddressToEdit, BytesOverwritten, BytesToOverwrite, &BytesRead))
		return false;

	// Add the temp register pop to the Wrapper
	Wrapper << (BYTE)(POP | TempRegister);

	// Add the wrapper bytes to the patch buffer
	// This is used both for wrapping and to restore from
	for(DWORD i = 0; i < BytesToOverwrite; i++)
	{
		Wrapper << (BYTE) BytesOverwritten[i];
	}

	// If we're making a wrapper, set it up
	if(GenerateWrapper)
	{
		// Thedistance of this call is to (FunctionToCall) from (NextWrapper + 1 + 5 - [the sizeof the wrapper data]) 
		Wrapper << CALL << (DWORD)(FunctionToCall) - ((DWORD)NextWrapper + 1 + BytesToOverwrite + 5);
		// If we're doing a call here, we also have to add a ret
		Wrapper << (BYTE)(PUSH | TempRegister);
		Wrapper << RET;

		// If we're using a wrapper, the patched call needs to go from the original
		// address to the NextWrapper
		Patch << CALL << ((DWORD)(NextWrapper) - (DWORD)(AddressToEdit) - 5);
	}
	else
	{
		// If we aren't using a wrapper, the patched call needs to go from the original
		// address to the requested function
		Patch << CALL << ((DWORD)(FunctionToCall) - (DWORD)(AddressToEdit) - 5);
	}

	// Pad out the patch with NOP's
	for(DWORD i = 0; i < (BytesToOverwrite - 5); i++)
	{
		Patch << NOP;
	}

	// Write the patch to the target process
	// Then write the patch
	if(WriteProcessMemory(Process, AddressToEdit, Patch.c_str(), Patch.GetSize(), NULL) == 0)
		return false;

	// Write the wrapper 
	if(WriteProcessMemory(GetCurrentProcess(), NextWrapper, Wrapper.c_str(), Wrapper.GetSize(), NULL) == 0)
		return false;

	RestoreData[NumberOfPatches].OriginalData = (void*)(((DWORD)NextWrapper) + 1);
	RestoreData[NumberOfPatches].PatchLocation = (void*) AddressToEdit;
	RestoreData[NumberOfPatches].PatchLength = BytesToOverwrite;

	// Move up to the next available wrapper
	NextWrapper = (void*)((DWORD) NextWrapper + Wrapper.GetSize());


	// Increment the number of patches
	NumberOfPatches++;

	// Free up the overwritten bytes
	free(BytesOverwritten);


	return true;
}


