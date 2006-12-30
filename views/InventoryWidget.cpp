/*
 *  InventoryWidget.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Mon Sep 01 2003.
 *  Copyright (c) 2003 The Nuvie Team. All rights reserved.
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
#include <cassert>
#include "nuvieDefs.h"
#include "U6LList.h"
#include "Configuration.h"

#include "GUI.h"
#include "InventoryWidget.h"
#include "Actor.h"
#include "Text.h"
#include "GameClock.h"
#include "Event.h"
#include "MsgScroll.h"
#include "TimedEvent.h"
#include "UseCode.h"

#include "InventoryFont.h"
#include "ViewManager.h"

#define USE_BUTTON 1 /* FIXME: put this in a common location */
#define ACTION_BUTTON 3
#define DRAG_BUTTON 1

static SDL_Rect arrow_rects[2] = {{0,16,8,8},{0,3*16+8,8,8}};

InventoryWidget::InventoryWidget(Configuration *cfg, GUI_CallBack *callback): GUI_Widget(NULL, 0, 0, 0, 0)
{
 config = cfg;
 callback_object = callback;

 container_obj = NULL;
 selected_obj = NULL;
 target_obj = NULL;
 ready_obj = NULL; // FIXME: this is unused but I might need it again -- SB-X
 row_offset = 0;
 
 config->value("config/GameType",game_type);
 config->value("config/input/enable_doubleclick",enable_doubleclick,true);
}

InventoryWidget::~InventoryWidget()
{

}

bool InventoryWidget::init(Actor *a, uint16 x, uint16 y, TileManager *tm, ObjManager *om, Text *t)
{
 tile_manager = tm;
 obj_manager = om;
 text = t;

 //72 =  4 * 16 + 8
 GUI_Widget::Init(NULL, x, y, 72, 64);

 set_actor(a);
 set_accept_mouseclick(true, USE_BUTTON); // accept [double]clicks from button1 (even if double-click disabled we need clicks)

 return true;
}

void InventoryWidget::set_actor(Actor *a)
{
 actor = a;
 container_obj = NULL;
 Redraw();
}

void InventoryWidget::set_prev_container()
{
  if(!container_obj)
    return;
  
  set_container(container_obj->parent_obj);
  
  return;
}

void InventoryWidget::Display(bool full_redraw)
{
 if(full_redraw || update_display)
  {
   screen->fill(0x31, area.x, area.y, area.w, area.h);
   display_inventory_container();
   display_arrows();
  }
   //screen->blit(area.x+40,area.y+16,portrait_data,8,56,64,56,false);
 display_inventory_list();

 if(full_redraw || update_display)
  {
   update_display = false;
   screen->update(area.x, area.y, area.w, area.h);
  }
 else
  {
   screen->update(area.x+8,area.y+16,area.w-8,area.h-16); // update only the inventory list
  }

}

//either an Actor or an object container.
void InventoryWidget::display_inventory_container()
{
 Tile *tile;

 if(!container_obj) //display actor
   tile = tile_manager->get_tile(actor->get_downward_facing_tile_num());
 else // display container object
   tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(container_obj->obj_n)+container_obj->frame_n);

 screen->blit(area.x+32,area.y,tile->data,8,16,16,16,true);

 return;
}

