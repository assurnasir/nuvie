#ifndef __PartyView_h__
#define __PartyView_h__

/*
 *  PartyView.h
 *  Nuvie
 *
 *  Created by Eric Fry on Fri Aug 22 2003.
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

#include <string>

#include "U6def.h"
#include "Configuration.h"
#include "View.h"
#include "Screen.h"
#include "Text.h"
#include "TileManager.h"
#include "ObjManager.h"
#include "Party.h"
#include "Player.h"

class PartyView : public View {

 Player *player;
 void *view_manager;
 
 public:
 PartyView(Configuration *cfg);
 ~PartyView();
 
 bool init(void *vm, uint16 x, uint16 y, Text *t, Party *p, Player *pl, TileManager *tm, ObjManager *om);
 GUI_status MouseUp(int x,int y,int button);
 void Display(bool full_redraw);
 
 protected:
 
 void display_sun_moon_strip();
 void display_surface_strip();
 void display_dungeon_strip();
 
};

#endif /* __PartyView_h__ */

