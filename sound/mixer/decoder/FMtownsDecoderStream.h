#ifndef __FMtownsDecoderStream_h__
#define __FMtownsDecoderStream_h__
/* Created by Eric Fry 
 * Copyright (C) 2011 The Nuvie Team
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
#include <cstdio>
#include <string>
#include <list>

#include "SDL.h"
#include "audiostream.h"
class U6Lib_n;
class U6Lzw;
class NuvieIOBuffer;

using std::string;

class FMtownsDecoderStream : public Audio::AudioStream
{
public:
	FMtownsDecoderStream()
	{

	}

	FMtownsDecoderStream(unsigned char *buf, uint32 len);
	FMtownsDecoderStream(std::string filename, uint16 sample_num, bool isCompressed = true);
	~FMtownsDecoderStream();

	int readBuffer(sint16 *buffer, const int numSamples);

	/** Is this a stereo stream? */
	bool isStereo() const { return false; }

	/** Sample rate of the stream. */
	int getRate() const { return 14700; }

	/**
	 * End of data reached? If this returns true, it means that at this
	 * time there is no data available in the stream. However there may be
	 * more data in the future.
	 * This is used by e.g. a rate converter to decide whether to keep on
	 * converting data or stop.
	 */
	bool endOfData() const { return (buf_pos >= buf_len); }

protected:

	bool should_free_raw_data;
	unsigned char *raw_audio_buf;
	uint32 buf_len;
	uint32 buf_pos;
    
};

#endif /* __FMtownsDecoderStream_h__ */
