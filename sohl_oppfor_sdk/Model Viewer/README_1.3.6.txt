[Jed's Half-Life Model Viewer 1.3.5]
==========================================================================
(c) 2005 Neil "Jed" Jedrzejewski (jed@wunderboy.org) http:///www.wunderboy.org

[Info]
======

This is a custom build of the Mete's Ciragan's Half-Life Model Viewer
with the addition of several new features. Its has been through two
public beta's and is fairly bug free but some may still exist.

I, Neil Jedrzejewski, assume no responsibility whatsoever for its use
by other parties, and makes no guarantees, expressed or implied, about
its quality, reliability, or any other characteristic.

[Installation]
==============

Well if you can read this is probably installed!

[Added Features over HLMV 1.25]
===============================

* GUI re-organised.
* Default background colour is now blue/grey instead of black.
* "Save Options" allows you to save your preferences so they are loaded
  next time you start HLMV.
* Dump Model Info option creates a text file giving a detailed
  breakdown of your models internal structure.
* Acts as a front-end to STUDIOMDL and MDLDEC allowing you to 
  decompile and compile models from within HLMV.
* Can open GCF, PAK, WAD and BSP files.

* Poly counter shows how many polygons are being drawn.

* Supports the Alpha and Additive texture flags.
* Displays transparent and additive textures in the model preview.
* Texture flags can be set and previewed and saved to the model.
* Option to view the UV Map for any given texture.

* Sequence panel now shows additional sequence parameters.
* Events for a given sequnce can be viewed.

* Previewing of new weapon origins for v_model's (tweaked this version).
* Addition of crosshair and guides for helping with re-origining weapons

* Small bugs from HLMV 1.25 fixed.

[Known Bugs]
============

* 3D view doesnt move or respond to keyboard commands occasionally.
  Just click once inside the 3D window to un-stick it.

* Scale Mesh/Bones can be a little funky at times.

* Loading a model while in Weapon Origin view can make the model get
  a little lost when switching tabs.

[Changes]
=========

1.3.6	Added Windows Vista Fixes
	Updated HLLib to version 1.1.8.0

1.3.5	Added Windows XP themes support.
	Updated HLLib to version 1.1.6.0.
	Fixed minor memory leak.
	Fixed errant message box about changed models.

1.3	Replaced PAK viewer with a generic Half-Life Package Viewer.
	Can now open GCF, BSP, WAD and PAK files.
	Removed the "Black Alpha Option"
	Removed the File Associations. Package viewer now opens files
	with the currently registered application for that type.
	Removed dependancy on DevIL library.

1.2	Added support for opening PS2 format Half-Life Models
	Added Export UV Map as BMP function
	Added support for playing back even sounds with animation
	Added "Save view" and "Recall view" options for rembering viewport positions
	Added "Black Alpha", turns transparent texture parts black to stop blue edges
	Can now use GUIStudioMDL as a compiler.
	Fixed erroneous event display
	Other stuff I cant remember... :-/

1.1	Added "Wireframe Overlay" option
	Added "View Eye Position" option
	Added Polygon counter to the sub-models menu
	Added sequence controls to Weapon Origin view
	Added a few "user friendly" reminders to some tabs
	Small tweaks to stop textures in "Texture View" blurring
	Sexy new icon!
	Proper installer/Uninstaller


1.0	Fixed config loading errors
	Expanded Model Dump info
	Added support for decompiling/compiling
	Re-organised GUI
	Added UW Map display option
	Improved Events display for Sequences
	Adjusted Weapon Origin view for accuracy
	Various bug fixes


BETA VERSIONS

1.1B	Re-wrote rendering code
	Fixed "X-Ray" additive textures (needs testing)
	Improved transparency support to be more like Half-Life's
	Added Day Of Defeat first-person weapon origin editing*

	* Only on the DoD version

1.0B	Added support for transparent and additive texture modes
	Added support for setting of texture flags and previewing
	Added listing of events associated with a sequence
	Added additional sequence info
	Added dump model info option
	Added Save Options command
	Changed default background colour to grey

[F.A.Q]
=======

General

Q) Where has the "Save Model" button gone?
A) Its been moved to the "File" menu at the top of the screen.

