/*
Copyright (C) 1994-1995 Apogee Software, Ltd.
Copyright (C) 2025-2025 Vitaliy Novichkov <admin@wohlnet,ru>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

/* Mixer functions to handle stereo inputs */

#include "_multivc.h"

extern uint8_t *MV_HarshClipTable;
extern uint8_t *MV_MixDestination;
extern uint32_t MV_MixPosition;
extern short *MV_LeftVolume;
extern short *MV_RightVolume;
extern int MV_SampleSize;
extern int MV_RightChannelOffset;

void MV_Mix8BitMono2C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos1, pos2;
   int s1c1 = 0, s2c1 = 0, s1c2, s2c2, s1, s2, o1, o2;
   uint8_t* dest;

   length >>= 1;
   if( !length )
      {
      return;
      }

   pos1 = (position >> 15) & 0xFFFFFFFE; position += rate;
   pos2 = (position >> 15) & 0xFFFFFFFE; position += rate;

   s1c1 = start[pos1];
   s2c1 = start[pos2];

   s1c2 = start[pos1 + 1];
   s2c2 = start[pos2 + 1];

   s1 = (s1c1 + s1c2) / 2;
   s2 = (s2c1 + s2c2) / 2;

   dest = MV_MixDestination;

   do
      {
      o1 = (signed char)MV_LeftVolume[s1] + dest[0];
      o2 = (signed char)MV_LeftVolume[s2] + dest[MV_SampleSize];

      dest[0] = MV_HarshClipTable[128 + o1];
      dest[MV_SampleSize] = MV_HarshClipTable[128 + o2];

      dest += MV_SampleSize * 2;

      pos1 = (position >> 15) & 0xFFFFFFFE; position += rate;
      pos2 = (position >> 15) & 0xFFFFFFFE; position += rate;

      s1c1 = start[pos1];
      s2c1 = start[pos2];

      s1c2 = start[pos1 + 1];
      s2c2 = start[pos2 + 1];

      s1 = (s1c1 + s1c2) / 2;
      s2 = (s2c1 + s2c2) / 2;
      }
   while( --length );

   MV_MixDestination = dest;
   MV_MixPosition = position;
   }


void MV_Mix8BitStereo2C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos;
   uint8_t* dest;
   int o1, o2, s1, s2;

   if( !length )
      {
      return;
      }

   pos = (position >> 15) & 0xFFFFFFFE;
   s1 = start[pos];
   s2 = start[pos + 1];

   dest = MV_MixDestination;

   do
      {
      o1 = (signed char)MV_LeftVolume[s1] + dest[0];
      o2 = (signed char)MV_RightVolume[s2] + dest[MV_RightChannelOffset];

      dest[0] = MV_HarshClipTable[128 + o1];
      dest[MV_RightChannelOffset] = MV_HarshClipTable[128 + o2];

      dest += MV_SampleSize;

      position += rate;

      pos = (position >> 15) & 0xFFFFFFFE;
      s1 = start[pos];
      s2 = start[pos + 1];
      }
   while( --length );

   MV_MixDestination = dest;
   MV_MixPosition = position;
   }