void InventoryWidget::display_inventory_list()
{
 Tile *tile, *empty_tile;
 U6LList *inventory;
 U6Link *link;
 Obj *obj = NULL;
 uint16 i,j;
 uint16 skip_num;

 empty_tile = tile_manager->get_tile(410);

 if(container_obj)
   inventory = container_obj->container;
 else
   inventory = actor->get_inventory_list();

 link = inventory->start();

 //skip row_offset rows of objects.
 skip_num = row_offset * 4;
 for(i=0;link != NULL && i < skip_num; link = link->next)
   {
    obj = (Obj *)link->data;
    if((obj->status & 0x18) != 0x18)
      i++;
   }

  //clear the screen first inventory icons, 4 x 3 tiles

  screen->fill(0x31, area.x +8, area.y + 16, 16 * 4, 16 * 3);

  for(i=0;i<3;i++)
   {
    for(j=0;j<4;j++)
      {
       if(link != NULL)
         {
          obj = (Obj *)link->data;
          if((obj->status & 0x18) == 0x18) //skip any readied objects
            {
             for(;link != NULL && (obj->status & 0x18) == 0x18; link = link->next)
                obj = (Obj *)link->data;
            }
          else
            link = link->next;

          tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(obj->obj_n)+obj->frame_n);
          if(link == NULL)
            {
             if((obj->status & 0x18) == 0x18) //last object is readied so skip it.
                tile = empty_tile;
            }
         }
        else
          tile = empty_tile;

       //tile = tile_manager->get_tile(actor->indentory_tile());


       if(tile != empty_tile)
        {
         //draw qty string for stackable items
         if(obj_manager->is_stackable(obj))       
           display_qty_string((area.x+8)+j*16,area.y+16+i*16,obj->qty);
       
         //draw special char for Keys.
         if(game_type == NUVIE_GAME_U6 && obj->obj_n == 64)
           display_special_char((area.x+8)+j*16,area.y+16+i*16,obj->quality);
        }

       screen->blit((area.x+8)+j*16,area.y+16+i*16,tile->data,8,16,16,16,true);
      }
   }
}

void InventoryWidget::display_qty_string(uint16 x, uint16 y, uint16 qty)
{
 uint8 len, i, offset;
 char buf[6];

 sprintf(buf,"%d",qty);
 len = strlen(buf);

 offset = (16 - len*4) / 2;

 for(i=0;i<len;i++)
  screen->blitbitmap(x+offset+4*i,y+11,inventory_font[buf[i]-48],3,5,0x48,0x31);

 return;
}

void InventoryWidget::display_special_char(uint16 x, uint16 y, uint8 quality)
{
 if(quality + 9 >= NUVIE_MICRO_FONT_COUNT)
   return;

 screen->blitbitmap(x+6,y+11,inventory_font[quality + 9],3,5,0x48,0x31);
}

void InventoryWidget::display_arrows()
{
 uint32 num_objects;

 if(is_showing_container())
 {
	 if(container_obj->container)
		 num_objects = container_obj->container->count();
	 else
		 num_objects = 0;
 }
 else
	 num_objects = actor->inventory_count_objects(false);

 if(num_objects <= 12) //reset row_offset if we only have one page of objects
   row_offset = 0;

 if(row_offset > 0) //display top arrow
    text->drawChar(screen, 24, area.x, area.y + 16, 0x48);

 if(num_objects - row_offset * 4 > 12) //display bottom arrow
    text->drawChar(screen, 25, area.x, area.y + 3 * 16 + 8, 0x48);
}

GUI_status InventoryWidget::MouseDown(int x, int y, int button)
{
 //Event *event = Game::get_game()->get_event();
 //MsgScroll *scroll = Game::get_game()->get_scroll();
 x -= area.x;
 y -= area.y;

 // ABOEING
 if(actor && (button == USE_BUTTON || button == ACTION_BUTTON || button == DRAG_BUTTON))
   {
    Obj *obj; // FIXME: duplicating code in DollWidget
    if((obj = get_obj_at_location(x,y)) != NULL)
      {
       // send to View
       if(callback_object->callback(INVSELECT_CB, this, obj) == GUI_PASS
          && button == DRAG_BUTTON)
           selected_obj = obj; // start dragging
       return GUI_YUM;
      }
   }
 return GUI_PASS;
}

inline uint16 InventoryWidget::get_list_position(int x, int y)
{
 uint16 list_pos;

 list_pos = ((y - 16) / 16) * 4 + (x - 8) / 16;
 list_pos += row_offset * 4;

 return list_pos;
}

