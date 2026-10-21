# new_dust_fx_obj

> updated as of 2.1.9.1

this is the vfx instance created during oPlayer dash, run, jump, and landlag states

| Name          | Type    | Description                                              |
| ------------- | ------- | -------------------------------------------------------- |
| `fg_index`    | `real`  |                                                          |
| `__sync_id`   | `real`  | unknown, presumably for rollback netcode synchronization |
| `player`      | `real`  | index of player object was spawned from                  |
| `dust_depth`  | `real`  |                                                          |
| `dust_fx`     | `int64` |                                                          |
| `draw_angle`  | `real`  |                                                          |
| `step_timer`  | `real`  |                                                          |
| `player_id`   | `real`  | unused?                                                  |
| `shader_init` | `real`  | unknown? is shader initialized?                          |
| `dust_length` | `real`  |                                                          |
| `init`        | `real`  |                                                          |
| `hsp`         | `real`  | horizontal speed                                         |
| `spr_dir`     | `real`  | sprite direction (-1: mirrored, 1: normal)               |
| `dust_color`  | `real`  |                                                          |
| `vsp`         | `real`  | vertical speed                                           |
