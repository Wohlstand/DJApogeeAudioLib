;
; Copyright (C) 1994-1995 Apogee Software, Ltd.
; Copyright (C) 2023 Frenkel Smeijers
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program. If not, see <https://www.gnu.org/licenses/>.
;

cpu 386

extern   _MV_HarshClipTable
extern   _MV_MixDestination
extern   _MV_MixPosition
extern   _MV_LeftVolume
extern   _MV_RightVolume
extern   _MV_SampleSize
extern   _MV_RightChannelOffset


extern   _MV_Position
extern   _MV_Rate
extern   _MV_Start
extern   _MV_Length

%ifidn __OUTPUT_FORMAT__, coff
section .text public class=CODE USE32
%elifidn __OUTPUT_FORMAT__, obj
section _TEXT public class=CODE USE32
%endif

align 4

;================
;
; MV_Mix8BitMono16
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

global  _MV_Mix8BitMono16
_MV_Mix8BitMono16:
; Two at once
        pushad
        mov     ebp, [_MV_Position]

        mov     esi, [_MV_Start]                ; Source pointer
        inc     esi

        ; Sample size
        mov     ebx, [_MV_SampleSize]
        mov     eax,apatch7+2            ; convice tasm to modify code...
        mov     [eax],bl
        mov     eax,apatch8+2            ; convice tasm to modify code...
        mov     [eax],bl
        mov     eax,apatch9+3            ; convice tasm to modify code...
        mov     [eax],bl

        ; Volume table ptr
        mov     ebx, [_MV_LeftVolume]             ; Since we're mono, use left volume
        mov     eax,apatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,apatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Harsh Clip table ptr
        mov     ebx, [_MV_HarshClipTable]
        add     ebx, 128
        mov     eax,apatch3+2            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,apatch4+2            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
        mov     edx, [_MV_Rate]
        mov     eax,apatch5+2            ; convice tasm to modify code...
        mov     [eax],edx
        mov     eax,apatch6+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     edi, [_MV_MixDestination]         ; Get the position to write to

        ; Number of samples to mix
        mov     ecx, [_MV_Length]
        shr     ecx, 1                          ; double sample count
        cmp     ecx, 0
        je      exit8m

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; apatch1 - volume table
; apatch2 - volume table
; apatch3 - harsh clip table
; apatch4 - harsh clip table
; apatch5 - sample rate
; apatch6 - sample rate

        mov     eax,ebp                         ; begin calculating first sample
        add     ebp,edx                         ; advance frac pointer
        shr     eax,16                          ; finish calculation for first sample

        mov     ebx,ebp                         ; begin calculating second sample
        add     ebp,edx                         ; advance frac pointer
        shr     ebx,16                          ; finish calculation for second sample

        movsx   eax, byte [esi+2*eax]       ; get first sample
        movsx   ebx, byte [esi+2*ebx]       ; get second sample
        add     eax, 80h
        add     ebx, 80h

        ALIGN 4
mix8Mloop:
        movzx   edx, byte [edi]             ; get current sample from destination
apatch1:
        movsx   eax, byte [2*eax+12345678h] ; volume translate first sample
apatch2:
        movsx   ebx, byte [2*ebx+12345678h] ; volume translate second sample
        add     eax, edx                        ; mix first sample
apatch9:
        movzx   edx, byte [edi + 1]         ; get current sample from destination
apatch3:
        mov     eax, [eax + 12345678h]          ; harsh clip new sample
        add     ebx, edx                        ; mix second sample
        mov     [edi], al                       ; write new sample to destination
        mov     edx, ebp                        ; begin calculating third sample
apatch4:
        mov     ebx, [ebx + 12345678h]          ; harsh clip new sample
apatch5:
        add     ebp,12345678h                   ; advance frac pointer
        shr     edx, 16                         ; finish calculation for third sample
        mov     eax, ebp                        ; begin calculating fourth sample
