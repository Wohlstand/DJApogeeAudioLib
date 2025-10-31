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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined(__DJGPP__) || defined(__WATCOMC__)
#  define uint32_t   unsigned long
#  define uint8_t    unsigned char
#  define int8_t     signed char
#else
#  include <stdint.h>
#endif

#define true   ( 1 == 1 )
#define false  ( !true )

#define IBK_SIGNATURE    0x1a4b4249
#define BANK_NUMTIMBRES  128

typedef struct
   {
   uint8_t SAVEK[ 2 ];
   uint8_t Level[ 2 ];
   uint8_t Env1[ 2 ];
   uint8_t Env2[ 2 ];
   uint8_t Wave[ 2 ];
   uint8_t Feedback;
   uint8_t unused[ 5 ];
   } TIMBRE;

typedef struct
   {
   uint32_t Signature;
   TIMBRE   Timbre[ BANK_NUMTIMBRES ];
   } BANK;

typedef struct
   {
   uint8_t SAVEK[ 2 ];
   uint8_t Level[ 2 ];
   uint8_t Env1[ 2 ];
   uint8_t Env2[ 2 ];
   uint8_t Wave[ 2 ];
   uint8_t Feedback;
   int8_t  Transpose;
   int8_t  Velocity;
   } NEWTIMBRE;

int Parse( char *filename );
BANK *LoadIBK( char *filename );
int WriteSource( void );

char TMBName[ 200 ];
char HeaderName[ 200 ];
char SourceName[ 200 ];
BANK *IBKPtr = NULL;
int  CreateSource = false;
int  CreateHeader = false;
int  CreateTMB    = false;

NEWTIMBRE Timbres[ 256 ];

int main
   (
   int argc,
   char *argv[]
   )

   {
#if defined(__DJGPP__) || defined(__WATCOMC__)
      printf( "MAKETMB v1.04 by Jim Dos\x82, updated by Vitaliy Novichkov\n" );
#else
      printf( "MAKETMB v1.04 by Jim Dosé, updated by Vitaliy Novichkov\n" );
#endif

   switch( argc )
      {
      case 2 :
         return Parse( argv[ 1 ] );

      default :
         printf( "usage: MAKETMB [input file]\n" );
         return( 1 );
      }
   }

