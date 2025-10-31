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
   module: TIMER.C

   author: James R. Dose
   date:   October 28, 1994

   Demonstrates how to set up multiple timers with TASK_MAN.

   (c) Copyright 1994 James R. Dose.  All Rights Reserved.
**********************************************************************/

#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <stdio.h>
#include "task_man.h"

/*---------------------------------------------------------------------
   Function prototypes
---------------------------------------------------------------------*/

void  TimerFunc( task *Task );

#define TRUE  ( 1 == 1 )
#define FALSE ( 1 != 1 )


/*---------------------------------------------------------------------
   Function: main
---------------------------------------------------------------------*/

int main
   (
   void
   )

   {
   task *Task1 = NULL;
   task *Task2 = NULL;
   task *Task3 = NULL;
   volatile int   Timer1, Timer1p;
   volatile int   Timer2, Timer2p;
   volatile int   Timer3, Timer3p;

   Timer1 = 0;
   Timer2 = 0;
   Timer3 = 0;
   Timer1p = 0;
   Timer2p = 0;
   Timer3p = 0;

   Task1 = TS_ScheduleTask( &TimerFunc, 60, 1, (void*)&Timer1 );
   Task2 = TS_ScheduleTask( &TimerFunc, 129, 1, (void*)&Timer2 );

   TS_Suspend = TRUE;
   printf( "First, Timer 1 is set to 60 ticks per second and Timer 2 is\n"
      "set to 129 ticks per second.\n" );
   fflush( stdout );
   TS_Suspend = FALSE;

   // Start the first two scheduled tasks
   TS_Dispatch();

   // Wait 5 seconds
   while( Timer1 < 300 )
      {
      if(Timer1 != Timer1p || Timer2 != Timer2p || Timer3 != Timer3p)
         {
         TS_Suspend = TRUE;
         printf( "\rTimer 1 = %d\t\tTimer 2 = %d\t\tTimer 3 = %d",
            Timer1, Timer2, Timer3 );
         fflush( stdout );
         Timer1p = Timer1;
         Timer2p = Timer2;
         Timer3p = Timer3;
         TS_Suspend = FALSE;
         }
      }

   TS_Suspend = TRUE;
   printf( "\n\nThen, we start Timer 3 at 849 ticks per second.\n" );
   TS_Suspend = FALSE;

   // Start the third task
   Task3 = TS_ScheduleTask( &TimerFunc, 849, 1, (void*)&Timer3 );
   TS_Dispatch();

   // Wait 5 seconds
   Timer1 = 0;
   while( Timer1 < 300 )
      {
      if(Timer1 != Timer1p || Timer2 != Timer2p || Timer3 != Timer3p)
         {
         TS_Suspend = TRUE;
         printf( "\rTimer 1 = %d\t\tTimer 2 = %d\t\tTimer 3 = %d",
            Timer1, Timer2, Timer3 );
         fflush( stdout );
         Timer1p = Timer1;
         Timer2p = Timer2;
         Timer3p = Timer3;
         TS_Suspend = FALSE;
         }
      }

   TS_Suspend = TRUE;
   printf( "\n\nNow we'll set Timer 2 to run at 1000 ticks per second.\n" );
   TS_Suspend = FALSE;

   // Change the task's rate
   TS_SetTaskRate( Task2, 1000 );

   // Wait 5 seconds
   Timer1 = 0;
   while( Timer1 < 300 )
      {
      if(Timer1 != Timer1p || Timer2 != Timer2p || Timer3 != Timer3p)
         {
         TS_Suspend = TRUE;
         printf( "\rTimer 1 = %d\t\tTimer 2 = %d\t\tTimer 3 = %d",
            Timer1, Timer2, Timer3 );
         fflush( stdout );
         Timer1p = Timer1;
         Timer2p = Timer2;
         Timer3p = Timer3;
         TS_Suspend = FALSE;
         }
      }

   TS_Suspend = TRUE;
   printf("\n\nAnd finally, we'll halt Timer 2, leaving Timers 1 and 3 to "
      "continue.\n" );
   TS_Suspend = FALSE;

   // Stop one of the timers
   TS_Terminate( Task2 );

   // Wait 5 seconds
   Timer1 = 0;
   while( Timer1 < 300 )
      {
      if(Timer1 != Timer1p || Timer2 != Timer2p || Timer3 != Timer3p)
         {
         TS_Suspend = TRUE;
         printf( "\rTimer 1 = %d\t\tTimer 2 = %d\t\tTimer 3 = %d",
            Timer1, Timer2, Timer3 );
         fflush( stdout );
         Timer1p = Timer1;
         Timer2p = Timer2;
         Timer3p = Timer3;
         TS_Suspend = FALSE;
         }
      }

   TS_Suspend = TRUE;
   printf( "\n\nAnd now we're done.\n" );
   fflush( stdout );
   TS_Suspend = FALSE;

   // Terminate the other timers.
   TS_Terminate( Task1 );
   TS_Terminate( Task3 );

   // Shutdown TaskMan.  Failure to do so could be fatal!!!
   TS_Shutdown();
   return 0;
   }

/*---------------------------------------------------------------------
   Function: TimerFunc

   A simple timer example.  Simply increments a timer passed in through
   the data pointer.
---------------------------------------------------------------------*/

void TimerFunc
   (
   task *Task
   )

   {
   ( *( volatile int * )Task->data )++;
   }
