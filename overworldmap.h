#ifndef _OVERWORLD_MAP_
#define _OVERWORLD_MAP_

#include <genesis.h>
#include "resources.h"


extern u32 offset_x;
extern u32 offset_y;

enum{ // enum for collision array
    WALKABLE_TILE = 0,
    SOLID_TILE = 1,
    TIRE_TILE = 2,
    VERTICAL_JUMP_TILE = 3,
    HORIZONTAL_JUMP_TILE = 4,
    CHECKPOINT_1_TILE = 5,
    CHECKPOINT_2_TILE = 6,
    CHECKPOINT_3_TILE = 7,
    ELEVATED_TILE = 8
};

typedef struct{

    u16 length;
    u16 height;

    u16 startPosX;
    u16 startPosY;

    MapDefinition *mapDef;
    u8 collision[];

}overworld_map;

extern overworld_map* currentMap; // pointer which will point to whichever map is currently being used

overworld_map* OVW_MAP_create(u32 l, u32 h, u16 spX, u16 spY, const u8 col[], const MapDefinition* mpdef); // constructor for struct

u8 OVW_MAP_tileAt(overworld_map *ovw_map, u32 x, u32 y); // returns the type of collision tile at the pixel coordinates given
#endif // _OVERWORLD_MAP_
