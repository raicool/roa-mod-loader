# roa-mod-loader
basic loader for mods made for Rivals of Aether. provides a logger and wrapper functions for MinHook (https://github.com/TsudaKageyu/minhook)

## hooking a built-in function example
```cpp
// game_save function ptr
// will be set to the address of the built-in function game_save() if it is found
GMLScriptPtr game_save;

// our detour function that logs whenever game_save is called
void game_save_detour(GMLInstance* self, GMLInstance* other, GMLVar& out, uint32_t arg_count, GMLVar* args)
{
    loader_log_info("game_save called");
    return;
}

DWORD WINAPI entry(LPVOID hModule)
{
    // get the function pointer of built-in function game_save()
    void* hook_ptr = loader_get_yyc_func_ptr("game_save");
    
    loader_hook_create(hook_ptr, &game_save_detour, game_save);
    loader_hook_enable(hook_ptr);
}
```
> NOTE: This can also be applied to custom rivals functions as well, but as of now there is no way to get the function's address through the function name alone. You will have to find the function address yourself
