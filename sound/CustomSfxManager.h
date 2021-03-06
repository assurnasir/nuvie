/*
 *  CustomSfxManager.h
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Feb 1 2011.
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

#ifndef CUSTOMSFXMANAGER_H
#define CUSTOMSFXMANAGER_H

#include "nuvieDefs.h"
#include "Configuration.h"
#include "mixer.h"
#include "decoder/wave/wave.h"
#include "SfxManager.h"
#include "audiostream.h"


class CustomSfxManager : public SfxManager
{
 public:
	CustomSfxManager(Configuration *cfg, Audio::Mixer *m);
	~CustomSfxManager();



 bool playSfx(SfxIdType sfx_id, uint8 volume);
 bool playSfxLooping(SfxIdType sfx_id, Audio::SoundHandle *handle, uint8 volume);

 void playSoundSample(uint16 sample_num, Audio::SoundHandle *looping_handle, uint8 volume);

 private:
 bool loadSfxMapFile(std::string cfg_filename, std::map<uint16,uint16> *m);

 private:
 std::string custom_filepath;

 std::map<uint16,uint16> *sfx_map;

};

#endif
