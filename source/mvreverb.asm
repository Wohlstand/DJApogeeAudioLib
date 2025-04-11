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
        ; IDEAL

        ; p386
        ; MODEL  flat

        ; dataseg
        ; CODESEG

        ; MASM
        ; ALIGN 4

extern   _MV_Src
extern   _MV_Dest
extern   _MV_Volume
extern   _MV_Count
extern   _MV_Shift


%ifidn __OUTPUT_FORMAT__, coff
section .text public class=CODE USE32
%elifidn __OUTPUT_FORMAT__, obj
section _TEXT public class=CODE USE32
%endif

align 4

;================
;
; MV_16BitReverb
;
;================

; eax - source position
; edx - destination position
; ebx - Volume table
; ecx - number of samples

global _MV_16BitReverb
_MV_16BitReverb:

        mov     esi, [_MV_Src]
        mov     edx, [_MV_Dest]
        lea     edi, [edx - 2]

        mov     ecx, [_MV_Count]

        ALIGN 4
rev16loop:
        movzx   eax, word [esi]             ; get sample
        add     edi, 2

        movzx   edx, ah
        sub     ah, ah

        mov     ebx, [_MV_Volume]
        movsx   eax, byte [2*eax+ebx+1]     ; volume translate low byte of sample
        xor     edx, 80h

        movsx   edx, word [2*edx+ebx]       ; volume translate high byte of sample
        add     esi, 2

        lea     eax, [ eax + edx + 80h ]        ; mix high byte of sample
        dec     ecx                             ; decrement count

        mov     [edi], ax                       ; write new sample to destination
        jnz     rev16loop                       ; loop

        ret


;================
;
; MV_8BitReverb
;
;================

; eax - source position
; edx - destination position
; ebx - Volume table
; ecx - number of samples

global _MV_8BitReverb
_MV_8BitReverb:

        mov     esi, [_MV_Src]
        mov     edx, [_MV_Dest]
        lea     edi, [edx - 1]

        xor     eax, eax

        mov     ecx, [_MV_Count]

        ALIGN 4
rev8loop:
;        movzx   eax, byte [esi]             ; get sample
        mov     al, byte [esi]              ; get sample
        inc     edi

        mov     ebx, [_MV_Volume]
;        movsx   eax, byte [2*eax+ebx]       ; volume translate sample
        mov     al, byte [2*eax+ebx]        ; volume translate sample
        inc     esi

;        add     eax, 80h
        add     al, 80h
        dec     ecx                             ; decrement count

        mov     [edi], al                       ; write new sample to destination
        jnz     rev8loop                        ; loop

        ret


;================
;
; MV_16BitReverbFast
;
;================

; eax - source position
; edx - destination position
; ebx - number of samples
; ecx - shift

global _MV_16BitReverbFast:
_MV_16BitReverbFast:

        mov     esi, [_MV_Src]
        mov     eax,rpatch16+3

        mov     [eax],cl
        mov     edx, [_MV_Dest]
        lea     edi, [edx - 2]

        mov     ebx, [_MV_Count]
        mov     ecx, [_MV_Shift]

        ALIGN 4
frev16loop:
        mov     ax, word [esi]             ; get sample
        add     edi, 2

rpatch16:
        sar     ax, 5    ;;;;Add 1 before shift
        add     esi, 2

        mov     [edi], ax                       ; write new sample to destination
        dec     ebx                             ; decrement count

        jnz     frev16loop                      ; loop

        ret


;================
;
; MV_8BitReverbFast
;
;================

; eax - source position
; edx - destination position
; ebx - number of samples
; ecx - shift

global _MV_8BitReverbFast
_MV_8BitReverbFast:
        mov     esi, [_MV_Src]
        mov     eax,rpatch8+2

        mov     ebx, [_MV_Count]
        mov     ecx, [_MV_Shift]

        mov     edi, [_MV_Dest]
        mov     edx, 80h

        mov     [eax],cl
        mov     eax, 80h

        shr     eax, cl

        dec     edi
        sub     edx, eax

        ALIGN 4
frev8loop:
        mov     al, byte [esi]             ; get sample
        inc     esi

        mov     ecx, eax
        inc     edi

rpatch8:
        shr     eax, 3
        xor     ecx, 80h                        ; flip the sign bit

        shr     ecx, 7                          ; shift the sign down to 1
        add     eax, edx

        add     eax, ecx                        ; add sign bit to round to 0
        dec     ebx                             ; decrement count

        mov     [edi], al                       ; write new sample to destination
        jnz     frev8loop                       ; loop

        ret
