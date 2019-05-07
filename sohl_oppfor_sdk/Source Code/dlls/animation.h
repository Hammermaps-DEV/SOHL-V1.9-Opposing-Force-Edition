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
#ifndef ANIMATION_H
#define ANIMATION_H

#define ACTIVITY_NOT_AVAILABLE		-1

#ifndef MONSTEREVENT_H
#include "monsterevent.h"
#endif

extern int IsSoundEvent(int eventNumber);

int LookupActivity(void *pmodel, entvars_t *pev, int activity);
int LookupActivityHeaviest(void *pmodel, entvars_t *pev, int activity);
int LookupSequence(void *pmodel, const char *label);
void GetSequenceInfo(void *pmodel, entvars_t *pev, float *pflFrameRate, float *pflGroundSpeed);
int GetSequenceFlags(void *pmodel, entvars_t *pev);
int LookupAnimationEvents(void *pmodel, entvars_t *pev, float flStart, float flEnd);
float SetController(void *pmodel, entvars_t *pev, int iController, float flValue);
float SetBlending(void *pmodel, entvars_t *pev, int iBlender, float flValue);
void GetEyePosition(void *pmodel, float *vecEyePosition);
void SequencePrecache(void *pmodel, const char *pSequenceName);
int FindTransition(void *pmodel, int iEndingAnim, int iGoalAnim, int *piDir);
void SetBodygroup(void *pmodel, entvars_t *pev, int iGroup, int iValue);
int GetBodygroup(void *pmodel, entvars_t *pev, int iGroup);

//LRC
void SetBones(void *pmodel, float(*data)[3], int datasize);
int GetBoneCount(void *pmodel);
int GetSequenceFrames(void *pmodel, entvars_t *pev); //LRC

int GetAnimationEvent(void *pmodel, entvars_t *pev, MonsterEvent_t *pMonsterEvent, float flStart, float flEnd, int index);
int ExtractBbox(void *pmodel, int sequence, float *mins, float *maxs);

// From /engine/studio.h
#define STUDIO_LOOPING		0x0001


#endif	//ANIMATION_H