Obj *InventoryWidget::get_obj_at_location(int x, int y)
{
 uint8 location;
 U6LList *inventory;
 U6Link *link;
 Obj *obj =  NULL;
 uint16 i;

 if(x >= 8 && y >= 16)
   {
    location = get_list_position(x,y); //find the postion of the object we hit in the inventory

    if(container_obj)
      inventory = container_obj->container;
    else
      inventory = actor->get_inventory_list();

    for(i=0,link = inventory->start();link != NULL && i <= location;link=link->next)
     {
      obj = (Obj *)link->data;
      if((obj->status & 0x18) != 0x18)
        i++;
     }

    if(i >= location && obj && (obj->status & 0x18) != 0x18) // don't return readied or non existent objects
      return obj;
   }

 return NULL;
}

// change container, ready/unready object, activate arrows
GUI_status InventoryWidget::MouseUp(int x,int y,int button)
{
 Event *event = Game::get_game()->get_event();
 UseCode *usecode = Game::get_game()->get_usecode();

 if(button == USE_BUTTON)
   {
    x -= area.x;
    y -= area.y;

    if(x >= 32 && x <= 48 && // hit top icon either actor or container
       y >= 0 && y <= 16)
      {
			 if(is_showing_container())
				 set_prev_container(); //return to previous container or main actor inventory
			 else
				 Game::get_game()->get_view_manager()->set_party_mode();

       Redraw();
      }

    if(HitRect(x,y,arrow_rects[0])) //up arrow hit rect
      {
       if(up_arrow())
         Redraw();
      }

    if(HitRect(x,y,arrow_rects[1])) //down arrow hit rect
      {
       if(down_arrow())
         Redraw();
      }

    // only act now if objects can't be used with DoubleClick
    if(selected_obj && !enable_doubleclick)
      {
        if(usecode->is_container(selected_obj)) // open up the container.
          {
            container_obj = selected_obj;
            Redraw();
          }
        else // attempt to ready selected object.
          {
            event->ready(selected_obj);
            Redraw();
          }
        ready_obj = NULL;
      }
    else if(selected_obj)
      {
        wait_for_mouseclick(USE_BUTTON);
        ready_obj = selected_obj;
      }
   }

 selected_obj = NULL;

 return GUI_YUM;
}

bool InventoryWidget::up_arrow()
{
 if(row_offset > 0)
  {
   row_offset--;
   return true;
  }

 return false;
}

bool InventoryWidget::down_arrow()
{
 uint32 num_objects;

 num_objects = actor->inventory_count_objects(false);

 if(num_objects - row_offset * 4 > 12)
   {
    row_offset++;
    return true;
   }

 return false;
}

GUI_status InventoryWidget::MouseMotion(int x,int y,Uint8 state)
{
 Tile *tile;

 if(selected_obj && !dragging)
   {
    dragging = true;
    tile = tile_manager->get_tile(obj_manager->get_obj_tile_num(selected_obj->obj_n)+selected_obj->frame_n);
    return gui_drag_manager->start_drag(this, GUI_DRAG_OBJ, selected_obj, tile->data, 16, 16, 8);
   }

	return GUI_PASS;
}

void InventoryWidget::drag_drop_success(int x, int y, int message, void *data)
{
printf("InventoryWidget::drag_drop_success()\n");
 dragging = false;

// handled by drop target
// if(container_obj)
//   container_obj->container->remove(selected_obj);
// else
//   actor->inventory_remove_obj(selected_obj, container_obj);

 selected_obj = NULL;
 Redraw();
}

void InventoryWidget::drag_drop_failed(int x, int y, int message, void *data)
{
printf("InventoryWidget::drag_drop_failed()\n");
 dragging = false;
 selected_obj = NULL;
}

