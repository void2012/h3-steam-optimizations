#include <windows.h>
#include <stdint.h>

extern __declspec(dllexport) BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved);
static void WriteJump(uintptr_t address, void* func)
{
	// 0x68 push
	// 0xAABBCCDD function address
	// 0xC3 ret
	uint8_t buffer[6] = { 0x68, 0, 0, 0, 0, 0xC3 };
	*(uintptr_t*)&buffer[1] = (uintptr_t)func;
	DWORD prev;
	VirtualProtect((void*)(address), sizeof(buffer), PAGE_EXECUTE_READWRITE, &prev);
	memcpy((void*)(address), &buffer[0], sizeof(buffer));
	VirtualProtect((void*)(address), sizeof(buffer), prev, nullptr);
}

extern void __fastcall Process1WindowsMessage();
extern void __fastcall GameTime__DelayTil(DWORD);
extern void __fastcall GameTime__Delay(DWORD);

__declspec(dllexport) void rpmalloc_patches_initialize()
{
	// CPU utilization
	WriteJump(0x4F7FB0, &Process1WindowsMessage); // Process1WindowsMessage
	WriteJump(0x4F82F0, &GameTime__DelayTil); // GameTime::DelayTil
	WriteJump(0x4F83C0, &GameTime__Delay); // GameTime::Delay
	// Optimized heap
	WriteJump(0x61A405, &rpmalloc); // malloc
	WriteJump(0x616ED2, &rpmalloc); // operator new
	WriteJump(0x6195E0, &rpfree); // free
	WriteJump(0x60AB30, &rpfree); // operator delete
	WriteJump(0x61A491, &rpcalloc); // calloc
	WriteJump(0x6192C0, &rprealloc); // realloc
	WriteJump(0x61DF44, &rpmalloc_usable_size); // _msize
}

extern __declspec(dllexport) BOOL WINAPI
DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
	(void)sizeof(reserved);
	(void)sizeof(instance);
	if (reason == DLL_PROCESS_ATTACH)
	{
		rpmalloc_patches_initialize();
		rpmalloc_initialize();
	}
	else if (reason == DLL_PROCESS_DETACH)
		rpmalloc_finalize();
	else if (reason == DLL_THREAD_ATTACH)
		rpmalloc_thread_initialize();
	else if (reason == DLL_THREAD_DETACH)
		rpmalloc_thread_finalize();
	return TRUE;
}
