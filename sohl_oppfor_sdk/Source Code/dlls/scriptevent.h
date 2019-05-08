/***
*
*   SPIRIT OF HALF-LIFE 1.9: OPPOSING-FORCE EDITION
*
*   Half-Life and their logos are the property of their respective owners.
*   Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*   This product contains software technology licensed from Id
*   Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
*	Spirit of Half-Life, by Laurie R. Cheers. (LRC)
*   Modified by Lucas Brucksch (Code merge & Effects)
*   Modified by Andrew J Hamilton (AJH)
*   Modified by XashXT Group (g-cont...)
*
*   Code used from Battle Grounds Team and Contributors.
*   Code used from SamVanheer (Opposing Force code)
*   Code used from FWGS Team (Fixes for SOHL)
*   Code used from LevShisterov (Bugfixed and improved HLSDK)
*	Code used from Fograin (Half-Life: Update MOD)
*
***/
#ifndef SCRIPTEVENT_H
#define SCRIPTEVENT_H

#define SCRIPT_EVENT_DEAD			1000		// character is now dead
#define SCRIPT_EVENT_NOINTERRUPT	1001		// does not allow interrupt
#define SCRIPT_EVENT_CANINTERRUPT	1002		// will allow interrupt
#define SCRIPT_EVENT_FIREEVENT		1003		// event now fires
#define SCRIPT_EVENT_SOUND			1004		// Play named wave file (on CHAN_BODY)
#define SCRIPT_EVENT_SENTENCE		1005		// Play named sentence
#define SCRIPT_EVENT_INAIR			1006		// Leave the character in air at the end of the sequence (don't find the floor)
#define SCRIPT_EVENT_ENDANIMATION	1007		// Set the animation by name after the sequence completes
#define SCRIPT_EVENT_SOUND_VOICE	1008		// Play named wave file (on CHAN_VOICE)
#define	SCRIPT_EVENT_SENTENCE_RND1	1009		// Play sentence group 25% of the time
#define SCRIPT_EVENT_NOT_DEAD		1010		// Bring back to life (for life/death sequences)
#endif   //SCRIPTEVENT_H
