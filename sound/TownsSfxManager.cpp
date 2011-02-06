/*
 *  TownsSfxManager.cpp
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

#include <string>
#include "nuvieDefs.h"
#include "Configuration.h"
#include "mixer.h"
#include "U6Lzw.h"
#include "NuvieIO.h"
#include "NuvieIOFile.h"
#include "U6Lib_n.h"
#include "TownsSfxManager.h"



typedef struct // sfx lookup
{
    uint16 sfx_id;
    uint8 towns_sample_num;
} TownsSfxLookup;

#define TOWNS_SFX_TBL_SIZE 13
//15 hail effect
//16 explosion
//17 level not high enough, no effect etc.
//18 cast magic sound
//19 resurrection tune
static const TownsSfxLookup sfx_lookup_tbl[] = {
		{NUVIE_SFX_BLOCKED, 0},
		{NUVIE_SFX_HIT, 4},
		{NUVIE_SFX_BROKEN_GLASS, 12},
		{NUVIE_SFX_BELL, 13},
		{NUVIE_SFX_FOUNTAIN, 46},
		{NUVIE_SFX_PROTECTION_FIELD, 47},
		{NUVIE_SFX_CLOCK, 6},
		{NUVIE_SFX_FIRE, 7},
		{NUVIE_SFX_RUBBER_DUCK, 3},
		{NUVIE_SFX_WATER_WHEEL, 48},
		{NUVIE_SFX_MISSLE, 9},
		{NUVIE_SFX_EXPLOSION, 16},
		{NUVIE_SFX_ATTACK_SWING, 2}
};

TownsSfxManager::TownsSfxManager(Configuration *cfg, Audio::Mixer *m) : SfxManager(cfg, m)
{
	config->pathFromValue("config/ultima6/townsdir", "sounds2.dat", sounds2dat_filepath);
	loadSound1Dat();
}

void TownsSfxManager::loadSound1Dat()
{
	std::string filename;
    U6Lzw decompressor;
    U6Lib_n lib;
    NuvieIOBuffer iobuf;
    uint32 slib32_len = 0;

	config->pathFromValue("config/ultima6/townsdir", "sounds1.dat", filename);
    unsigned char *slib32_data = decompressor.decompress_file(filename, slib32_len);

    if(slib32_len == 0)
        return;

    iobuf.open(slib32_data, slib32_len);
    free(slib32_data);

    if(!lib.open(&iobuf, 4))
        return;

    uint8 i;
    for(i=0;i<TOWNS_SFX_SOUNDS1_SIZE;i++)
    {
    	sounds1_dat[i].buf = lib.get_item(i);
    	sounds1_dat[i].len = lib.get_item_size(i);
    }
}

bool TownsSfxManager::playSfx(SfxIdType sfx_id, uint8 volume)
{
	return playSfxLooping(sfx_id, NULL, volume);
}


bool TownsSfxManager::playSfxLooping(SfxIdType sfx_id, Audio::SoundHandle *handle, uint8 volume)
{
	uint16 i = 0;
	for(i=0;i<TOWNS_SFX_TBL_SIZE;i++)
	{
		if(sfx_lookup_tbl[i].sfx_id == sfx_id)
		{
			playSoundSample(sfx_lookup_tbl[i].towns_sample_num, handle, volume);
			return true;
		}
	}
	return false;
}

void TownsSfxManager::playSoundSample(uint8 sample_num, Audio::SoundHandle *looping_handle, uint8 volume)
{
	FMtownsDecoderStream *stream = NULL;

	if(sample_num < TOWNS_SFX_SOUNDS1_SIZE)
	{
		stream = new FMtownsDecoderStream(sounds1_dat[sample_num].buf, sounds1_dat[sample_num].len);
	}
	else
	{
		stream = new FMtownsDecoderStream(sounds2dat_filepath, sample_num - TOWNS_SFX_SOUNDS1_SIZE, false); //not compressed
	}

	if(looping_handle)
	{
		Audio::LoopingAudioStream *looping_stream = new Audio::LoopingAudioStream((Audio::RewindableAudioStream *)stream, 0);
		mixer->playStream(Audio::Mixer::kPlainSoundType, looping_handle, looping_stream, -1, volume);
	}
	else
	{
		Audio::SoundHandle handle;
		mixer->playStream(Audio::Mixer::kPlainSoundType, &handle, stream, -1, volume);
	}

}
