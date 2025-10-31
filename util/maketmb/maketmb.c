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

#define true   ( 1 == 1 )
#define false  ( !true )

#define IBK_SIGNATURE    0x1a4b4249
#define BANK_NUMTIMBRES  128

typedef struct
   {
   unsigned char SAVEK[ 2 ];
   unsigned char Level[ 2 ];
   unsigned char Env1[ 2 ];
   unsigned char Env2[ 2 ];
   unsigned char Wave[ 2 ];
   unsigned char Feedback;
   unsigned char unused[ 5 ];
   } TIMBRE;

typedef struct
   {
   unsigned long Signature;
   TIMBRE        Timbre[ BANK_NUMTIMBRES ];
   } BANK;

typedef struct
   {
   unsigned char SAVEK[ 2 ];
   unsigned char Level[ 2 ];
   unsigned char Env1[ 2 ];
   unsigned char Env2[ 2 ];
   unsigned char Wave[ 2 ];
   unsigned char Feedback;
   signed   char Transpose;
   signed   char Velocity;
   } NEWTIMBRE;

void Parse( char *filename );
BANK *LoadIBK( char *filename );
void WriteSource( void );

char TMBName[ 80 ];
char SourceName[ 80 ];
BANK *IBKPtr = NULL;
int  CreateSource = false;
int  CreateTMB    = false;

NEWTIMBRE Timbres[ 256 ];

void main
   (
   int argc,
   char *argv[]
   )

   {
   printf( "MAKETMB v1.02 by Jim Dos‚\n" );

   switch( argc )
      {
      case 2 :
         Parse( argv[ 1 ] );
         break;

      default :
         printf( "usage: MAKETMB [input file]\n" );
         exit( 1 );
      }
   }

void Parse
   (
   char *fn
   )

   {
   char text[ 80 ];
   char command[ 80 ];
   char filename[ 80 ];
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
      if ( fgets( text, 80, fp ) == NULL )
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
            exit( 1 );
            }
         CreateTMB = true;
         }
      else if ( strcmp( command, "source" ) == 0 )
         {
         if ( sscanf( text, "%s %s\n", command, SourceName ) != 2 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: source [filename]\n", linenum );
            exit( 1 );
            }
         CreateSource = true;
         }
      else if ( strcmp( command, "loadibk" ) == 0 )
         {
         if ( sscanf( text, "%s %s\n", command, filename ) != 2 )
            {
            printf( "Syntax error on line %d\n"
               "Syntax should be: loadibk [filename]\n", linenum );
            exit( 1 );
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
               "Syntax should be: patch [destination patch] [source patch] [transpose]\n",
               "   [velocity]\n", linenum );
            exit( 1 );
            }

         if ( IBKPtr == NULL )
            {
            printf( "No patch set loaded on line %d.\n"
               "Use 'loadibk' to load a patch set.\n", linenum );
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
            exit( 1 );
            }

         if ( IBKPtr == NULL )
            {
            printf( "No patch set loaded on line %d.\n"
               "Use 'loadibk' to load a patch set.\n", linenum );
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
         exit( 1 );
         }
      }

   fclose( fp );

   if ( CreateSource )
      {
      WriteSource();
      }

   if ( CreateTMB )
      {
      fp = fopen( TMBName, "wb" );
      if ( fp == NULL )
         {
         perror( TMBName );
         exit( 1 );
         }

      if ( fwrite( Timbres, sizeof( Timbres ), 1, fp ) != 1 )
         {
         perror( TMBName );
         }

      fclose( fp );
      }
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

void WriteSource
   (
   void
   )

   {
   FILE *fp;
   int   i;

   fp = fopen( SourceName, "w" );
   if ( fp == NULL )
      {
      perror( SourceName );
      exit( 1 );
      }

   fprintf( fp, "typedef struct\n"
           "   {\n"
           "   unsigned char SAVEK[ 2 ];\n"
           "   unsigned char Level[ 2 ];\n"
           "   unsigned char Env1[ 2 ];\n"
           "   unsigned char Env2[ 2 ];\n"
           "   unsigned char Wave[ 2 ];\n"
           "   unsigned char Feedback;\n"
           "   signed   char Transpose;\n"
           "   signed   char Velocity;\n"
           "   } TIMBRE;\n"
           "\n"
           "TIMBRE ADLIB_TimbreBank[ 256 ] =\n"
           "   {\n" );

   for( i = 0; i < 256; i++ )
      {
      fprintf( fp, "      { { %d, %d }, { %d, %d }, { %d, %d }, { %d, %d }, "
              "{ %d, %d }, %d, %d }",
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
              Timbres[ i ].Transpose
              );
      if ( i < 255 )
         {
         fprintf( fp, "," );
         }
      fprintf( fp, "\n" );
      }
   fprintf( fp, "   };\n" );

   fclose( fp );
   }
