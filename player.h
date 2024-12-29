#ifndef _PLAYER_
#define _PLAYER_

#include <genesis.h>
#include "overworldmap.h"
#include "resources.h"

enum {
    ANIM_PLR_VERTICAL = 0,
    ANIM_PLR_HORIZONTAL = 1,
    ANIM_PLR_DIAGONAL = 2
}; // NOTE : elevated indeces are 3 higher than the regular indeces. Jumping indeces are 6 higher than the regular indeces.


typedef struct{

    Sprite *spr;

    Sprite *HUD_head;
    Sprite *HUD_name;
    Sprite *HUD_lapCounter;

    s8 vel_x;
    s8 vel_y;

    u8 pos_x; //  universal position
    u8 pos_y;

    Map *mp;
    s32 offset_x;
    s32 offset_y;

    bool HFlip; // for keeping track of HFlip and VFlip values of sprite
    bool VFlip;

    u16 stunTimer; // says how long to stun the player for. If 0, player is not stunned.

    u8 laps;// the amount of laps the player has done.
    u8 lastCheckpoint; // tracks the last checkpoint the player hit (possible values 1, 2, 3. checkpoint 1 is the starting/finish line)

    bool elevated; // tracks if the player is on ground level or higher up (from a ramp)

    u8 jumpTimer; // tracks if the player is currently jumping

}player;

s8 sign(s8 val); // basic function to get the sign of a given term
u8 absoluteValue(s8 val); // gets absolute value of a given term

void PLR_init(player *p, u8 posx, u8 posy, const SpriteDefinition* sprDef, const MapDefinition* mpDef, VDPPlane plane, u8 pal, u8 *ind);
void PLR_release(player *p);

void PLR_setVRAMTileIndex(player *p, u8 ind);

bool PLR_canMoveX(player *p, overworld_map *mp);
bool PLR_canMoveY(player *p, overworld_map *mp);

bool PLR_isTouching(player *p, overworld_map *mp, u8 TILE);

void PLR_moveOnMap(player *p, overworld_map *mp);

bool PLR_playerScreensOverlap(player *p1, player *p2); // function to check if players are close enough to where they will appear on each others screens.

void PLR_setMapPos(player *p, s32 relative_pos_x, s32 relative_pos_y); // position setter only to be used to initialize a player's position on a map

void PLR_setHFlip(player *p, bool val); // these to be used instead of SPR_setHFlip and VFlip
void PLR_setVFlip(player *p, bool val);

void PLR_incrementTimers(player *p);

void PLR_setAnim(player *p, u8 animInd);

#endif // _PLAYER_
