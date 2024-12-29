#include "player.h"

s8 sign(s8 val) {

    if (val > 0)    {
        return 1;
    }
    if (val < 0)    {
        return -1;
    }
    return 0;

}

u8 absoluteValue(s8 val)    {

    if (val == 0)   {
        return 0;
    }   else    {
        return (val / sign(val));
    }

}


void PLR_init(player *p, u8 posx, u8 posy, const SpriteDefinition* sprDef, const MapDefinition* mpDef, VDPPlane plane, u8 pal, u8 *ind){

    p->pos_x = posx;
    p->pos_y = posy;

    p->spr = SPR_addSprite(sprDef, posx, posy, TILE_ATTR(pal, TRUE, FALSE, FALSE));
    SPR_setAnim(p->spr, ANIM_PLR_VERTICAL);

    if (posy > 112) {   //if on lower half of screen, show Oakley HUD

        p->HUD_head = SPR_addSprite(&spr_oakley_head_neutral, 0, 112, TILE_ATTR(PAL3, 0, 0, 0));
        p->HUD_name = SPR_addSprite(&spr_oakley_name, 0, 136, TILE_ATTR(PAL3, 0, 0, 0));
        p->HUD_lapCounter = SPR_addSprite(&spr_lap_counter, 0, 144, TILE_ATTR(PAL3, 0, 0, 0));

    }   else    { // otherwise show Tucker hud

        p->HUD_head = SPR_addSprite(&spr_tucker_head_neutral, 0, 0, TILE_ATTR(PAL3, 0, 0, 0));
        p->HUD_name = SPR_addSprite(&spr_tucker_name, 0, 24, TILE_ATTR(PAL3, 0, 0, 0));
        p->HUD_lapCounter = SPR_addSprite(&spr_lap_counter, 0, 32, TILE_ATTR(PAL3, 0, 0, 0));

    }


    p->mp = MAP_create(mpDef, plane, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, *ind));

    p->vel_x = 0;
    p->vel_y = 0;
    p->offset_x = 0;
    p->offset_y = 0;

    p->HFlip = false;
    p->VFlip = false;

    p->stunTimer = 0; // not stunned to start

    p->laps = 0;
    p->lastCheckpoint = 1; // start at checkpoint 1(the starting line)

    p->elevated = false; // player starts on ground level
    p->jumpTimer = 0; // player is not jumping


}

void PLR_release(player *p) {

    SPR_releaseSprite(p->spr);

    SPR_releaseSprite(p->HUD_head);
    SPR_releaseSprite(p->HUD_name);
    SPR_releaseSprite(p->HUD_lapCounter);

    MAP_release(p->mp);

}

void PLR_setVRAMTileIndex(player *p, u8 ind)    {

    SPR_setVRAMTileIndex(p->spr, TILE_SPRITE_INDEX + ind);
    ind += 9;
    SPR_setVRAMTileIndex(p->HUD_head, TILE_SPRITE_INDEX + ind);
    ind += 9;
    SPR_setVRAMTileIndex(p->HUD_name, TILE_SPRITE_INDEX + ind);
    ind += 3;
    SPR_setVRAMTileIndex(p->HUD_lapCounter, TILE_SPRITE_INDEX + ind);

}

bool PLR_canMoveX(player *p, overworld_map *mp) { // checks if player can move one pixel in the x axis according to their velocity

    s8 vel_x_one_pixel = sign(p->vel_x);

    if (p->pos_y > 112) { // if on lower half of screen, map will be offset vertically


        for (int i=8; i<=16; i+= 8) {
            for (int j=8; j<=16; j+=8)  {
                if (OVW_MAP_tileAt(mp, p->offset_x + p->pos_x + vel_x_one_pixel + j, p->offset_y + p->pos_y - 112 + i) == SOLID_TILE)    {
                    return false;
                }
            }
        }

        return true;

    }   else    {

        for (int i=8; i<=16; i+= 8) {
            for (int j=8; j<=16; j+=8)  {
                if (OVW_MAP_tileAt(mp, p->offset_x + p->pos_x + vel_x_one_pixel + j, p->offset_y + p->pos_y + i) == SOLID_TILE)    {
                    return false;
                }
            }
        }
        return true;

    }



}

bool PLR_canMoveY(player *p, overworld_map *mp) {

    s8 vel_y_one_pixel = sign(p->vel_y);

    if (p->pos_y > 112) { // if on lower half of screen, map will be offset vertically

        for (int i=8; i<=16; i+= 8) {
            for (int j=8; j<=16; j+=8)  {
                if (OVW_MAP_tileAt(mp, p->offset_x + p->pos_x + j, p->offset_y + p->pos_y + vel_y_one_pixel - 112 + i) == SOLID_TILE)    {
                    return false;
                }
            }
        }

        return true;

    }   else    {

        for (int i=8; i<=16; i+= 8) {
            for (int j=8; j<=16; j+=8)  {
                if (OVW_MAP_tileAt(mp, p->offset_x + p->pos_x + j, p->offset_y + p->pos_y + vel_y_one_pixel + i) == SOLID_TILE)    {
                    return false;
                }
            }
        }

        return true;

    }



}