bool InventoryWidget::drag_accept_drop(int x, int y, int message, void *data)
{
printf("InventoryWidget::drag_accept_drop()\n");
 if(message == GUI_DRAG_OBJ)
   {
    Obj *obj = (Obj*)data;
    x -= area.x;
    y -= area.y;
    if(target_obj == NULL) //we need to check this so we don't screw up target_obj on subsequent calls
      target_obj = get_obj_at_location(x,y);

    if(obj->is_in_container_new())
    {
        printf("InventoryWidget: Not from a container!\n");
        return false;
    }
    if((obj->is_in_inventory_new() || obj->is_readied() ) && obj->x != actor->get_actor_num())
    {
        printf("InventoryWidget: Cannot Move between party members!\n"); 
        return false;
    }
    if(obj->is_in_inventory_new())
    {
        printf("InventoryWidget: Already holding object!\n");
        return false;
    }

    printf("Drop Accepted\n");
    return true;
   }

 printf("Drop Refused\n");
 return false;
}

void InventoryWidget::drag_perform_drop(int x, int y, int message, void *data)
{
printf("InventoryWidget::drag_perform_drop()\n");
 Obj *obj;

 x -= area.x;
 y -= area.y;

 if(message == GUI_DRAG_OBJ)
   {
    printf("Drop into inventory.\n");
    obj = (Obj *)data;
    bool have_object = false;

    if(target_obj && target_obj->container && target_obj != obj)
      {
       container_obj = target_obj; //swap to container ready to drop item inside
      }

    if(obj->is_readied()) // unready
      {
       assert(obj->x == actor->get_actor_num());
       have_object = Game::get_game()->get_event()->unready(obj);
      }
    else // get
      {
       assert(obj->is_on_map());
       // event->newAction(GET_MODE);
       Game::get_game()->get_scroll()->display_string("Get-");
       have_object = Game::get_game()->get_event()->perform_get(obj, container_obj, actor);
      }

    if(have_object)
      {
       if(container_obj != 0) // looking into a container, so place it there
         {
          actor->inventory_remove_obj(obj);
          actor->inventory_add_object(obj, container_obj);
         }
      }
//    if(!container_obj)
//     actor->inventory_add_object(obj);
//    else
//     obj_manager->list_add_obj(container_obj->container, obj);

    Redraw();
   }

 target_obj = NULL;

 return;
}

void InventoryWidget::drag_draw(int x, int y, int message, void* data)
{
	Tile* tile;

	if (!selected_obj)
		return;

	tile = tile_manager->get_tile(obj_manager->get_obj_tile_num (selected_obj->obj_n) + selected_obj->frame_n);

	int	nx = x - 8;
	int	ny = y - 8;

	if (nx + 16 >= 320)
		nx = 303;
	else if (nx < 0)
		nx = 0;

	if (ny + 16 >= 200)
		ny = 183;
	else if (ny < 0)
		ny = 0;

	screen->blit(nx, ny, tile->data, 8, 16, 16, 16, true);
	screen->update(nx, ny, 16, 16);
}


/* Use object. */
GUI_status InventoryWidget::MouseDouble(int x, int y, int button)
{
    // we have to check if double-clicks are allowed here, since we use single-clicks
    if(!enable_doubleclick)
        return(GUI_PASS);
    Event *event = Game::get_game()->get_event();
    Obj *obj = selected_obj;

    ready_obj = NULL;
    selected_obj = NULL;

    if(!actor)
        return(GUI_YUM);
    if(!obj)
        return(MouseUp(x, y, button)); // probably hit an arrow

    if(event->newAction(USE_MODE))
        event->select_obj(obj);
    return(GUI_PASS);
}

GUI_status InventoryWidget::MouseClick(int x, int y, int button)
{
    return(MouseUp(x, y, button));
}

// change container, ready/unready object, activate arrows
GUI_status InventoryWidget::MouseDelayed(int x, int y, int button)
{
    Event *event = Game::get_game()->get_event();
    UseCode *usecode = Game::get_game()->get_usecode();

    if(ready_obj)
    {
        if(usecode->is_container(ready_obj)) // open up the container.
        {
            container_obj = ready_obj;
            Redraw();
        }
        else // attempt to ready selected object.
        {
            event->ready(ready_obj);
            Redraw();
        }
        ready_obj = NULL;
    }
    return GUI_PASS;
}
