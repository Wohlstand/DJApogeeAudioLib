#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define MULTIVOC_ASM_IMP
#include "_multivc.h"

void ClearBuffer_DW(void *ptr, unsigned data, int length)
{
    int32_t i;

    for (i = 0; i < length; i++)
        ((uint32_t *)ptr)[i] = data;
}


uint32_t MV_Position    __attribute__ ((externally_visible)) = 0;
uint32_t MV_Rate        __attribute__ ((externally_visible)) = 0;
uint8_t *MV_Start       __attribute__ ((externally_visible)) = NULL;
uint32_t MV_Length      __attribute__ ((externally_visible)) = 0;

#define MixFunction(x)   \
void x##_Wrapper(unsigned long position, unsigned long rate, uint8_t *start, unsigned long length) \
{ \
    MV_Position = position; \
    MV_Rate = rate; \
    MV_Start = (uint8_t*)start; \
    MV_Length = length; \
\
    x(); \
}

MixFunction(MV_Mix8BitMono)
MixFunction(MV_Mix8BitStereo)
MixFunction(MV_Mix16BitMono)
MixFunction(MV_Mix16BitStereo)

MixFunction(MV_Mix8BitMono16)
MixFunction(MV_Mix8BitStereo16)
MixFunction(MV_Mix16BitMono16)
MixFunction(MV_Mix16BitStereo16)



uint8_t *MV_Src         __attribute__ ((externally_visible)) = NULL;
uint8_t *MV_Dest        __attribute__ ((externally_visible)) = NULL;
VOLUME16 *MV_Volume     __attribute__ ((externally_visible)) = NULL;
uint32_t MV_Count       __attribute__ ((externally_visible)) = 0;
int32_t MV_Shift        __attribute__ ((externally_visible)) = 0;

void MV_16BitReverb_Wrapper( uint8_t *src, uint8_t *dest, VOLUME16 *volume, int count )
{
    MV_Src = src;
    MV_Dest = dest;
    MV_Volume = volume;
    MV_Count = count;

    MV_16BitReverb();
}

void MV_8BitReverb_Wrapper(uint8_t *src, uint8_t *dest, VOLUME16 *volume, int count )
{
    MV_Src = src;
    MV_Dest = dest;
    MV_Volume = volume;
    MV_Count = count;

    MV_8BitReverb();
}

void MV_16BitReverbFast_Wrapper( uint8_t *src, uint8_t *dest, int count, int shift )
{
    MV_Src = src;
    MV_Dest = dest;
    MV_Count = count;
    MV_Shift = shift;

    MV_16BitReverbFast();
}

void MV_8BitReverbFast_Wrapper(uint8_t *src, uint8_t *dest, int count, int shift )
{
    MV_Src = src;
    MV_Dest = dest;
    MV_Count = count;
    MV_Shift = shift;

    MV_8BitReverbFast();
}
