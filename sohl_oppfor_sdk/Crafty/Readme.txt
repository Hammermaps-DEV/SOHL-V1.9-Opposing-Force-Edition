=================================
=================================
 CRAFTY BY: RYAN "NEMESIS" GREGG
=================================
=================================

===========================
Program/Author Information:
===========================

---- General Program Information ----
Date                     : September 15th, 2007
Author                   : Ryan "NEMESIS" Gregg
Title                    : Crafty
Build                    : 1.0.0 Alpha 14
Email address            : ryansgregg@hotmail.com
Home page /  Website     : http://nemesis.thewavelength.net/

---- Program Construction Information ----
Written In               : C++ .NET

=============
Requirements:
=============

  OS
  - Microsoft Windows XP SP2
  - Microsoft Windows 2000 SP3
  - Microsoft Windows ME
  - Microsoft Windows 98
  - Microsoft Windows NT
  - Microsoft Windows Server 2003

  Frameworks
  - Microsoft .NET Framework v2.0

  CPU
  - 1.4 GHz Intel Pentium 4 Class Processor or Equivalent (2.0 GHz Recommended)

  RAM
  - 512 MB of RAM (1 GB Recommended)

====
FAQ:
====

Q.
  When I launch Crafty I get the following error message:
  "This application has failed to start because the application configuration
  is incorrect. Reinstalling the application may fix this problem."

A.
  Crafty is written in C++ .NET v2.0 and such as requires the .NET runtimes to
  run. See the following link for more information:
  http://www.microsoft.com/downloads/details.aspx?FamilyID=0856EACB-4362-4B0D-8EDD-AAB15C5E04F5&displaylang=en

Q.
  When I launch Crafty I get the following error message:
  "The application failed to initialize properly (0xc0000135). Click ok
  to terminate the application."

A.
  Crafty is written in C++ .NET v2.0 and such as requires the .NET runtimes to
  run. See the following link for more information:
  http://www.microsoft.com/downloads/details.aspx?FamilyID=0856EACB-4362-4B0D-8EDD-AAB15C5E04F5&displaylang=en

Q.

  When I launch Crafty I get the following error message:
  "A required .DLL file, MSCOREE.DLL, was not found."

A.
  Crafty is written in C++ .NET v2.0 and such as requires the .NET runtimes to
  run. See the following link for more information:
  http://www.microsoft.com/downloads/details.aspx?FamilyID=0856EACB-4362-4B0D-8EDD-AAB15C5E04F5&displaylang=en

