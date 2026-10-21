# roa-mod-loader
basic loader for mods made for Rivals of Aether. provides a logger and wrapper functions for MinHook (https://github.com/TsudaKageyu/minhook)

> Works through versions 1.4.0 - 2.1.9.1, for older version you will have to manually start the loader using Cheat Engine, as they don't load `catch_error_mini.dll`

> [!NOTE]
> interested in creating your own roa mod? a mod template is available [here](https://github.com/raicool/roa-mod-template)

## hooking a built-in function example
```cpp
// audio_play_sound function pointer
// will be set to the address of the built-in function audio_play_sound() if it is found
PFUNC_YYGML audio_play_sound;

// our detour function that logs whenever audio_play_sound is called
void audio_play_sound_detour(CInstance* self, CInstance* other, RValue& out, uint32_t argc, RValue* argv)
{
    loader_log_info("audio_play_sound called");
    audio_play_sound(self, other, out, argc, argv);
}

DWORD WINAPI entry(LPVOID hModule)
{
    // get the function pointer of built-in function game_save()
    void* loc = loader_get_yyc_func_ptr("audio_play_sound");
    
    loader_hook_create(loc, &audio_play_sound_detour, audio_play_sound);
    loader_hook_enable(loc);
}
```