apatch7:
        add     edi, 2                          ; move destination to second sample
        shr     eax, 16                         ; finish calculation for fourth sample
        mov     [edi], bl                       ; write new sample to destination
apatch6:
        add     ebp,12345678h                   ; advance frac pointer
        movsx   ebx, byte [esi+2*eax]         ; get fourth sample
        movsx   eax, byte [esi+2*edx]         ; get third sample
        add     ebx, 80h
        add     eax, 80h
apatch8:
        add     edi, 2                          ; move destination to third sample
        dec     ecx                             ; decrement count
        jnz     mix8Mloop                       ; loop

        mov     [_MV_MixDestination], edi         ; Store the current write position
        mov     [_MV_MixPosition], ebp            ; return position
exit8m:
        popad
        ret


;================
;
; MV_Mix8BitStereo16
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

global  _MV_Mix8BitStereo16
_MV_Mix8BitStereo16:

        pushad
        mov     ebp, [_MV_Position]

        mov     esi, [_MV_Start]         ; Source pointer
        inc     esi

        ; Sample size
        mov     ebx, [_MV_SampleSize]
        mov     eax,bpatch8+2            ; convice tasm to modify code...
        mov     [eax],bl
 ;       mov     eax,bpatch9+2            ; convice tasm to modify code...
 ;       mov     [eax],bl

        ; Right channel offset
        mov     ebx, [_MV_RightChannelOffset]
        mov     eax,bpatch6+3            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,bpatch7+2            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Volume table ptr
        mov     ebx, [_MV_LeftVolume]
        mov     eax,bpatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx

        mov     ebx, [_MV_RightVolume]
        mov     eax,bpatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Rate scale ptr
        mov     edx, [_MV_Rate]
        mov     eax,bpatch3+2            ; convice tasm to modify code...
        mov     [eax],edx

        ; Harsh Clip table ptr
        mov     ebx, [_MV_HarshClipTable]
        add     ebx,128
        mov     eax,bpatch4+2            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,bpatch5+2            ; convice tasm to modify code...
        mov     [eax],ebx

        mov     edi, [_MV_MixDestination]         ; Get the position to write to

        ; Number of samples to mix
        mov     ecx, [_MV_Length]
        cmp     ecx, 0
        je      short exit8S

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; bpatch1 - left volume table
; bpatch2 - right volume table
; bpatch3 - sample rate
; bpatch4 - harsh clip table
; bpatch5 - harsh clip table

        mov     eax,ebp                         ; begin calculating first sample
        shr     eax,16                          ; finish calculation for first sample

        movsx   ebx, byte [esi+2*eax]       ; get first sample
        add     ebx, 80h

        ALIGN 4
mix8Sloop:
bpatch1:
        movsx   eax, byte [2*ebx+12345678h] ; volume translate left sample
        movzx   edx, byte [edi]             ; get current sample from destination
bpatch2:
        movsx   ebx, byte [2*ebx+12345678h] ; volume translate right sample
        add     eax, edx                        ; mix left sample
bpatch3:
        add     ebp,12345678h                   ; advance frac pointer
bpatch6:
        movzx   edx, byte [edi+12345678h]   ; get current sample from destination
bpatch4:
        mov     eax, [eax + 12345678h]          ; harsh clip left sample
        add     ebx, edx                        ; mix right sample
        mov     [edi], al                       ; write left sample to destination
bpatch5:
        mov     ebx, [ebx + 12345678h]          ; harsh clip right sample
        mov     edx, ebp                        ; begin calculating second sample
bpatch7:
        mov     [edi+12345678h], bl             ; write right sample to destination
        shr     edx, 16                         ; finish calculation for second sample
bpatch8:
        add     edi, 1                          ; move destination to second sample
        movsx   ebx, byte [esi+2*edx]       ; get second sample
        add     ebx, 80h
        dec     ecx                             ; decrement count
        jnz     mix8Sloop                       ; loop

        mov     [_MV_MixDestination], edi         ; Store the current write position
        mov     [_MV_MixPosition], ebp            ; return position

