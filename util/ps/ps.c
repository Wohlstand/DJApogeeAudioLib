/*
Copyright (C) 1994-1995 Apogee Software, Ltd.

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
/**********************************************************************
   module: PS.C

   author: James R. Dose
   date:   December 12, 1995

   Simple sound player.

   (c) Copyright 1995 James R. Dose.  All Rights Reserved.
**********************************************************************/

#include <conio.h>
#include <dos.h>
#include <sys/nearptr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "fx_man.h"


/*---------------------------------------------------------------------
   Function prototypes
---------------------------------------------------------------------*/

uint8_t *LoadFile( char *filename, int *length );
char *GetUserText(const char *parameter , int _argc, char **_argv);
int   CheckUserParm( const char *parameter, int _argc, char **_argv );
void  DefaultExtension( char *path, char *extension );

#define TRUE  ( 1 == 1 )
#define FALSE ( !TRUE )

#define NUMCARDS 8

char *SoundCardNames[] =
   {
   "Sound Blaster", "Awe 32", "Pro AudioSpectrum",
   "Sound Man 16", "Ensoniq SoundScape", "Gravis UltraSound",
   "Disney SoundSource", "Tandy SoundSource"
   };

int SoundCardNums[] =
   {
   SoundBlaster, Awe32, ProAudioSpectrum, SoundMan16,
   SoundScape, UltraSound, SoundSource, TandySoundSource
   };

extern void  USER_InitArgs(int argc, char **argv);

/*---------------------------------------------------------------------
   Function: main

   Sets up sound cards, calls the demo, and then cleans up.
---------------------------------------------------------------------*/

int main
   (
   int argc,
   char *argv[]
   )

   {
   int card;
   int voices;
   int rate;
   int bits;
   int channels;
   int reverb;
   int status;
   int length;
   int voice;
   fx_device device;
   fx_blaster_config blaster;
   uint8_t *SoundPtr = NULL;
   char *ptr;
   char  filename[ 128 ];
   char ch;

   USER_InitArgs(argc, argv);

   printf( "\nPS Version 1.0 by Jim Dose\n" );

   if ( ( CheckUserParm( "?", argc, argv ) ) || ( argc < 2 ) )
      {
      int index;

      printf(
         "Usage: PS [ soundfile ] CARD=[ card # ] VOICES=[ # of voices ]\n"
         "          BITS=[ 8 or 16 ] [MONO/STEREO] RATE=[ mixing rate ]\n"
         "          REVERB=[ reverb amount]\n\n"
         "   sound card # = \n" );
      for( index = 0; index < NUMCARDS; index++ )
         {
         printf( "      %d : %s\n", index, SoundCardNames[ index ] );
         }

      printf( "\nDefault: PS [ soundfile ] CARD=0 VOICES=4 "
                "BITS=8 MONO RATE=11000\n\n" );
      return 0;
      }

   __djgpp_nearptr_enable();

   // Default is Sound Blaster
   card     = 0;
   voices   = 4;
   bits     = 8;
   channels = 1;
   reverb   = 0;
   rate     = 11000;

   FX_GetBlasterSettings( &blaster );
   printf("Card setup: A=0x%lX, I=%ld, D8=%ld, D16=%ld\n", blaster.Address, blaster.Interrupt, blaster.Dma8, blaster.Dma16);
   FX_SetupSoundBlaster(blaster, &device.MaxVoices, &device.MaxSampleBits, &device.MaxChannels);

   ptr = GetUserText( "VOICES", argc, argv );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &voices );
      }

   ptr = GetUserText( "BITS", argc, argv );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &bits );
      }

   ptr = GetUserText( "RATE", argc, argv );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &rate );
      }

   ptr = GetUserText( "REVERB", argc, argv );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &reverb );
      }

   ptr = GetUserText( "MONO", argc, argv );
   if ( ptr != NULL )
      {
      channels = 1;
      }

   ptr = GetUserText( "STEREO", argc, argv );
   if ( ptr != NULL )
      {
      channels = 2;
      }

   ptr = GetUserText( "CARD", argc, argv );
   if ( ptr != NULL )
      {
      sscanf( ptr, "%d", &card );
      }

   if ( ( card < 0 ) || ( card >= NUMCARDS ) )
      {
      printf( "Value out of range for sound card #: %d\n", card );
      return 1;
      }

   strcpy( filename, argv[ 1 ] );
   DefaultExtension( filename, ".wav" );
   SoundPtr = LoadFile( filename, &length );
   if ( !SoundPtr )
      {
      strcpy( filename, argv[ 1 ] );
      DefaultExtension( filename, ".voc" );
      SoundPtr = LoadFile( filename, &length );
      if ( !SoundPtr )
         {
         strcpy( filename, argv[ 1 ] );
         SoundPtr = LoadFile( filename, &length );
         if ( !SoundPtr )
            {
            printf( "Cannot open '%s' for read.\n", argv[ 1 ] );
            return 1;
            }
         }
      }

   printf("Init card...\n");
   status = FX_SetupCard( SoundCardNums[ card ], &device );
   if ( status != FX_Ok )
      {
      printf( "%s\n", FX_ErrorString( status ) );
      return 1;
      }

   printf("Init FX... (C=%d [%d=%s], V=%d, C=%d, B=%d, R=%d)\n", card, SoundCardNums[ card ], SoundCardNames[ card ], voices, channels, bits, rate);
   status = FX_Init( SoundCardNums[ card ], voices, channels, bits, rate );
   if ( status != FX_Ok )
      {
      printf( "%s\n", FX_ErrorString( status ) );
      return 1;
      }

   printf("Set reverb...\n");
   FX_SetReverb( reverb );

   printf("Set volume...\n");
   FX_SetVolume( 255 );

   printf( "Playing file '%s'.\n\n", filename );
   printf( "Press any key to play the sound.\n"
           "Press ESCape to end.\n\n" );

   ch = 0;
   while( ch != 27 )
      {
      if ( stricmp( &filename[ strlen( filename ) - 3 ], "WAV" ) == 0 )
         {
         voice = FX_PlayWAV( SoundPtr, 0, 255, 255, 255, 0, 0 );
         }
      else if ( stricmp( &filename[ strlen( filename ) - 3 ], "VOC" ) == 0 )
         {
         voice = FX_PlayVOC( SoundPtr, 0, 255, 255, 255, 0, 0 );
         }
      else
         {
         voice = FX_PlayRaw( SoundPtr, length, rate, 0, 255, 255, 255, 0, 0 );
         }

      if ( voice < FX_Ok )
         {
         printf( "Sound error - %s\n", FX_ErrorString( status ) );
         }

      ch = getch();
      }

   FX_StopAllSounds();
   free( SoundPtr );
   FX_Shutdown();

   printf( "\n" );
   return 0;
   }


