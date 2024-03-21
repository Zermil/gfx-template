global u64 win32_ticks_per_sec = 1;
global HINSTANCE win32_instance =  0;

global b32 win32_os_is_init = 0;

internal b32 os_main_is_init(void)
{
    return(win32_os_is_init);
}

internal b32 os_main_init(void)
{
    b32 error = 0;
    if (win32_os_is_init) {
        er_push(str8("OS layer already initialized"));
        error = 1;
    }
    
    if (!error) {
        LARGE_INTEGER freq = {0};
        if (QueryPerformanceFrequency(&freq)) {
            win32_ticks_per_sec = (u64) freq.QuadPart;
        }
        
        win32_instance = GetModuleHandle(0);
    }
    
    if (!error) {
        win32_os_is_init = 1;
    }
    
    b32 result = !error;
    return(result);
}

internal void os_wait(f64 ms)
{
    if (!os_main_is_init()) {
        er_push(str8("OS layer was not initialized"));
    } else {
        f64 prev = os_ticks_now();
        f64 now = prev;
        while ((now - prev) < ms) {
            now = os_ticks_now();
        }
    }
}

internal f64 os_ticks_now(void)
{
    f64 result = 0.0;
    if (!os_main_is_init()) {
        er_push(str8("OS layer was not initialized"));
    } else {
        LARGE_INTEGER time = {0};
        if (QueryPerformanceCounter(&time)) {
            result = (f64) time.QuadPart;
            result *= 1000.0;
            result /= win32_ticks_per_sec;
        }
    }
    
    return(result);
}

internal HINSTANCE os_win32_get_instance(void)
{
    if (win32_instance == 0) {
        win32_instance = GetModuleHandle(0);
    }
    
    return(win32_instance);
}

internal void *os_memory_reserve(usize size)
{
    void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
    return(result);
}

internal void os_memory_release(void *ptr)
{
    VirtualFree(ptr, 0, MEM_RELEASE);
}

internal b32 os_memory_commit(void *ptr, usize size)
{
    b32 result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
    return(result);
}

internal void os_memory_decommit(void *ptr, usize size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

internal usize os_get_page_size(void)
{
    SYSTEM_INFO sysinfo = {0};
    GetSystemInfo(&sysinfo);
    return(sysinfo.dwPageSize);
}

internal void os_exit_process(u32 code)
{
    ExitProcess(code);
}