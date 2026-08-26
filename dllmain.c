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
	*(uint32_t*)&buffer[1] = (uint32_t)func;
	DWORD prev;
	VirtualProtect((void*)(address), sizeof(buffer), PAGE_EXECUTE_READWRITE, &prev);
	memcpy((void*)address, &buffer[0], sizeof(buffer));
	VirtualProtect((void*)(address), sizeof(buffer), prev, nullptr);
}

__declspec(dllexport) void rpmalloc_patches_initialize()
{
	WriteJump(0x61A405, &rpmalloc); // malloc
	WriteJump(0x616ED2, &rpmalloc); // operator new
	WriteJump(0x6195E0, &rpfree); // free
	WriteJump(0x60AB30, &rpfree); // operator delete
	WriteJump(0x61A491, &rpcalloc); // calloc
	WriteJump(0x6192C0, &rprealloc); // realloc
	WriteJump(0x61DF44, &rpmalloc_usable_size); // resize
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
