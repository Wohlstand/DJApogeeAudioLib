//
//
// Copyright (C) 2023-2024 Frenkel Smeijers
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef DJCONFIG_H
#define DJCONFIG_H

#include <dpmi.h>
#include <go32.h>
#include <stdint.h>
#include <sys/nearptr.h>
#include <crt0.h>

#define FP_SEG(x) (uint16_t)((uint32_t)(x) >> 16) /* grab 16 most significant bits */
#define FP_OFF(x) (uint16_t)((uint32_t)(x)) /* grab 16 least significant bits */

static inline void *MK_FP
   (
   uint16_t seg, uint16_t ofs
   )

   {
   if( !(_crt0_startup_flags & _CRT0_FLAG_NEARPTR ) )
      {
      if ( !__djgpp_nearptr_enable() )
         {
         return (void *)0;
         }
      }

   return (void *)( seg * 16 + ofs + __djgpp_conventional_base );
   }

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))

#define mkdir(x) mkdir(x,0)

//DJGPP doesn't inline inp, outp and outpw,
//but it does inline inportb, outportb and outportw
#define inp(port)           inportb(port)
#define outp(port,data)     outportb(port,data)
#define outpw(port,data)    outportw(port,data)

#define cdecl
#define _cdecl
#define interrupt
#define _interrupt
#define __interrupt
#define far
#define _far
#define __far

#define replaceInterrupt( OldInt, NewInt, vector, handler ) \
   _go32_dpmi_get_protected_mode_interrupt_vector(vector, &OldInt); \
\
   NewInt.pm_selector = _go32_my_cs(); \
   NewInt.pm_offset = (int32_t)handler; \
   _go32_dpmi_allocate_iret_wrapper(&NewInt); \
   _go32_dpmi_set_protected_mode_interrupt_vector(vector, &NewInt)

#define restoreInterrupt( vector, OldInt, NewInt ) \
   _go32_dpmi_set_protected_mode_interrupt_vector(vector, &OldInt); \
   _go32_dpmi_free_iret_wrapper(&NewInt)

#define _chain_intr( OldInt ) \
   asm \
   ( \
      "cli \n" \
      "pushfl \n" \
      "lcall *%0" \
      : \
      : "m" (OldInt.pm_offset) \
   )

#endif // DJCONFIG_H