int Parse
   (
   char *fn
   )

   {
   char text[ 200 ];
   char command[ 200 ];
   char filename[ 200 ];
   int  patch;
   int  offset;
   int  transpose;
   int  velocity;
   int  linenum;
   FILE *fp;

   memset( Timbres, 0, sizeof( Timbres ) );

   fp = fopen( fn, "r" );
   if ( fp == NULL )
      {
      perror( fn );
      exit( 1 );
      }

   linenum = 0;
   while( 1 )
      {
      if ( fgets( text, 200, fp ) == NULL )
         {
         break;
         }

      linenum++;

      if ( text[ 0 ] == ';' )
         {
         continue;
         }

      if ( sscanf( text, "%s", command ) != 1 )
         {
         continue;
         }

      if ( strcmp( command, "create" ) == 0 )
         {
         if ( sscanf( text, "%s %s\n", command, TMBName ) != 2 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: create [filename]\n", linenum );
            fclose( fp );
            return( 1 );
            }
         CreateTMB = true;
         }
      else if ( strcmp( command, "source" ) == 0 )
         {
         if ( sscanf( text, "%s %s\n", command, SourceName ) != 2 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: source [filename]\n", linenum );
            fclose( fp );
            return( 1 );
            }
         CreateSource = true;
         }
      else if ( strcmp( command, "header" ) == 0 )
         {
         if ( sscanf( text, "%s %s\n", command, HeaderName ) != 2 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: header [filename]\n", linenum );
            fclose( fp );
            return( 1 );
            }
         CreateHeader = true;
         }
      else if ( strcmp( command, "loadibk" ) == 0 )
         {
         if ( sscanf( text, "%s %s\n", command, filename ) != 2 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: loadibk [filename]\n", linenum );
            fclose( fp );
            return( 1 );
            }

         if ( IBKPtr != NULL )
            {
            free( IBKPtr );
            }
         IBKPtr = LoadIBK( filename );
         }
      else if ( strcmp( command, "patch" ) == 0 )
         {
         if ( sscanf( text, "%s %d %d %d %d\n", command, &patch, &offset,
            &transpose, &velocity ) != 5 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: patch [destination patch] [source patch] [transpose]\n"
               "   [velocity]\n", linenum );
            fclose( fp );
            return( 1 );
            }

         if ( IBKPtr == NULL )
            {
            printf( "No patch set loaded on line %d.\n"
               "Use 'loadibk' to load a patch set.\n", linenum );
            fclose( fp );
            return( 1 );
            }

         patch  -= 1;
         offset -= 1;
         Timbres[ patch ].SAVEK[ 0 ] = IBKPtr->Timbre[ offset ].SAVEK[ 0 ];
         Timbres[ patch ].SAVEK[ 1 ] = IBKPtr->Timbre[ offset ].SAVEK[ 1 ];
         Timbres[ patch ].Level[ 0 ] = IBKPtr->Timbre[ offset ].Level[ 0 ];
         Timbres[ patch ].Level[ 1 ] = IBKPtr->Timbre[ offset ].Level[ 1 ];
         Timbres[ patch ].Env1[ 0 ]  = IBKPtr->Timbre[ offset ].Env1[ 0 ];
         Timbres[ patch ].Env1[ 1 ]  = IBKPtr->Timbre[ offset ].Env1[ 1 ];
         Timbres[ patch ].Env2[ 0 ]  = IBKPtr->Timbre[ offset ].Env2[ 0 ];
         Timbres[ patch ].Env2[ 1 ]  = IBKPtr->Timbre[ offset ].Env2[ 1 ];
         Timbres[ patch ].Wave[ 0 ]  = IBKPtr->Timbre[ offset ].Wave[ 0 ];
         Timbres[ patch ].Wave[ 1 ]  = IBKPtr->Timbre[ offset ].Wave[ 1 ];
         Timbres[ patch ].Feedback   = IBKPtr->Timbre[ offset ].Feedback;
         Timbres[ patch ].Transpose  = transpose;
         Timbres[ patch ].Velocity   = velocity;
         }
      else if ( strcmp( command, "drum" ) == 0 )
         {
         if ( sscanf( text, "%s %d %d %d %d\n", command, &patch, &offset,
            &transpose, &velocity ) != 5 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: drum [key #] [source patch] [pitch] [velocity]\n",
               linenum );
            fclose( fp );
            return( 1 );
            }

         if ( IBKPtr == NULL )
            {
            printf( "No patch set loaded on line %d.\n"
               "Use 'loadibk' to load a patch set.\n", linenum );
            fclose( fp );
            return( 1 );
            }

         patch  += 128;
         offset -= 1;
         Timbres[ patch ].SAVEK[ 0 ] = IBKPtr->Timbre[ offset ].SAVEK[ 0 ];
         Timbres[ patch ].SAVEK[ 1 ] = IBKPtr->Timbre[ offset ].SAVEK[ 1 ];
         Timbres[ patch ].Level[ 0 ] = IBKPtr->Timbre[ offset ].Level[ 0 ];
         Timbres[ patch ].Level[ 1 ] = IBKPtr->Timbre[ offset ].Level[ 1 ];
         Timbres[ patch ].Env1[ 0 ]  = IBKPtr->Timbre[ offset ].Env1[ 0 ];
         Timbres[ patch ].Env1[ 1 ]  = IBKPtr->Timbre[ offset ].Env1[ 1 ];
         Timbres[ patch ].Env2[ 0 ]  = IBKPtr->Timbre[ offset ].Env2[ 0 ];
         Timbres[ patch ].Env2[ 1 ]  = IBKPtr->Timbre[ offset ].Env2[ 1 ];
         Timbres[ patch ].Wave[ 0 ]  = IBKPtr->Timbre[ offset ].Wave[ 0 ];
         Timbres[ patch ].Wave[ 1 ]  = IBKPtr->Timbre[ offset ].Wave[ 1 ];
         Timbres[ patch ].Feedback   = IBKPtr->Timbre[ offset ].Feedback;
         Timbres[ patch ].Transpose  = transpose;
         Timbres[ patch ].Velocity   = velocity;
         }
      else
         {
         printf( "Syntax error on line %d : %s\n", linenum, command );
         fclose( fp );
         return( 1 );
         }
      }

   fclose( fp );

   if ( CreateSource )
      {
      int ret = WriteSource();
      if ( ret != 0)
         return ret;
      }

   if ( CreateTMB )
      {
      fp = fopen( TMBName, "wb" );
      if ( fp == NULL )
         {
         perror( TMBName );
         return( 1 );
         }

      if ( fwrite( Timbres, sizeof( Timbres ), 1, fp ) != 1 )
         {
         perror( TMBName );
         }

      fclose( fp );
      }
   return( 0 );
   }

