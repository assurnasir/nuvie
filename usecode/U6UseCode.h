#ifndef __U6UseCode_h__
#define __U6UseCode_h__
/*
 *  U6UseCode.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri May 30 2003.
 *  Copyright (c) 2003. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "U6def.h"
#include "Configuration.h"
#include "ObjManager.h"
#include "MsgScroll.h"
#include "Player.h"
#include "UseCode.h"

//object numbers
#define OBJ_U6_VORTEX_CUBE      62
#define OBJ_U6_KEY              64

#define OBJ_U6_GARLIC           67
#define OBJ_U6_SNAKE_VENOM      92
#define OBJ_U6_GRAPES           95
#define OBJ_U6_BUTTER           96

#define OBJ_U6_CHEST            98

#define OBJ_U6_WINE            115
#define OBJ_U6_MEAD            116
#define OBJ_U6_ALE             117

#define OBJ_U6_CANDLE          122
#define OBJ_U6_MIRROR          123

#define OBJ_U6_BREAD           128
#define OBJ_U6_MEAT_PORTION    129
#define OBJ_U6_ROLLS           130
#define OBJ_U6_CAKE            131
#define OBJ_U6_CHEESE          132
#define OBJ_U6_HAM             133
#define OBJ_U6_HORSE_CHOPS     135

#define OBJ_U6_PICTURE         143
#define OBJ_U6_CANDELABRA      145
#define OBJ_U6_PERSON_SLEEPING 146
#define OBJ_U6_BOOK            151
#define OBJ_U6_SCROLL          152
#define OBJ_U6_CLOCK           159
#define OBJ_U6_BED             163
#define OBJ_U6_FIREPLACE       164
#define OBJ_U6_SWITCH          174
#define OBJ_U6_ELECTRIC_FIELD  175
#define OBJ_U6_DRAWER          176
#define OBJ_U6_BARREL          186
#define OBJ_U6_BAG             188
#define OBJ_U6_HONEY_JAR       183

#define OBJ_U6_HONEY           184

#define OBJ_U6_CRATE           192
#define OBJ_U6_BRAZIER         206

#define OBJ_U6_MEAT            209
#define OBJ_U6_RIBS            210

#define OBJ_U6_MOUSEHOLE       213
#define OBJ_U6_CHAIR           252
#define OBJ_U6_CROSS           254
#define OBJ_U6_TOMBSTONE       255
#define OBJ_U6_LEVER           268
#define OBJ_U6_DRAWBRIDGE      269
#define OBJ_U6_V_PASSTHROUGH   278
#define OBJ_U6_H_PASSTHROUGH   280
#define OBJ_U6_CRANK           288

#define OBJ_U6_OAKEN_DOOR      297
#define OBJ_U6_WINDOWED_DOOR   298
#define OBJ_U6_CEDAR_DOOR      299
#define OBJ_U6_STEEL_DOOR      300

#define OBJ_U6_DOORWAY         301
#define OBJ_U6_LADDER          305
#define OBJ_U6_HOLE            308
#define OBJ_U6_PORTCULLIS      310
#define OBJ_U6_CAVE            326

#define OBJ_U6_THRONE          327

#define OBJ_U6_SIGN            332
#define OBJ_U6_SECRET_DOOR     334

#define OBJ_U6_JESTER          383
#define OBJ_U6_MUSICIAN        392

#define OBJ_U6_QUEST_GATE      416

#define OBJ_U6_DRAGON_EGG      417

#define OBJ_U6__LAST_          417

class U6UseCode;

struct u6_uc_s // usecode definition (object)
{
 uint16 obj_n; // obj and frame numbers must match
 uint8 frame_n; // 255 matches any frame
 uint8 dist; // distance
 uint8 trigger; // event(s) to call ucf with
 bool (U6UseCode::*ucf)(Obj *, uint8); // usecode function
 void set(uint16 o, uint8 f, uint8 d, uint8 e, bool (U6UseCode::*u)(Obj *, uint8))
 { obj_n = o; frame_n = f; dist = d; trigger = e; ucf = u; }
};

class U6UseCode: public UseCode
{
 typedef struct u6_uc_s uc_obj;
 uc_obj *uc_objects;
 uint16 uc_object_count; // count of game object<->usecode objects

 public:
 
 U6UseCode(Configuration *cfg);
 ~U6UseCode();
 void init_objects();
 
 bool use_obj(Obj *obj, Obj *src_obj=NULL);
 bool use_obj(Obj *obj, Actor *actor);
 bool look_obj(Obj *obj, Actor *actor);
 bool pass_obj(Obj *obj, Actor *actor);

 bool is_unlocked_door(Obj *obj) { return(obj->obj_n >= 297 && obj->obj_n <= 300 && obj->frame_n != 9 && obj->frame_n != 11); }
 bool is_locked_door(Obj *obj)   { return(obj->obj_n >= 297 && obj->obj_n <= 300 && (obj->frame_n == 9 || obj->frame_n == 11)); }
 bool is_food(Obj *obj);
 bool can_use(Obj *obj);
 bool can_look(Obj *obj);
 bool can_pass(Obj *obj);

 protected:
 sint16 get_ucobject_index(uint16 n, uint8 f = 0);
 bool uc_event(sint16 uco, uint8 ev, Obj *obj);
 
 bool use_door(Obj *obj);
 bool use_ladder(Obj *obj);
 bool use_passthrough(Obj *obj);
 bool use_switch(Obj *obj, uint16 target_obj_n);
 bool use_crank(Obj *obj);
 bool use_food(Obj *obj, uint8 ev);
 
 Obj *drawbridge_find(Obj *crank_obj);
 void drawbridge_open(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_close(uint16 x, uint16 y, uint8 level, uint16 b_width);
 void drawbridge_remove(uint16 x, uint16 y, uint8 level, uint16 *bridge_width);

 bool use_firedevice_message(Obj *obj, bool lit);

 bool pass_quest_barrier(Obj *obj, uint8 ev);
 bool look_mirror(Obj *obj, uint8 ev);
 bool look_sign(Obj *obj, uint8 ev);
 bool look_clock(Obj *obj, uint8 ev);

#if 0 /* names for other events? */
 bool approach_mirror(Obj *obj, uint8 ev); // change to reflect frame
 bool attack_mirror(Obj *obj, uint8 ev); // break mirror
 bool drop_mirror(Obj *obj, uint8 ev); // call after drop (break mirror)
#endif
};

#endif /* __U6UseCode_h__ */
