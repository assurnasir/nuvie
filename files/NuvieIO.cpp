/*
 *  NuvieIO.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Jul 01 2003.
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
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "nuvieDefs.h"

#include "NuvieIO.h"

NuvieIO::NuvieIO()
{
 size = 0;
 pos = 0;
}

NuvieIO::~NuvieIO()
{
}

unsigned char *NuvieIO::readAll()
{
 uint32 bytes_read;
 return readBuf(size, &bytes_read);
}

unsigned char *NuvieIO::readBuf(uint32 read_size, uint32 *bytes_read)
{
 unsigned char *buf;

 *bytes_read = 0;

 if(pos + read_size > size)
    return NULL;

 buf = (unsigned char *)malloc(read_size);
 if(buf == NULL)
   return NULL;

 if(readToBuf(buf, read_size) == false)
   {
    free(buf);
    return NULL;
   }

 *bytes_read = read_size;

 return buf;
}


// NuvieIOBuffer

NuvieIOBuffer::NuvieIOBuffer() : NuvieIO()
{
 data = NULL;
 copied_data = false;
}

NuvieIOBuffer::~NuvieIOBuffer()
{
 if (data != NULL)
   free(data);
}

bool NuvieIOBuffer::open(unsigned char *buf, uint32 buf_size, bool copy_buf)
{
 if(data != NULL)
   return false;

 if(copy_buf == NUVIE_BUF_COPY)
  {
   copied_data = true;
   data = (unsigned char *)malloc(buf_size);
   if(data == NULL)
     {
      printf("Error: NuvieIOBuffer::open() allocating %d bytes.\n",buf_size);
      return false;
     }

   memcpy(data,buf,buf_size);
  }
 else
   data = buf;

 size = buf_size;

 return true;
}

void NuvieIOBuffer::close()
{
 size = 0;
 pos = 0;

 if(copied_data && data != NULL)
   free(data);

 data = NULL;

}

uint8 NuvieIOBuffer::read1()
{
 if(pos >= size)
   return 0;

 return data[pos++];
}

uint16 NuvieIOBuffer::read2()
{
 uint16 val;

  if(pos >= size-2)
   return 0;

 val = data[pos] + (data[pos+1]<<8);
 pos += 2;

 return val;
}

uint32 NuvieIOBuffer::read4()
{
 uint32 val;

 if(pos >= size-4)
   return 0;

 val = (data[pos] + (data[pos+1]<<8) + (data[pos+2]<<16) + (data[pos+3]<<24));
 pos += 4;

 return val;
}

bool NuvieIOBuffer::readToBuf(unsigned char *buf, uint32 buf_size)
{
 if(pos + buf_size > size || buf == NULL)
   return false;

 memcpy(buf,&data[pos],buf_size);

 pos += buf_size;

 return true;
}

bool NuvieIOBuffer::write1(uint8 src)
{
 if(pos >= size)
   return false;

 data[pos] = src;
 pos++;

 return true;
}


bool NuvieIOBuffer::write2(uint16 src)
{
 if(pos >= size-2)
   return false;

 data[pos] = src & 0xff;
 data[pos+1] = (src >> 8) & 0xff;
 pos += 2;

 return true;
}

bool NuvieIOBuffer::write4(uint32 src)
{
 unsigned char *ptr;
 if(pos >= size-4)
   return false;

 ptr = &data[pos];

 *ptr++ = src & 0xff;
 *ptr++ = (src >> 8) & 0xff;
 *ptr++ = (src >> 16) & 0xff;
 *ptr++ = (src >> 24) & 0xff;

 pos += 4;

 return true;
}

uint32 NuvieIOBuffer::writeBuf(unsigned char *src, uint32 src_size)
{
  if(pos + src_size > size || src == NULL)
   return 0;

 memcpy(&data[pos],src,src_size);

 pos += src_size;

 return src_size;
}

uint32 NuvieIOBuffer::write(NuvieIO *src)
{
 return 0;
}


void NuvieIOBuffer::seek(uint32 new_pos)
{
    if(data && new_pos < size)
        pos = new_pos;
}