BANK *LoadIBK
   (
   char *filename
   )

   {
   FILE   *in;
   BANK   *TimbrePtr;

   in = fopen( filename, "rb" );
   if ( in == NULL )
      {
      perror( filename );
      return( NULL );
      }

   TimbrePtr = ( BANK * )malloc( sizeof( BANK ) );
   if ( TimbrePtr == NULL )
      {
      perror( filename );
      fclose( in );
      return( NULL );
      }

   if ( fread( TimbrePtr, sizeof( BANK ), 1, in ) != 1 )
      {
      free( TimbrePtr );
      fclose( in );
      return( NULL );
      }

   fclose( in );

   return( TimbrePtr );
   }

int WriteSource
   (
   void
   )

   {
   FILE *fp;
   int   i;
   const char *struct_prototype =
      "typedef struct\n"
      "   {\n"
      "   unsigned char SAVEK[ 2 ];\n"
      "   unsigned char Level[ 2 ];\n"
      "   unsigned char Env1[ 2 ];\n"
      "   unsigned char Env2[ 2 ];\n"
      "   unsigned char Wave[ 2 ];\n"
      "   unsigned char Feedback;\n"
      "   signed   char Transpose;\n"
      "   signed   char Velocity;\n"
      "   } TIMBRE;\n";

   if ( CreateHeader )
      {
         fp = fopen( HeaderName, "w" );
         if ( fp == NULL )
         {
         perror( SourceName );
         return( 1 );
         }

         fprintf( fp, "%s", struct_prototype);
         fprintf( fp, "\n"
                      "extern TIMBRE ADLIB_TimbreBank[ 256 ];\n\n");
         fclose( fp );
      }

   fp = fopen( SourceName, "w" );
   if ( fp == NULL )
      {
      perror( SourceName );
      return( 1 );
      }

   if ( CreateHeader )
      {
      fprintf( fp, "#include \"%s\"\n", HeaderName);
      }
   else
      {
      fprintf( fp, "%s", struct_prototype);
      }

   fprintf( fp, "\n"
              "TIMBRE ADLIB_TimbreBank[ 256 ] =\n"
              "   {\n");

   for( i = 0; i < 256; i++ )
      {
      fprintf( fp, "      { { %d, %d }, { %d, %d }, { %d, %d }, { %d, %d }, "
              "{ %d, %d }, %d, %d, %d }",
              Timbres[ i ].SAVEK[ 0 ],
              Timbres[ i ].SAVEK[ 1 ],
              Timbres[ i ].Level[ 0 ],
              Timbres[ i ].Level[ 1 ],
              Timbres[ i ].Env1[ 0 ],
              Timbres[ i ].Env1[ 1 ],
              Timbres[ i ].Env2[ 0 ],
              Timbres[ i ].Env2[ 1 ],
              Timbres[ i ].Wave[ 0 ],
              Timbres[ i ].Wave[ 1 ],
              Timbres[ i ].Feedback,
              Timbres[ i ].Transpose,
              Timbres[ i ].Velocity
              );
      if ( i < 255 )
         {
         fprintf( fp, "," );
         }
      fprintf( fp, "\n" );
      }
   fprintf( fp, "   };\n" );

   fclose( fp );
   return 0;
   }
