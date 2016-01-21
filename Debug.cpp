/*
 *  Debug.cpp
 *  Nuvie
 *
 *  Created by Pieter Luteijn on Wed Aug 08 2007.
 *  Copyright (c) 2007. All rights reserved.
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
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <nuvieDefs.h>
//#include "Console.h"
#include <nuvie.h>

#ifndef WITHOUT_DEBUG

// maybe make these configurable at runtime instead? 
#define WITHOUT_DEBUG_TIMESTAMP_IN_HEADER
//#define WITHOUT_DEBUG_LEVEL_IN_HEADER
//#define WITHOUT_DEBUG_FUNC_IN_HEADER
//#define WITHOUT_DEBUG_FILE_LINE_IN_HEADER
//#define WITHOUT_DEBUG_NEWLINE_IN_HEADER

// Redirect stderr/stdout to platform-dependent location (%appdata% on windows)
// Must call SDL_Init() first for this to have any effect in SDL2.
// With SDL2 it only affects things logged with Nuvie's debug() function. (not SDL messages)
void initLogging() {
	const char *STDERR_FN = "\\stdout.txt";
	const char *STDOUT_FN = "\\stderr.txt";
#ifdef WIN32
	const char *configDir = Nuvie::getConfigDirWin32();
	std::string stdout_fn = configDir;
	std::string stderr_fn = configDir;
	std::string infoMsg = "stdout/stderr writing to ";
	infoMsg.append(configDir);
	infoMsg.append("\n");

	stdout_fn.append(STDOUT_FN);
	stderr_fn.append(STDERR_FN);

	DEBUG(1,LEVEL_INFORMATIONAL,infoMsg.c_str()); // write notice to the old file
	freopen(stdout_fn.c_str(), "w", stdout);
	freopen(stderr_fn.c_str(), "w", stderr);
	DEBUG(1,LEVEL_INFORMATIONAL,infoMsg.c_str());
#endif
}

DebugLevelType debug(const char * func, const char * file, const int line, const bool no_header,const DebugLevelType level, const char *format, ...)
{
// original
//  static const char* DebugLevelNames[]= { "EMERGENCY", "ALERT", "CRITICAL", "ERROR", "WARNING", "NOTIFICATION", "INFORMATIONAL", "DEBUGGING" };
//  shorter, because spammy enough as is.
//  static const char* DebugLevelNames[]= { "EMERG", "ALERT", "CRIT.", "ERROR", "WARN.", "NOTE.", "INFO.", "DEBUG" };
//  shorter, because spammy enough as is.
  static const char* DebugLevelNames[]= { "!", "A", "C", "E", "W", "N", "I", "D" };
  static DebugLevelType CurrentDebugLevel=LEVEL_DEBUGGING;
  
  va_list ap;
  time_t now;
  struct tm ts;
  char buf[128]; 
  va_start(ap,format);
  
  if (format==NULL) { 
    CurrentDebugLevel=level;
    return CurrentDebugLevel;
  }
  if (!strcmp(format,"!!increase!!\n")) { 
    unsigned char c=(unsigned char) CurrentDebugLevel;
    if (c<7) {c++;}
    CurrentDebugLevel=(DebugLevelType) c;
  }
  if (!strcmp(format,"!!decrease!!\n")) { 
    unsigned char c=(unsigned char) CurrentDebugLevel;
    if (c>0) {c--;}
    CurrentDebugLevel=(DebugLevelType) c;
  }
  if (level>CurrentDebugLevel) { return CurrentDebugLevel; } // Don't call ourselves here to log something like 'message suppressed'
  if (!no_header) {
    time(&now);
    ts = *localtime(&now);
#ifdef _MSC_VER
	strftime(buf,128,"%Y-%m-%d %H:%M:%S",&ts); 
#else
    strftime(buf,128,"%s",&ts); 
#endif
    fprintf(stderr,"<");
#ifndef WITHOUT_DEBUG_TIMESTAMP_IN_HEADER
    fprintf(stderr,"%s ",buf); 
#endif
#ifndef WITHOUT_DEBUG_LEVEL_IN_HEADER
    fprintf(stderr,"[%s] ",DebugLevelNames[(unsigned char)level]);
#endif
#ifndef WITHOUT_DEBUG_FUNC_IN_HEADER
    fprintf(stderr,"%s ",func);
#endif
#ifndef WITHOUT_DEBUG_FILE_LINE_IN_HEADER
    fprintf(stderr,"%s:%d",file,line);
#endif
    fprintf(stderr,"> ");
#ifndef WITHOUT_DEBUG_NEWLINE_IN_HEADER
    fprintf(stderr,"\n");
#endif
  }
  vfprintf(stderr,format,ap);
  va_end(ap);

  return CurrentDebugLevel;
}

#endif /* WITHOUT_DEBUG */

/* test code / documentation.
int main(char ** argv,int argc)
{
  DEBUG(0,LEVEL_EMERGENCY,NULL); // to set the debug cut-off rather high
  DEBUG(0,LEVEL_EMERGENCY,"%d %c %s\n",1,'a',"aarrgghh..");
  DEBUG(1,LEVEL_EMERGENCY,"continuation of aarrgghh..");
  DEBUG(0,LEVEL_ALERT,"%d %c %s\n",1,'a',"RED"); // should be suppressed
  DEBUG(0,LEVEL_DEBUGGING,NULL); // to allow all messages through.
  DEBUG(0,LEVEL_DEBUGGING,"%d %c %s\n",1,'a',"debugging");
  return 1;
  }
*/
