# pHurtbox

> updated as of 2.1.9.1

this instance is usually owned by an oPlayer instance

> [!WARNING]
> multiple variables in this instance can change into int64 types when created from rollback netcode states, mostly due to the netcode being written in haxe

| Name              | Type   | Description                                              |
|-------------------|--------|----------------------------------------------------------|
| `have_bubble`     | `real` | set to true if owner (oPlayer) is in a "bubbled" state   |
| `hit_prior`       | `real` | unknown, usually set to 0                                |
| `__sync_id`       | `real` | unknown, presumably for rollback netcode synchronization |
| `have_collision`  | `real` |                                                          |
| `have_plant`      | `real` |                                                          |
| `playerID`        | `real` | instance id of owner (oPlayer)                           |
| `player`          | `real` | player index of owner (oPlayer)                          |
| `ice_id`          | `real` |                                                          |
| `dodging`         | `real` | set to 1 when player is rolling or tech rolling          |
| `have_burn`       | `real` | if owner (oPlayer) is currently burned                   |
| `have_ice`        | `real` | if owner (oPlayer) is currently frozen                   |
| `have_tree_plant` | `real` | if owner (oPlayer) is currently wrapped by lily          |
| `plant_id`        | `real` |                                                          |