bool PLR_isTouching(player *p, overworld_map *mp, u8 TILE)   { // checks if a player is touching a specific type of tile

    if (p->pos_y > 112) { // if on lower half of screen, map will be offset vertically

        for (int i=8; i<=16; i+= 8) {
            for (int j=8; j<=16; j+=8)  {
                if (OVW_MAP_tileAt(mp, p->offset_x + p->pos_x + j, p->offset_y + p->pos_y - 112 + i) == TILE)    {
                    return true;
                }
            }
        }

        return false;

    }   else    {

        for (int i=8; i<=16; i+= 8) {
            for (int j=8; j<=16; j+=8)  {
                if (OVW_MAP_tileAt(mp, p->offset_x + p->pos_x + j, p->offset_y + p->pos_y + i) == TILE)    {
                    return true;
                }
            }
        }

        return false;

    }

}

void PLR_moveOnMap(player *p, overworld_map *mp){

    for (u8 i=1; i<=absoluteValue(p->vel_x); i++)  { // fancy schmancy movement code to account for velocities that are really big
        if (PLR_canMoveX(p, mp))    {
            p->offset_x += sign(p->vel_x);
        }   else    {

            p->vel_x = sign(p->vel_x); // reset velocity back down to magnitude 1 if they hit something
            break;
        }
    }
    for (u8 i=1; i<=absoluteValue(p->vel_y); i++)  {
        if (PLR_canMoveY(p, mp))    {
            p->offset_y += sign(p->vel_y);
        }   else    {

            p->vel_y = sign(p->vel_y); // reset velocity back down to magnitude 1 if they hit something
            break;
        }
    }

    if (PLR_isTouching(p, mp, TIRE_TILE))   {

        p->stunTimer = 30 - p->stunTimer; // stun the player temporarily (the - p->stunTimer should prevent players from getting caught in a loop of tire bounces)

        p->vel_x *= -1; // invert player's motion
        p->vel_y *= -1;

        XGM_startPlayPCM(66, 0, SOUND_PCM_CH4); // play boing sound effect. (66 used instead of SFX_BOING because it's not declared in this scope. should be fine)

    }

    if (p->elevated == false)   {

        if ((PLR_isTouching(p, mp, VERTICAL_JUMP_TILE) || PLR_isTouching(p, mp, HORIZONTAL_JUMP_TILE)) && PLR_isTouching(p, mp, WALKABLE_TILE) == false)  {
            p->elevated = true; // TEMP
            if (p->jumpTimer == 0)  {
                PLR_setAnim(p, p->spr->animInd);
            }
        }

    }
    if (p->elevated == true)    {

        if (PLR_isTouching(p, mp, WALKABLE_TILE))   {
            p->elevated = false;
            if (p->jumpTimer == 0)  {
                PLR_setAnim(p, p->spr->animInd - 3);
            }
        }

    }


    if (p->lastCheckpoint == 1){
        if (PLR_isTouching(p, mp, CHECKPOINT_2_TILE))   {
            p->lastCheckpoint = 2;
        }
    }
    if (p->lastCheckpoint == 2){
        if (PLR_isTouching(p, mp, CHECKPOINT_3_TILE))   {
            p->lastCheckpoint = 3;
        }
    }
    if (p->lastCheckpoint == 3){
        if (PLR_isTouching(p, mp, CHECKPOINT_1_TILE))   {
            p->lastCheckpoint = 1;
            p->laps++;
            if (p->laps != 3)  { // if laps is 3 you're accessing an animation that doesn't exist.
                SPR_setAnim(p->HUD_lapCounter, p->laps);
            }
        }
    }

}

bool PLR_playerScreensOverlap(player *p1, player *p2)   {

    s32 dist_x = p1->offset_x - p2->offset_x; // vertical and horizontal distances between players.
    s32 dist_y = p1->offset_y - p2->offset_y;
    if (dist_x < 0) { // turn distances into absolute values
        dist_x *= -1;
    }
    if (dist_y < 0) {
        dist_y *= -1;
    }

    if (dist_x < 160 && dist_y < 56)    {
        return true; // if the players are close enough, return that they are on each others screens.
    }   else    {
        return false;
    }

}

void PLR_setMapPos(player *p, s32 relative_pos_x, s32 relative_pos_y){

    if (p->pos_y > 112) { // if on lower half of screen

        p->offset_x = relative_pos_x - p->pos_x;
        p->offset_y = relative_pos_y - (p->pos_y - 112);

    }   else    {

        p->offset_x = relative_pos_x - p->pos_x;
        p->offset_y = relative_pos_y - p->pos_y;

    }



}

void PLR_setHFlip(player *p, bool val){

    SPR_setHFlip(p->spr, val);
    p->HFlip = val;

}
void PLR_setVFlip(player *p, bool val){

    SPR_setVFlip(p->spr, val);
    p->VFlip = val;

}

void PLR_incrementTimers(player *p){

    if (p->stunTimer > 0)   {

        p->stunTimer --;
        if (p->stunTimer == 0)  { // if just now unstunned, set velocity to 0
            p->vel_x = 0;
            p->vel_y = 0;
            XGM_stopPlayPCM(SOUND_PCM_CH2); // stop any engine sounds that are playing, as player is now stopped
        }

    }
    if (p->jumpTimer > 0)   {
        p->jumpTimer--;
        if (p->jumpTimer == 0)  { // if just now stopped jumping

            PLR_setAnim(p, p->spr->animInd - 6);

        }
    }
}

void PLR_setAnim(player *p, u8 animInd){

    if (p->jumpTimer == 0)  {

        if (p->elevated == false)  {
            SPR_setAnim(p->spr, animInd);
        }   else    {
            SPR_setAnim(p->spr, animInd + 3); // if the player is elevated, show the elevated sprite instead of the default one
        }

    }   else    { // if player is jumping

        SPR_setAnim(p->spr, animInd + 6); // if the player is jumping, show the jumping sprite instead of the default one. This takes highest priority

    }



}

