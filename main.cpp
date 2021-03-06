/*
 *  main.cpp
 *  Nuvie
 *
 *  Created by Eric Fry on Tue Mar 11 2003.
 *  Copyright (c) 2003. All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>
#include <cstdlib>

#include "SDL.h"

#include "nuvieDefs.h"
#include "Console.h"
#include "nuvie.h"

#include "main.h"

#ifdef MACOSX
#include <CoreFoundation/CoreFoundation.h>
#include <XCodeBuild/main.cpp>
int nuvieMain(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
 Nuvie *nuvie;
 DEBUG(0,LEVEL_INFORMATIONAL,"Debugging enabled\n");
 DEBUG(1,LEVEL_DEBUGGING,"To disable debugging altogether, recompile with \"WITHOUT_DEBUG\" defined.\n");
 DEBUG(1,LEVEL_DEBUGGING,"To just get less spam, set the default for CurrentDebugLevel in Debug.cpp lower.\n");
 #ifdef MACOSX
 srandom(time(NULL));


 CFBundleRef bundle = CFBundleGetMainBundle();
 if(bundle != NULL)
 {
	CFURLRef fileUrl = CFBundleCopyResourcesDirectoryURL(bundle);
	if (fileUrl) {
		// Try to convert the URL to an absolute path
		uint8 buf[MAXPATHLEN];
		if (CFURLGetFileSystemRepresentation(fileUrl, true, buf, sizeof(buf))) {
			// Success: Add it to the search path
         DEBUG(0,LEVEL_INFORMATIONAL, "Changing working dir to %s.\n", (const char *)buf);
			chdir((const char *)buf);
		}
		CFRelease(fileUrl);
	}
 }

 #else
 srand(time(NULL));
 #endif

 nuvie = new Nuvie;

 if(nuvie->init(argc, argv) == false)
 {
   ConsolePause();
   delete nuvie;
   return 1;
 }

 nuvie->play();

 delete nuvie;

 return 0;
}
