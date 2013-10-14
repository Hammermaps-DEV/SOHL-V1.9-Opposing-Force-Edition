BSPTwoMap version 1.4b (January 4th, 2004)
by Skyler 'Zipster' York (zipster89134@hotmail.com)
--------------------------------------------

Description
-----------
The purpose of BSPTwoMap is to decompile a Half-Life BSP file into a MAP file viewable in Valve's Hammer editor.  The ultimate goal of this application is to produce something that can be easily recompilable (using Zoner's compile tools) into a working version of the map.  In addition, any textures that are stored in the BSP will be extracted into a custom WAD file created by the application.

Usage
-----
BSPTwoMap is a command-line application, much like the original compile tools.  If you are familiar with these compile tools (HLCSG, HLBSP, etc.), then BSPTwoMap operates in the same way.  If you are not familiar, then to run BSPTwoMap you can either a) drag the BSP file over the EXE file or b) open up the command prompt, go to the directory BSPTwoMap is located, and run the application with the appropiate parameters.  A console window will open showing the progress of the application, and when it is finished a MAP file will be generated in the same directory as the original BSP file, with the same name.

The following are acceptable command-line switches:
-notextures	Do not extract included textures.
-xpcagey	Use when the map was compiled with XP-Cagey's tools and optimizers.
-origin N	Change the dimensions of created origin brushes to NxNxN.
mapname		Name of the BSP to decompile.

Current Version Notes
---------------------
This version now allows you to decompile Blue-Shift maps.  The problem was that Gearbox decided to use a slightly different format for the BSP file header, and that was causing problems.  Also, this version addresses issues with brush-based entities always appearing at the origin of that map.  They are now properly placed in the map, and an origin brush is created automatically for that entity (of size 16x16x16).  This size can be changed using a new command-line switch.

Version 1.3b Notes
---------------------
Some users were having problems with the application crashing when extracting textures, and the most likely cause was large memory allocations.  The WAD extracting code has been optimized to run more efficiently and allocate less memory.

Version 1.2b Notes
---------------------
You know how I said I fixed the issues with decompiling maps optimized using XP-Cagey's tools?  Well, I lied :)  But I've pinpointed the source of the error this time around.  No more problems anymore!  You have to use a new command line switch though when decompiling maps compiled with XP-Cagey's tools.  If you're not sure what the map was compiled with, then try decompiling without the switch and then with it.  Whichever way doesn't give you horrific output (or crash Hammer) is the way to go :)

Version 1.1b Notes
---------------------
Fixed a few things causing problems when decompiling maps optimized with XP-Cagey's tools (http://www.xp-cagey.com).  At least I hope I did.  To be honest, I just added a few more sanity checks in the code that should hopefully keep it from producing bad output.

Version 1.0b Notes
---------------------
At this early beta stage, the output produced by BSPTwoMap could hardly be considered something easily recompilable into BSP format.  As a matter of fact, it's highly unrecommened at this point.  However, the program produces output acceptable for viewing in Valve's Hammer editor, output magnitudes better than that of its predecessors (WinBSP for example).  A few of the recreated brushes may extrude slightly, however this is a known side effect and will hopefully be fixed in the next release or two.  Remember that no map will ever be 100% perfect, so you should always check it over before attempting to recompile, especially with more complex maps.

Currently, BSPTwoMap works well on some maps, and really poorly on other maps.  It worked well on 95% of the maps I tested it with, however on others it created a lot of invalid brushes.  During the early development stages, BSPTwoMap would also get stuck in an infinite loop on a small number of maps, however as I made bug fixes this behavior eventually disappeared altogether.  Yet as a precaution, if you notice BSPTwoMap taking a long time on a certain model, noteably the worldspawn (a long time would be in the range of 20 to 30 seconds), then close the application.  If the map file generated is in the ballpark of 50 megs or greater, then BSPTwoMap is unable to handle your map at this stage in its development.  However this behavior is extremely rare, and I was unable to reproduce it before this release.  For your reference, most MAP files produced will be under 20 megs, or in the range of 20 - 30 megs for larger, more complex maps.  Future versions will also hopefully reduce the filesize as more complex decompile methods are introduced.

Version History
---------------

Version 1.4b
January 4th, 2004	->	Fixed problems decompiling Blue-Shift maps.
				Fixed problems decompiling brush-based entities with origin brushes.
				Added a new command line switch.

Version 1.3b
January 3rd, 2004	->	Improved WAD extracting code.  Allocates less memory and improves efficiency.

Version 1.2b
January 2nd, 2004	->	Fixed issues when decompiling a map compiled with XP-Cagey's plane optimizing tools.
				Added a new command line switch.

Version 1.1b
January 1st, 2004	->	Fixed (hopefully) issues when decompiling a map compiled with XP-Cagey's plane optimizing tools.

Version 1.0b
December 31st, 2003	->	First release