void MV_Mix16BitMono2C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos1, pos2;
   uint8_t* dest;
   int s1c1, s2c1, s1c2, s2c2, s1, s2, o1, o2;

   length >>= 1;
   if( !length )
      {
      return;
      }

   pos1 = (position >> 15) & 0xFFFFFFFE; position += rate;
   pos2 = (position >> 15) & 0xFFFFFFFE; position += rate;

   s1c1 = start[pos1];
   s2c1 = start[pos2];

   s1c2 = start[pos1 + 1];
   s2c2 = start[pos2 + 1];

   s1 = (s1c1 + s1c2) / 2;
   s2 = (s2c1 + s2c2) / 2;

   dest = MV_MixDestination;

   do
      {
      o1 = (short)MV_LeftVolume[s1] + *(short*)dest;
      o2 = (short)MV_LeftVolume[s2] + *(short*)(dest + MV_SampleSize);

      if( o1 < -32768 )
         {
         o1 = -32768;
         }
      else if( o1 > 32767 )
         {
         o1 = 32767;
         }
      *(short*)dest = o1;

      if( o2 < -32768 )
         {
         o2 = -32768;
         }
      else if( o2 > 32767 )
         {
         o2 = 32767;
         }

      *(short*)(dest + MV_SampleSize) = o2;

      dest += MV_SampleSize * 2;

      pos1 = (position >> 15) & 0xFFFFFFFE; position += rate;
      pos2 = (position >> 15) & 0xFFFFFFFE; position += rate;

      s1c1 = start[pos1];
      s2c1 = start[pos2];

      s1c2 = start[pos1 + 1];
      s2c2 = start[pos2 + 1];

      s1 = (s1c1 + s1c2) / 2;
      s2 = (s2c1 + s2c2) / 2;
      }
      while (--length);

   MV_MixDestination = dest;
   MV_MixPosition = position;

   }


void MV_Mix16BitStereo2C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos;
   int s1, s2, o1, o2;
   uint8_t *dest;

   if( !length )
      {
      return;
      }

   pos = (position >> 15) & 0xFFFFFFFE;
   s1 = start[pos];
   s2 = start[pos + 1];

   dest = MV_MixDestination;

   do
      {
      o1 = (short)MV_LeftVolume[s1] + *(short*)dest;
      o2 = (short)MV_RightVolume[s2] + *(short*)(dest + MV_RightChannelOffset);

      if( o1 < -32768 )
         {
         o1 = -32768;
         }
      else if( o1 > 32767 )
         {
         o1 = 32767;
         }
      *(short*)dest = o1;

      if( o2 < -32768 )
         {
         o2 = -32768;
         }
      else if( o2 > 32767 )
         {
         o2 = 32767;
         }
      *(short*)(dest + MV_RightChannelOffset) = o2;

      dest += MV_SampleSize;

      position += rate;

      pos = (position >> 15) & 0xFFFFFFFE;
      s1 = start[pos];
      s2 = start[pos + 1];
      }
      while( --length );

   MV_MixDestination = dest;
   MV_MixPosition = position;
   }


void MV_Mix8BitMono162C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos1, pos2;
   int s1c1, s2c1, s1c2, s2c2, s1, s2, o1, o2;
   uint8_t *dest;

   start++;

   length >>= 1;
   if( !length )
      {
      return;
      }

   pos1 = (position >> 14) & 0xFFFFFFFC; position += rate;
   pos2 = (position >> 14) & 0xFFFFFFFC; position += rate;

   s1c1 = 128 + (signed char)start[pos1];
   s2c1 = 128 + (signed char)start[pos2];

   s1c2 = 128 + (signed char)start[pos1 + 2];
   s2c2 = 128 + (signed char)start[pos2 + 2];

   s1 = (s1c1 + s1c2) / 2;
   s2 = (s2c1 + s2c2) / 2;

   dest = MV_MixDestination;


   do
      {
      o1 = (signed char)MV_LeftVolume[s1] + dest[0];
      o2 = (signed char)MV_LeftVolume[s2] + dest[MV_SampleSize];

      dest[0] = MV_HarshClipTable[128 + o1];
      dest[MV_SampleSize] = MV_HarshClipTable[128 + o2];

      dest += MV_SampleSize * 2;

      pos1 = (position >> 14) & 0xFFFFFFFC; position += rate;
      pos2 = (position >> 14) & 0xFFFFFFFC; position += rate;

      s1c1 = 128 + (signed char)start[pos1];
      s2c1 = 128 + (signed char)start[pos2];

      s1c2 = 128 + (signed char)start[pos1 + 2];
      s2c2 = 128 + (signed char)start[pos2 + 2];

      s1 = (s1c1 + s1c2) / 2;
      s2 = (s2c1 + s2c2) / 2;
      }
      while( --length );

   MV_MixDestination = dest;
   MV_MixPosition = position;
   }