exit8S:
        popad
        ret


;================
;
; MV_Mix16BitMono16
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

global  _MV_Mix16BitMono16
_MV_Mix16BitMono16:

        pushad
        mov     ebp, [_MV_Position]

        mov     esi, [_MV_Start]                ; Source pointer

        ; Sample size
        mov     ebx, [_MV_SampleSize]
        mov     eax,cpatch4+2            ; convice tasm to modify code...
        mov     [eax],bl
        mov     eax,cpatch5+3            ; convice tasm to modify code...
        mov     [eax],bl

        ; Volume table ptr
        mov     ebx, [_MV_LeftVolume]
        mov     eax,cpatch2+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,cpatch1+4            ; convice tasm to modify code...
        inc     ebx
        mov     [eax],ebx

        ; Rate scale ptr
        mov     edx, [_MV_Rate]
        mov     eax,cpatch3+2            ; convice tasm to modify code...
        mov     [eax],edx

        mov     edi, [_MV_MixDestination]         ; Get the position to write to

        ; Number of samples to mix
        mov     ecx, [_MV_Length]
        cmp     ecx, 0
        je exit16M

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     ecx - count
;     edi - destination
;     esi - source
;     ebp - frac pointer
; cpatch1 - volume table
; cpatch2 - volume table
; cpatch3 - sample rate
; cpatch4 - sample rate

        mov     ebx,ebp                         ; begin calculating first sample
        add     ebp,edx                         ; advance frac pointer
        shr     ebx,16                          ; finish calculation for first sample
        movzx   eax, word [esi+2*ebx]       ; get low byte of sample
        xor     eax, 8000h
        movzx   ebx, ah
        sub     ah, ah

        movsx   edx, word [edi]             ; get current sample from destination

        ALIGN 4
mix16Mloop:
cpatch1:
        movsx   eax, byte [2*eax+12345678h] ; volume translate low byte of sample
cpatch2:
        movsx   ebx, word [2*ebx+12345678h] ; volume translate high byte of sample
        lea     eax, [ eax + ebx + 80h ]        ; mix high byte of sample
        add     eax, edx                        ; mix low byte of sample
cpatch5:
        movsx   edx, word [edi + 2]         ; get current sample from destination

        cmp     eax, -32768                     ; Harsh clip sample
        jge     short m16skip1
        mov     eax, -32768
        jmp     short m16skip2
m16skip1:
        cmp     eax, 32767
        jle     short m16skip2
        mov     eax, 32767
m16skip2:
        mov     ebx, ebp                        ; begin calculating second sample
        mov     [edi], ax                       ; write new sample to destination

        shr     ebx, 16                         ; finish calculation for second sample
cpatch3:
        add     ebp, 12345678h                  ; advance frac pointer

        movzx   eax, word [esi+2*ebx]       ; get second sample
cpatch4:
        add     edi, 2                          ; move destination to second sample
        xor     eax, 8000h
        movzx   ebx, ah
        sub     ah, ah

        dec     ecx                             ; decrement count
        jnz     mix16Mloop                      ; loop

        mov     [_MV_MixDestination], edi         ; Store the current write position
        mov     [_MV_MixPosition], ebp            ; return position
exit16M:
        popad
        ret


;================
;
; MV_Mix16BitStereo16
;
;================

; eax - position
; edx - rate
; ebx - start
; ecx - number of samples to mix