==================
Program Changelog:
==================

  v1.0.0 Alpha 14
  - Added Team Fortress 2 VBSP support.
  - Added OBJ buffered reading.
  - Updated to HLLib v2.0.7.
  - Updated to VTFLib v1.2.6.

  v1.0.0 Alpha 13
  - Added dynamic VBSP lighting.
  - Added VMT Cloud shader support.
  - Added VBSP and VMF fog.
  - Added basic Half-Life 1 MDL exportation.
  - Added basic OBJ MTL read support.
  - Added form location and size saving.
  - Added validation support to File System Browser.
  - Added time estimate to File System Browser.
  - Added pausing to File System Browser.
  - Added verbose mode to File System Browser.
  - Added find next to Console.
  - Added sprite font support.
  - Added drag n' drop functionality to Object Viewer.
  - Improved VBSP lightmap generation.
  - Improved render loop.
  - Improved File System Browser tree managment.
  - Improved primitive rendering performance.
  - Improved material proxy management.
  - Fixed VBSP PVS decompression bug.
  - Fixed OBJ indexing bug.
  - Updated to HLLib v2.0.6.

  v1.0.0 Alpha 12
  - Added Half-Life 2 3D skybox support.
  - Added Half-Life 1 BSP exportation.
  - Fixed crash on level change.
  - Fixed crash on VBSP lightmap export.

  v1.0.0 Alpha 11
  - Added PRT support.
  - Added PTS support.
  - Added basic MAP writing.
  - Added basic VMF writing.
  - Added VBSP lightmap exportation.
  - Added VMF support for comments.
  - Added GL exportation.
  - Added GL support for new-line delimiters.
  - Added object overlay support.
  - Fixed overflow in File System Browser when extracting more than 2 GB of data.
  - Fixed normal transformations in OBJ export code.
  - Improved nested package reading.
  - Updated to HLLib v2.0.1.

  v1.0.0 Alpha 10
  - Added drag and drop support to File System Browser.
  - Fixed Half-Life BSP texture bug which prevented BSP files compiled with different sized textures from loading.
  - Fixed File System Browser, Material Browser and Model Browser shell execution code.

  v1.0.0 Alpha 9
  - Added X-Ray render mode.
  - Added VMT fallback shader support.
  - Added VMT texture scrolling.
  - Added VMT UnlitTwoTexture shader support.
  - Added -convert switch for command line object conversion.
  - Added Half-Life: Blue Shift BSP support.
  - Added Half-Life 2 MDL skin support.
  - Added Half-Life 2 MDL LOD selection.
  - Added Half-Life 2 BSP entity occlusion culling.
  - Added simple automatic configuration.
  - Added camera options to settings.
  - Optimized Half-Life 2 MDL rendering.
  - Optimized primitive rendering.
  - Fixed Half-Life and Half-Life 2 BSP frustum culling option.
  - Fixed a few minor memory leaks.

  v1.0.0 Alpha 8
  - Added animated texture support.
  - Added Half-Life 2 BSP exportation.
  - Added active profile to status bar.
  - Fixed a bug which caused the 3D camera to disable.

  v1.0.0 Alpha 7
  - Added basic OBJ support.
  - Added Half-Life MAP and RMF exportation.
  - Added Half-Life 2 MDL and VMF exportation.
  - Added cull freezing.
  - Added model detail option to settings.
  - Added default skyboxes to Half-Life 2 BSP files and VMF files.
  - Fixed Half-Life 2 model meshes.
  - Fixed Half-Life 2 model LOD rendering.
  - Fixed Half-Life BSP skyboxes.
  - Fixed several threading related render option bugs.

  v1.0.0 Alpha 6
  - Added game profiles.
  - Added basic ambient lighting to Half-Life 2 BSP models.
  - Added Half-Life 2 BSP HDR settings.
  - Added automatic WAD loading to Half-Life BSP files.
  - Added entity rendermode support to Half-Life BSP files.
  - Added Half-Life BSP gamma correction setting.
  - Added find dialog to console.
  - Improved file system referencing system (now works like Steam does).
  - Improved Material Browser and Model Browser reference trees.
  - Fixed Half-Life 2 BSP occlusion culling.
  - Fixed File System Browser color coding.
  - Fixed file system crashing when vbsp.zip is prematurely unmounted.

  v1.0.0 Alpha 5
  - Added skybox support to Half-Life BSP files.
  - Added generic model support to Model Browser.
  - Added color coding to File System Browser.
  - Improved missing texture warning messages.

  v1.0.0 Alpha 4
  - Added basic Half-Life BSP support.
  - Added basic Half-Life MDL support.
  - Added dynamic configuration file support.
  - Added File System Browser watcher.
  - Added model fading.
  - Added model LOD multiplier.
  - Added initial camera positioning based on player starts.
  - Improved Object Viewer loading code.
  - Improved material system.
  - Fixed Model Browser not showing after two uses.
  - Fixed model frustum culling clipping bug (again).
  - Fixed Half-Life 2 MDL material search directories.

  v1.0.0 Alpha 3
  - Added displacement map lightmapping.
  - Added displacement map occlusion culling.
  - Added lightmap toggle option.
  - Improved material system.
  - Improved lightmap tone mapping.
  - Improved lightmap resize algorithm.
  - Improved default File System Browser open operation logic.
  - Fixed model frustum culling clipping bug.
  - Fixed occlusion culling bug caused by disabling models.
  - Fixed Object Viewer recent file menu.

  v1.0.0 Alpha 2
  - Added configuration saving/loading.
  - Added Open command to File System Browser.
  - Added File System Browser access to Material Browser.
  - Added File System Browser access to Model Browser.
  - Added rendering options to Model Browser.
  - Fixed Model Browser icon.
  - Fixed displacement map parent faces drawing in BPS files with no visibility data.

  v1.0.0 Alpha 1
  - Initial release.

==============================
Program Copyright-Permissions:
==============================

LICENSE 

Terms and Conditions for Copying, Distributing, and Modifying 

Items other than copying, distributing, and modifying the Content
with which this license was distributed (such as using, etc.) are
outside the scope of this license. 

1. You may copy and distribute exact replicas of Crafty as you receive
   it, in any medium, provided that you conspicuously and appropriately
   publish on each copy an appropriate copyright notice and disclaimer
   of warranty; keep intact all the notices that refer to this License
   and to the absence of any warranty; and give any other recipients of
   Crafty a copy of this License along with Crafty. You may at your
   option charge a fee for the media and/or handling involved in creating
   a unique copy of the Crafty for use offline, you may at your option offer
   instructional support for the Crafty in exchange for a fee, or you may at
   your option offer warranty in exchange for a fee. You may not charge a
   fee for Crafty itself. You may not charge a fee for the sole service
   of providing access to and/or use of Crafty via a network (e.g. the Internet),
   whether it be via the world wide web, FTP, or any other method. 

2. You may not modify your copy or copies of Crafty or any portion of it.

3. You are not required to accept this License, since you have not signed it.
   However, nothing else grants you permission to copy, distribute or modify Crafty.
   These actions are prohibited by law if you do not accept this License. Therefore,
   by copying or distributing Crafty you indicate your acceptance of this License to do
   so, and all its terms and conditions for copying, distributing and modifying Crafty. 

NO WARRANTY 

4. BECAUSE CRAFTY IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
   FOR CRAFTY, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN OTHERWISE STATED
   IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE CRAFTY "AS IS" WITHOUT
   WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO,
   THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE
   ENTIRE RISK OF USE OF THE CRAFTY IS WITH YOU. SHOULD CRAFTY PROVE FAULTY, INACCURATE, OR
   OTHERWISE UNACCEPTABLE YOU ASSUME THE COST OF ALL NECESSARY REPAIR OR CORRECTION. 

5. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING WILL ANY
   COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MIRROR AND/OR REDISTRIBUTE CRAFTY AS
   PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY GENERAL, SPECIAL,
   INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE CRAFTY,
   EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
