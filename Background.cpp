/*
 *  Background.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Sun Aug 24 2003.
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

#include <ctype.h>
#include <string>

#include "nuvieDefs.h"
#include "Configuration.h"
#include "U6misc.h"
#include "U6Lib_n.h"
#include "U6Bmp.h"
#include "Dither.h"
#include "Background.h"
#include "MapWindow.h"
#include "GUI.h"

Background::Background(Configuration *cfg) : GUI_Widget(NULL)
{
 config = cfg;
 config->value("config/GameType",game_type);

 bg_w = 0; bg_h = 0; border_width = 0;
 background = NULL;
 x_off = Game::get_game()->get_game_x_offset();
 y_off = Game::get_game()->get_game_y_offset();


 Init(NULL, 0,0,Game::get_game()->get_screen()->get_width(),Game::get_game()->get_screen()->get_height());
}

Background::~Background()
{
 if(background)
  delete background;
}

bool Background::init()
{
 std::string filename;

 if(!Game::get_game()->is_new_style())
 {
	 switch(game_type)
	   {
		case NUVIE_GAME_U6 : config_get_path(config,"paper.bmp",filename);
							 background = (U6Shape *) new U6Bmp();
							 if(background->load(filename) == false)
							   return false;
							 if(Game::get_game()->is_original_plus()) {
								 border_width = 158;
								 right_bg_x_off = x_off + Game::get_game()->get_game_width() - 152;
								 left_bg_x_off = x_off + Game::get_game()->get_game_width() - border_width;
							 }
							 break;

		case NUVIE_GAME_MD :
							 background = new U6Shape();
							 background->load_WoU_background(config, game_type);
							 if(Game::get_game()->is_original_plus()) {
								 border_width = 144;
								 left_bg_x_off = x_off + Game::get_game()->get_game_width() - border_width;
							 }
							 break;

		case NUVIE_GAME_SE :
							 background = new U6Shape();
							 background->load_WoU_background(config, game_type);
							 if(Game::get_game()->is_original_plus()) {
								 border_width = 142;
								 left_bg_x_off = x_off + Game::get_game()->get_game_width() - border_width;
							 }
							 break;
	   }

	 background->get_size(&bg_w,&bg_h);

	 Game::get_game()->get_dither()->dither_bitmap(background->get_data(), bg_w, bg_h, DITHER_NO_TRANSPARENCY);
 }
 return true;
}

void Background::Display(bool full_redraw)
{
 if(full_redraw || update_display || Game::get_game()->is_original_plus_full_map())
   {
    if(Game::get_game()->is_original_plus()) {
        if(Game::get_game()->is_original_plus_cutoff_map())
            screen->clear(area.x,area.y,area.w,area.h,NULL);
        else if(full_redraw || update_display) { // need to clear null background when we have a game size smaller than the screen
            uint16 game_width = Game::get_game()->get_game_width();
            uint16 game_height = Game::get_game()->get_game_height();
            if(x_off > 0) { // centered
                screen->clear(area.x, area.y, x_off, area.h, NULL); // left side
                screen->clear(x_off + game_width, area.y, x_off, area.h, NULL); // right side
            } else if(area.w > game_width) { // upper_left position
                screen->clear(game_width, area.y, area.w - game_width, area.h, NULL); // right side
            }
            if(y_off > 0) {  // centered
                screen->clear(area.x, area.y, area.w, y_off, NULL); // top
                screen->clear(area.x, y_off + game_height, area.w, y_off, NULL); // bottom
            } else if(area.h > game_height) { // upper_left position
                screen->clear(area.x, game_height, area.w, area.h - game_height, NULL); // bottom
            }
        }
        unsigned char *ptr = background->get_data();
        if(game_type == NUVIE_GAME_U6) {
            ptr += (bg_w - 152);
            screen->blit(right_bg_x_off, y_off, ptr, 8, 152, bg_h, bg_w, true);
            screen->blit(left_bg_x_off, y_off, background->get_data(), 8, 6, bg_h, bg_w, true);
        } else {
            if(game_type == NUVIE_GAME_MD)
                screen->fill(0, left_bg_x_off, y_off, border_width, bg_h); // background has transparent parts that should be black
            ptr += (bg_w - border_width);
            screen->blit(left_bg_x_off, y_off, ptr, 8, border_width, bg_h, bg_w, true);
        }
    } else {
        screen->clear(area.x,area.y,area.w,area.h,NULL);
        if(Game::get_game()->is_orig_style())
            screen->blit(x_off, y_off, background->get_data(), 8,  bg_w, bg_h, bg_w, true);
    }
    update_display = false;
    screen->update(0,0,area.w,area.h);
   }

 return;
}

bool Background::drag_accept_drop(int x, int y, int message, void *data)
{
	GUI::get_gui()->force_full_redraw();
	DEBUG(0,LEVEL_DEBUGGING,"Background::drag_accept_drop()\n");
	if(Game::get_game()->is_original_plus_full_map() && message == GUI_DRAG_OBJ) { // added to gui before the map window so we need to redirect
		MapWindow *map_window = Game::get_game()->get_map_window();
		if(!map_window) // should be initialized before drops occur but we will play it safe
			return false;
		if(Game::get_game()->get_game_width() > x - x_off && x >= x_off // make sure we are on the map window
		   && Game::get_game()->get_game_height() > y - y_off && y >= y_off) {
			if(x >= left_bg_x_off && y <= 200 + y_off) // over background image
				return false;
			return map_window->drag_accept_drop(x, y, message, data);
		}
	}
	return false;
}

void Background::drag_perform_drop(int x, int y, int message, void *data)
{
	DEBUG(0,LEVEL_DEBUGGING,"Background::drag_perform_drop()\n");
	if(message == GUI_DRAG_OBJ) // should only happen with original_plus_full_map
		Game::get_game()->get_map_window()->drag_perform_drop(x, y, message, data);
}