/*---------------------------------------------------------------------
   Function: LoadFile

   Loads a file from disk.
---------------------------------------------------------------------*/

uint8_t *LoadFile(
   char *filename,
   int  *length
   )

   {
   FILE   *in;
   long   size;
   uint8_t *ptr;

   if ( ( in = fopen( filename, "rb" ) ) == NULL )
      {
      return NULL;
      }

   fseek( in, 0, SEEK_END );
   size = ftell( in );
   fseek( in, 0, SEEK_SET );

   ptr = ( uint8_t * )malloc( size );
   if ( ptr == NULL )
      {
      printf( "Out of memory while reading '%s'.\n", filename );
      exit( 1 );
      }

   if ( fread( ptr, size, 1, in ) != 1 )
      {
      printf( "Unexpected end of file while reading '%s'.\n", filename );
      exit(1);
      }

   fclose( in );

   *length = size;

   return( ptr );
   }


/*---------------------------------------------------------------------
   Function: GetUserText

   Checks if the specified string is present in the command line
   and returns a pointer to the text following it.
---------------------------------------------------------------------*/

char *GetUserText
   (
   const char *parameter, int _argc, char **_argv
   )

   {
   int i;
   int length;
   char *text;
   char *ptr;

   // extern int   _argc;
   // extern char **_argv;

   text = NULL;
   length = strlen( parameter );
   i = 1;
   while( i < _argc )
      {
      ptr = _argv[ i ];

      if ( ( strnicmp( parameter, ptr, length ) == 0 ) &&
         ( *( ptr + length ) == '=' ) )
         {
         text = ptr + length + 1;
         break;
         }

      i++;
      }

   return( text );
   }


/*---------------------------------------------------------------------
   Function: CheckUserParm

   Checks if the specified string is present in the command line.
---------------------------------------------------------------------*/

int CheckUserParm
(
const char *parameter
, int _argc, char **_argv)

   {
   int i;
   int found;
   char *ptr;

   // extern int   _argc;
   // extern char **_argv;

   found = FALSE;
   i = 1;
   while( i < _argc )
      {
      ptr = _argv[ i ];

      // Only check parameters preceded by - or /
      if ( ( *ptr == '-' ) || ( *ptr == '/' ) )
         {
         ptr++;
         if ( stricmp( parameter, ptr ) == 0 )
            {
            found = TRUE;
            break;
            }
         }

      i++;
      }

   return( found );
   }


/*---------------------------------------------------------------------
   Function: DefaultExtension

   Checks if the specified filename contains an extension and adds
   one if it doesn't.
---------------------------------------------------------------------*/

void DefaultExtension
   (
   char *path,
   char *extension
   )

   {
   char  *src;

   //
   // if path doesn't have a .EXT, append extension
   // (extension should include the .)
   //
   src = path + strlen( path ) - 1;

   while( ( *src != '\\' ) && ( src != path ) )
      {
      if ( *src == '.' )
         {
         // it has an extension
         return;
         }
      src--;
      }

   strcat( path, extension );
   }
