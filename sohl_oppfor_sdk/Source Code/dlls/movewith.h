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
#define LF_NOTEASY				(1<<0)
#define LF_NOTMEDIUM			(1<<1)
#define LF_NOTHARD				(1<<2)

#define LF_POSTASSISTVEL			(1<<3)
#define LF_POSTASSISTAVEL			(1<<4)

#define LF_DOASSIST				(1<<5)
#define LF_CORRECTSPEED			(1<<6)

#define LF_DODESIRED			(1<<7)
#define LF_DESIRED_THINK			(1<<8)
#define LF_DESIRED_POSTASSIST			(1<<9)

#define LF_DESIRED_INFO			(1<<10)
#define LF_DESIRED_ACTION			(1<<11)

#define LF_MOVENONE				(1<<12)
#define LF_MERGEPOS				(1<<13)
#define LF_PARENTMOVE			(1<<14)
#define LF_ANGULAR				(1<<15)
#define LF_POSTORG				(1<<16)
#define LF_POINTENTITY			(1<<17)

#define LF_ALIASLIST			(1<<18)

// an entity must have one of these flags set in order to be in the AssistList
#define LF_ASSISTLIST  (LF_DOASSIST|LF_DODESIRED|LF_MERGEPOS|LF_POSTORG)

extern void CheckDesiredList(void);
extern void CheckAssistList(void);

extern void UTIL_DesiredAction(CBaseEntity *pEnt);
extern void UTIL_DesiredThink(CBaseEntity *pEnt);
extern void UTIL_DesiredInfo(CBaseEntity *pEnt);
extern void UTIL_DesiredPostAssist(CBaseEntity *pEnt);
extern void UTIL_AddToAssistList(CBaseEntity *pEnt);
extern void UTIL_MarkForAssist(CBaseEntity *pEnt, BOOL correctSpeed);
extern void UTIL_AssignOrigin(CBaseEntity* pEntity, const Vector vecOrigin);
extern void UTIL_AssignOrigin(CBaseEntity* pEntity, const Vector vecOrigin, BOOL bInitiator);
extern void UTIL_SetVelocity(CBaseEntity *pEnt, const Vector vecSet);
extern void UTIL_AssignAngles(CBaseEntity* pEntity, const Vector vecAngles);
extern void UTIL_AssignAngles(CBaseEntity* pEntity, const Vector vecAngles, BOOL bInitiator);
extern void UTIL_SetAvelocity(CBaseEntity *pEnt, const Vector vecSet);
extern void UTIL_SetMoveWithVelocity(CBaseEntity *pEnt, const Vector vecSet, int loopbreaker);
extern void UTIL_SetMoveWithAvelocity(CBaseEntity *pEnt, const Vector vecSet, int loopbreaker);
extern void UTIL_MergePos(CBaseEntity *pEnt, int loopbreaker = 100);