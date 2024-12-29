#include "overworldmap.h"

u32 offset_x = 0;
u32 offset_y = 0;

overworld_map* currentMap;

overworld_map* OVW_MAP_create(u32 l, u32 h, u16 spX, u16 spY, const u8 col[], const MapDefinition* mpdef)  {

    overworld_map *ovw_map = malloc(sizeof(*ovw_map) + sizeof(u8)*(l*h));

    kprintf("%lu", sizeof(*ovw_map)); // DEBUGGing

    ovw_map->length = l;
    ovw_map->height = h;

    ovw_map->startPosX = spX;
    ovw_map->startPosY = spY;

    ovw_map->mapDef = mpdef;

    for (int i=0; i<l*h; i++)   { // add each member of the collision array one at a time
         ovw_map->collision[i] = col[i];
    }

    return ovw_map;
}

u8 OVW_MAP_tileAt(overworld_map* ovw_map, u32 x, u32 y)  {

    x /= 8; // convert from pixel coordinate to tile coordinate
    y /= 8;

    return ovw_map->collision[x + y*(ovw_map->length)]; // return value of tile in collision array

}