global  _MV_Mix16BitStereo16
_MV_Mix16BitStereo16:

        pushad
        mov     ebp, [_MV_Position]

        mov     esi, [_MV_Start]                ; Source pointer

        ; Sample size
        mov     ebx, [_MV_SampleSize]
        mov     eax,dpatch9+2            ; convice tasm to modify code...
        mov     [eax],bl

        ; Right channel offset
        mov     ebx, [_MV_RightChannelOffset]
        mov     eax,dpatch7+3            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,dpatch8+3            ; convice tasm to modify code...
        mov     [eax],ebx

        ; Volume table ptr
        mov     ebx, [_MV_LeftVolume]
        mov     eax,dpatch1+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,dpatch2+4            ; convice tasm to modify code...
        inc     ebx
        mov     [eax],ebx

        mov     ebx, [_MV_RightVolume]
        mov     eax,dpatch3+4            ; convice tasm to modify code...
        mov     [eax],ebx
        mov     eax,dpatch4+4            ; convice tasm to modify code...
        inc     ebx
        mov     [eax],ebx

        ; Rate scale ptr
        mov     edx, [_MV_Rate]
        mov     eax,dpatch5+2            ; convice tasm to modify code...
        mov     [eax],edx

        ; Source ptr
        mov     esi, [_MV_Start]
        mov     eax,dpatch6+4            ; convice tasm to modify code...
        mov     [eax],esi

        mov     edi, [_MV_MixDestination]         ; Get the position to write to

        ; Number of samples to mix
        mov     ecx, [_MV_Length]
        cmp     ecx, 0
        je      exit16S

;     eax - scratch
;     ebx - scratch
;     edx - scratch
;     esi - scratch
;     ecx - count
;     edi - destination
;     ebp - frac pointer
; dpatch1 - left volume table
; dpatch2 - right volume table
; dpatch3 - sample rate

        mov     ebx,ebp                         ; begin calculating first sample
        shr     ebx,16                          ; finish calculation for first sample

        movzx   edx, word [esi+2*ebx]       ; get first sample
        xor     edx, 8000h                      ; Change from signed to unsigned
        movzx   esi, dh                         ; put high byte in esi
        sub     dh, dh                          ; lo byte in edx

        ALIGN 4
mix16Sloop:
        ; Left channel
dpatch1:
        movsx   eax, word [2*esi+12345678h] ; volume translate high byte of sample
dpatch2:
        movsx   ebx, byte [2*edx+12345678h] ; volume translate low byte of sample
        lea     eax, [ eax + ebx + 80h ]        ; mix high byte of sample

        ; Right channel
dpatch3:
        movsx   esi, word [2*esi+12345678h] ; volume translate high byte of sample
dpatch4:
        movsx   ebx, byte [2*edx+12345678h] ; volume translate low byte of sample
        lea     ebx, [ esi + ebx + 80h ]        ; mix high byte of sample

dpatch7:
        movsx   edx, word [edi+12345678h]   ; get current sample from destination
dpatch5:
        add     ebp,12345678h                   ; advance frac pointer

        add     eax, edx                        ; mix left sample

        cmp     eax, -32768                     ; Harsh clip sample
        jge     short s16skip1
        mov     eax, -32768
        jmp     short s16skip2
s16skip1:
        cmp     eax, 32767
        jle     short s16skip2
        mov     eax, 32767
s16skip2:
        movsx   edx, word [edi+2]           ; get current sample from destination
        mov     [edi], ax                       ; write left sample to destination
        add     ebx, edx                        ; mix right sample

        cmp     ebx, -32768                     ; Harsh clip sample
        jge     short s16skip3
        mov     ebx, -32768
        jmp     short s16skip4
s16skip3:
        cmp     ebx, 32767
        jle     short s16skip4
        mov     ebx, 32767
s16skip4:

        mov     edx, ebp                        ; begin calculating second sample
dpatch8:
        mov     [edi+12345678h], bx             ; write right sample to destination
        shr     edx, 16                         ; finish calculation for second sample
dpatch9:
        add     edi, 4                          ; move destination to second sample

dpatch6:
        movzx   edx, word [2*edx+12345678h] ; get second sample
        xor     edx, 8000h                      ; Change from signed to unsigned
        movzx   esi, dh                         ; put high byte in esi
        sub     dh, dh                          ; lo byte in edx

        dec     ecx                             ; decrement count
        jnz     mix16Sloop                      ; loop

        mov     [_MV_MixDestination], edi         ; Store the current write position
        mov     [_MV_MixPosition], ebp            ; return position
exit16S:
        popad
        ret
