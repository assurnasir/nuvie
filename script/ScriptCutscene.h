#ifndef __ScriptCutscene_h__
#define __ScriptCutscene_h__

/*
 *  Script.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Sep 20 2011.
 *  Copyright (c) 2011. All rights reserved.
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

#include "GUI.h"
#include "GUI_widget.h"
#include "U6Shape.h"
#include "WOUFont.h"
#include "Configuration.h"

class SoundManager;
class Font;
class U6LineWalker;
class Cursor;

#include "lua.hpp"

class CSImage {
public:
	U6Shape *orig_shp;
	U6Shape *scaled_shp;
	U6Shape *shp;
	uint16 scale;
	uint16 refcount;

	CSImage(U6Shape *shape)
	{
		orig_shp = shape;
		scaled_shp = NULL;
		shp = shape;
		scale = 100;
		refcount = 0;
	}

	void setScale(uint16 percentage);
	uint16 getScale() { return scale; }

	virtual void updateEffect() { };
};

#define STAR_FIELD_NUM_STARS 70

class CSStarFieldImage : public CSImage {

private:
	uint16 w;
	uint16 h;
	struct {
		uint8 color;
		U6LineWalker *line;
	} stars[STAR_FIELD_NUM_STARS];
public:
	CSStarFieldImage(U6Shape *shape);
	virtual void updateEffect();
};

struct CSSprite {
	sint16 x;
	sint16 y;
	uint8 opacity;
	CSImage *image;
	bool visible;
	SDL_Rect clip_rect;
	std::string text;
	uint16 text_color;
	bool text_centred;

	CSSprite()
	{
		x = 0;
		y = 0;
		opacity = 255;
		image = NULL;
		visible = false;
		clip_rect.x = 0;
		clip_rect.y = 0;
		clip_rect.w = 0;
		clip_rect.h = 0;
		text = "";
		text_color = 0xffff;
		text_centred = false;
	}
};

struct CSMidGameData {
  std::vector<std::string> text;
  std::vector<CSImage *> images;
};

void nscript_init_cutscene(lua_State *L, Configuration *cfg, GUI *gui, SoundManager *sm);

class ScriptCutscene : public GUI_Widget
{
private:
	Configuration *config;
	GUI *gui;
	Cursor *cursor;
	std::list<CSSprite *> sprite_list; // in paint order
	Screen *screen;
	uint8 *palette;
	SoundManager *sound_manager;
	WOUFont *font;
	SDL_Rect clip_rect;
	uint16 x_off, y_off;
	uint32 next_time;
	uint32 loop_interval;
	uint8 screen_opacity;
	uint8 bg_color;
	bool solid_bg;
	bool rotate_game_palette;

public:
	ScriptCutscene(GUI *g, Configuration *cfg, SoundManager *sm);
	~ScriptCutscene();

	std::vector<std::string> load_text(const char *filename, uint8 idx);

	std::vector<CSMidGameData> load_midgame_file(const char *filename);

	CSImage *load_image(const char *filename, int idx, int sub_idx=0);
	std::vector<std::vector<CSImage *> > load_all_images(const char *filename);
	void add_sprite(CSSprite *s) { sprite_list.push_back(s); }
	void remove_sprite(CSSprite *s) { sprite_list.remove(s); }

	void load_palette(const char *filename, int idx);
	void set_palette_entry(uint8 idx, uint8 r, uint8 g, uint8 b);
	void rotate_palette(uint8 idx, uint8 length);
	void set_screen_opacity(uint8 new_opacity);
	void enable_game_palette_rotation(bool val) { rotate_game_palette = val; }

	void set_update_interval(uint16 interval);
	void update();

	void wait();
	void Display(bool full_redraw);
	virtual void Hide();

	void print_text(CSImage *image, const char *string, uint16 *x, uint16 *y, uint16 startx, uint16 width, uint8 color);

	SoundManager *get_sound_manager() { return sound_manager; }

	uint16 get_x_off() { return x_off; }
	uint16 get_y_off() { return y_off; }

	Font *get_font() { return (Font *)font; }
	Configuration *get_config() { return config; }

	void hide_sprites();

	void set_bg_color(uint8 new_color) { bg_color = new_color; }
  void set_solid_bg(bool value) { solid_bg = value; }

	Screen *get_screen() { return screen; }

	uint16 get_text_width(const char *text) { return font->getStringWidth(text); }

private:
  bool is_lzc(const char *filename);
  CSImage *load_image_from_lzc(std::string filename, uint16 idx, uint16 sub_idx);
  void display_wrapped_text(CSSprite *s);
  int display_wrapped_text_line(std::string str, uint8 text_color, int x, int y);
};

ScriptCutscene *get_cutscene();

#endif /* __ScriptCutscene_h__ */