void MV_Mix8BitStereo162C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos;
   int s1, s2, o1, o2;
   uint8_t *dest;

   start++;
   if( !length )
      {
      return;
      }

   pos = (position >> 14) & 0xFFFFFFFC;

   s1 = 128 + (signed char)start[pos];
   s2 = 128 + (signed char)start[pos + 2];

   dest = MV_MixDestination;

   do
      {
      o1 = (signed char)MV_LeftVolume[s1] + dest[0];
      o2 = (signed char)MV_RightVolume[s2] + dest[MV_RightChannelOffset];

      dest[0] = MV_HarshClipTable[128 + o1];
      dest[MV_RightChannelOffset] = MV_HarshClipTable[128 + o2];

      dest += MV_SampleSize;

      position += rate;

      pos = (position >> 14) & 0xFFFFFFFC;

      s1 = 128 + (signed char)start[pos];
      s2 = 128 + (signed char)start[pos + 2];
      }
      while( --length );

   MV_MixDestination = dest;
   MV_MixPosition = position;
   }


void MV_Mix16BitMono162C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos1;
   int s1, s2, ss, o1;
   short *start2 = (short*)start;
   uint8_t *dest;

   if( !length )
      {
      return;
      }

   pos1 = (position >> 15) & 0xFFFFFFFE; position += rate;

   s1 = start2[pos1];
   s2 = start2[pos1 + 1];
   ss = ((s1 + s2) / 2) ^ 32768;

   dest = MV_MixDestination;

   do
      {
      o1 = (MV_LeftVolume[(char)ss] >> 8) + (short)MV_LeftVolume[(char)(ss >> 8)] + 128 + *(short*)dest;

      if( o1 < -32768 )
         {
         o1 = -32768;
         }
      else if( o1 > 32767 )
         {
         o1 = 32767;
         }
      *(short*)dest = o1;

      dest += MV_SampleSize;

      pos1 = (position >> 15) & 0xFFFFFFFE; position += rate;

      s1 = start2[pos1];
      s2 = start2[pos1 + 1];
      ss = ((s1 + s2) / 2) ^ 32768;
      }
      while( --length );

   MV_MixDestination = dest;
   MV_MixPosition = position;
   }


void MV_Mix16BitStereo162C
   (
   unsigned long position, unsigned long rate, uint8_t *start, unsigned long length
   )

   {
   uint32_t pos1;
   int s1, s2, o1, o2;
   short *start2 = (short*)start;
   uint8_t *dest;

   if( !length )
      {
      return;
      }

   pos1 = (position >> 15) & 0xFFFFFFFE;

   s1 = start2[pos1] ^ 32768;
   s2 = start2[pos1 + 1] ^ 32768;

   dest = MV_MixDestination;

   do
      {
      o1 = (MV_LeftVolume[(char)s1] >> 8) + (short)MV_LeftVolume[(char)(s1 >> 8)] + 128 + *(short*)dest;
      o2 = (MV_RightVolume[(char)s2] >> 8) + (short)MV_RightVolume[(char)(s2 >> 8)] + 128 + *(short*)(dest + MV_RightChannelOffset);

      if( o1 < -32768 )
         {
         o1 = -32768;
         }
      else if( o1 > 32767 )
         {
         o1 = 32767;
         }
      *(short*)dest = o1;

      if( o2 < -32768 )
         {
         o2 = -32768;
         }
      else if( o2 > 32767 )
         {
         o2 = 32767;
         }
      *(short*)(dest + MV_RightChannelOffset) = o2;

      dest += MV_SampleSize;

      position += rate;

      pos1 = (position >> 15) & 0xFFFFFFFE;

      s1 = start2[pos1] ^ 32768;
      s2 = start2[pos1 + 1] ^ 32768;
      }
      while( --length );

   MV_MixDestination = dest;
   MV_MixPosition = position;
   }