Q) I've moved my model and now I can't find it. How do I centre it?
A) Use the "Center View" option under the "Options" menu.

Q) Why do my textures look blurry?
A) HLMV has to scale all textures up to be a power-of-2. This is a requirement
   of most graphic card hardware. Because of the scaling, some blurring may 
   occur. Textures that are originally power-of-2 will blur less. Also running HLMV
   in a 16-bit colour mode will cause the colours to appear less than optimal.

Q) How do I use the decompiler/compiler option?
A) First grab a copy of STUDIOMDL and MDLDEC relevant for your mod/game. A good
   place to look is at: http://collective.valve-erc.com/
   Next, use "Configure Tools" to tell HLMV where these are. Then its as simple
   as choosing Decompile or Compile and pointing it to you MDL or QC file.
   You can also use GUISTUDIOMDL as your compiler.

Q) I've decompiled my model and re-compiled it but some of my textures are no
   longer chrome and additive.
A) MDLDEC and STUDIOMDL dont seem to be able to handle textures with multiple
   rendermode flags sometimes. If this happens, re-set your texture flags 
   within HLMV and save your model from the "File Menu".

Q) How do I use the GCF viewer?
A) Goto File->Open Half-Life Package and from the drop down list select the
   type of package you want to open. When that is done a tree view of the 
   package will appear on the right.

Q) How do I load/extract stuff from the GCF file?
A) Click on the item you want to extract then right click and choose extract
   and choose a location to extract to. If the file is a MDL, texture and 
   sequence files will be extracted with it.


Textures Tab

Q) Why is the Texture Scale now fixed sizes?
A) To make the scaling more linear and to help with people who may be screen
   grabbing UV Maps or textures to scale them.

Q) Why cant I select Transparent and Chrome for a texture?
A) Although technically in a model you could set those flags, experimenting has
   shown that the effect varies from game to game and is hard to control. Hence
   HLMV is set to allow the most common working combinations only.

Q) I get an error when importing a texture...
A) Replacement textures must be the same size as the original, 256 colour and 
   NOT RLE compressed.

Q) I changed a texture/texture flags but it didnt stay in the model.
A) After you import a new texture or change flags, you need to save the model
   using the "Save Model" command under the "File Menu"

Q) In my BMP texture my transparent areas are red but in HLMV they show as blue
   or black.
A) Internally, HLMV replaces whatever your transparent palette colour was with
   black to reduce any edging around the transparent textures when displayed.
   Inside your model, the colour is still blue/red/whatever but the texture in
   your graphic card has be changed only. The transparent parts will go black
   if you had a transparent texture and then turn transparency off. Don't worry
   its just a display thing, its not altering your texture data.

Q) How do I save the UV Map?
A) At present, you cant export the UV Map directly but you can use the
   "Make Screenshot" option or do a screen capture with Alt+PrintScrn and open
   it in an editing program and crop as needed.

Q) My textures have a black edge around them.
A) If you decompile and recompile the same model over and over again the UV Map
   co-ordinates will start to "creep" due to rounding in the math during the
   normalisation process. To avoid this decompile a model ONLY once and 
   re-compile from the same QC again rather than de-compiling every time you want
   to make a change to your model.



Weapons Origin

Q) How do I save my new origin's into my model?
A) Decompile your v_model and add and $origin line into your QC file and re-compile.
   A tutorial on doing this using HLMV can be found at:
   http://homepage.ntlworld.com/scitzo/hlmvtut.htm

Q) How accurate is the origin view?
A) As accurate as we could get it. Lets just say Marzy the fearless Beta tester 
   spent weeks on checking alignments. Its not 100% but its so close its almost 
   un-noticable.

Q) Why can't I move the model in origin view?
A) The view is locked to stop you accidentaly nudging the weapon and getting bad
   alignments.