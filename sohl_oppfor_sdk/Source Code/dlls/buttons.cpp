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
/*

===== buttons.cpp ========================================================

  button-related code

*/

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "doors.h"

#define SF_BUTTON_DONTMOVE		1
#define SF_ROTBUTTON_NOTSOLID	1
#define SF_BUTTON_ONLYDIRECT	16  //LRC - button can't be used through walls.
#define	SF_BUTTON_TOGGLE		32	// button stays pushed until reactivated
#define	SF_BUTTON_SPARK_IF_OFF	64	// button sparks in OFF state
#define SF_BUTTON_NOT_SOLID		128	// button isn't solid
#define SF_BUTTON_TOUCH_ONLY	256	// button must be touched to be used.
#define SF_BUTTON_USEKEY		512 // change the reaction of the button to the USE key.

//===================================
// func_button (= CBaseButton)
//===================================

//LRC - moved here from cbase.h to use the spawnflags defined in this file
// Buttons that don't take damage can be IMPULSE used
int	CBaseButton::ObjectCaps()
{
	return (CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) |
		(pev->takedamage ? 0 : FCAP_IMPULSE_USE) |
		(pev->spawnflags & SF_BUTTON_ONLYDIRECT ? FCAP_ONLYDIRECT_USE : 0);
}

TYPEDESCRIPTION CBaseButton::m_SaveData[] =
{
	DEFINE_FIELD(CBaseButton, m_fStayPushed, FIELD_BOOLEAN),
	DEFINE_FIELD(CBaseButton, m_fRotating, FIELD_BOOLEAN),

	DEFINE_FIELD(CBaseButton, m_sounds, FIELD_INTEGER),
	DEFINE_FIELD(CBaseButton, m_bLockedSound, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseButton, m_bLockedSentence, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseButton, m_bUnlockedSound, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseButton, m_bUnlockedSentence, FIELD_CHARACTER),
	DEFINE_FIELD(CBaseButton, m_strChangeTarget, FIELD_STRING),
	//	DEFINE_FIELD( CBaseButton, m_ls, FIELD_??? ),   // This is restored in Precache()
};


IMPLEMENT_SAVERESTORE(CBaseButton, CBaseToggle);

void CBaseButton::Precache()
{
	char *pszSound;

	if (FBitSet(pev->spawnflags, SF_BUTTON_SPARK_IF_OFF))// this button should spark in OFF state
	{
		PRECACHE_SOUND("buttons/spark1.wav");
		PRECACHE_SOUND("buttons/spark2.wav");
		PRECACHE_SOUND("buttons/spark3.wav");
		PRECACHE_SOUND("buttons/spark4.wav");
		PRECACHE_SOUND("buttons/spark5.wav");
		PRECACHE_SOUND("buttons/spark6.wav");
	}

	// get door button sounds, for doors which require buttons to open

	if (m_bLockedSound)
	{
		pszSound = ButtonSound((int)m_bLockedSound);
		PRECACHE_SOUND(pszSound);
		m_ls.sLockedSound = ALLOC_STRING(pszSound);
	}

	if (m_bUnlockedSound)
	{
		pszSound = ButtonSound((int)m_bUnlockedSound);
		PRECACHE_SOUND(pszSound);
		m_ls.sUnlockedSound = ALLOC_STRING(pszSound);
	}

	// get sentence group names, for doors which are directly 'touched' to open

	switch (m_bLockedSentence)
	{
	case 1: m_ls.sLockedSentence = MAKE_STRING("NA"); break; // access denied
	case 2: m_ls.sLockedSentence = MAKE_STRING("ND"); break; // security lockout
	case 3: m_ls.sLockedSentence = MAKE_STRING("NF"); break; // blast door
	case 4: m_ls.sLockedSentence = MAKE_STRING("NFIRE"); break; // fire door
	case 5: m_ls.sLockedSentence = MAKE_STRING("NCHEM"); break; // chemical door
	case 6: m_ls.sLockedSentence = MAKE_STRING("NRAD"); break; // radiation door
	case 7: m_ls.sLockedSentence = MAKE_STRING("NCON"); break; // gen containment
	case 8: m_ls.sLockedSentence = MAKE_STRING("NH"); break; // maintenance door
	case 9: m_ls.sLockedSentence = MAKE_STRING("NG"); break; // broken door

	default: m_ls.sLockedSentence = 0; break;
	}

	switch (m_bUnlockedSentence)
	{
	case 1: m_ls.sUnlockedSentence = MAKE_STRING("EA"); break; // access granted
	case 2: m_ls.sUnlockedSentence = MAKE_STRING("ED"); break; // security door
	case 3: m_ls.sUnlockedSentence = MAKE_STRING("EF"); break; // blast door
	case 4: m_ls.sUnlockedSentence = MAKE_STRING("EFIRE"); break; // fire door
	case 5: m_ls.sUnlockedSentence = MAKE_STRING("ECHEM"); break; // chemical door
	case 6: m_ls.sUnlockedSentence = MAKE_STRING("ERAD"); break; // radiation door
	case 7: m_ls.sUnlockedSentence = MAKE_STRING("ECON"); break; // gen containment
	case 8: m_ls.sUnlockedSentence = MAKE_STRING("EH"); break; // maintenance door

	default: m_ls.sUnlockedSentence = 0; break;
	}
}

//
// Cache user-entity-field values until spawn is called.
//

void CBaseButton::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "changetarget"))
	{
		m_strChangeTarget = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sound"))
	{
		m_bLockedSound = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "locked_sentence"))
	{
		m_bLockedSentence = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sound"))
	{
		m_bUnlockedSound = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "unlocked_sentence"))
	{
		m_bUnlockedSentence = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sounds"))
	{
		m_sounds = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue(pkvd);
}

//
// ButtonShot
//
int CBaseButton::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	BUTTON_CODE code = ButtonResponseToTouch();

	if (code == BUTTON_NOTHING)
		return 0;
	// Temporarily disable the touch function, until movement is finished.
	SetTouch(NULL);

	m_hActivator = CBaseEntity::Instance(pevAttacker);
	if (m_hActivator == NULL)
		return 0;

	if (code == BUTTON_RETURN)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), VOL_NORM, ATTN_NORM);

		// Toggle buttons fire when they get back to their "home" position
		if (!(pev->spawnflags & SF_BUTTON_TOGGLE))
			SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);
		ButtonReturn();
	}
	else // code == BUTTON_ACTIVATE
		ButtonActivate();

	return 0;
}

/*QUAKED func_button (0 .5 .8) ?
When a button is touched, it moves some distance in the direction of it's angle,
triggers all of it's targets, waits some time, then returns to it's original position
where it can be triggered again.

"angle"		determines the opening direction
"target"	all entities with a matching targetname will be used
"speed"		override the default 40 speed
"wait"		override the default 1 second wait (-1 = never return)
"lip"		override the default 4 pixel lip remaining at end of move
"health"	if set, the button must be killed instead of touched
"sounds"
0) steam metal
1) wooden clunk
2) metallic click
3) in-out
*/
LINK_ENTITY_TO_CLASS(func_button, CBaseButton);


void CBaseButton::Spawn()
{
	char  *pszSound;

	//----------------------------------------------------
	//determine sounds for buttons
	//a sound of 0 should not make a sound
	//----------------------------------------------------
	pszSound = ButtonSound(m_sounds);
	PRECACHE_SOUND(pszSound);
	pev->noise = ALLOC_STRING(pszSound);

	Precache();

	if (FBitSet(pev->spawnflags, SF_BUTTON_SPARK_IF_OFF))// this button should spark in OFF state
	{
		SetThink(&CBaseButton::ButtonSpark);
		SetNextThink(0.5);// no hurry, make sure everything else spawns
	}

	SetMovedir(pev);

	pev->movetype = MOVETYPE_PUSH;
	if (FBitSet(pev->spawnflags, SF_BUTTON_NOT_SOLID))
	{
		pev->solid = SOLID_NOT;
		pev->skin = CONTENTS_EMPTY;
	}
	else
	{
		pev->solid = SOLID_BSP;
	}
	SET_MODEL(ENT(pev), STRING(pev->model));

	//LRC
	if (m_iStyle >= 32) LIGHT_STYLE(m_iStyle, "z");
	else if (m_iStyle <= -32) LIGHT_STYLE(-m_iStyle, "a");

	if (pev->speed == 0)
		pev->speed = 40;

	if (pev->health > 0)
	{
		pev->takedamage = DAMAGE_YES;
	}

	if (m_flWait == 0)
		m_flWait = 1;
	if (m_flLip == 0)
		m_flLip = 4;

	m_toggle_state = TS_AT_BOTTOM;
	m_vecPosition1 = pev->origin;
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions making the size too big
	m_vecPosition2 = m_vecPosition1 + (pev->movedir * (fabs(pev->movedir.x * (pev->size.x - 2)) + fabs(pev->movedir.y * (pev->size.y - 2)) + fabs(pev->movedir.z * (pev->size.z - 2)) - m_flLip));


	// Is this a non-moving button?
	if (((m_vecPosition2 - m_vecPosition1).Length() < 1) || (pev->spawnflags & SF_BUTTON_DONTMOVE))
		m_vecPosition2 = m_vecPosition1;

	m_fStayPushed = (m_flWait == -1 ? TRUE : FALSE);
	m_fRotating = FALSE;

	// if the button is flagged for USE button activation only, take away it's touch function and add a use function

	if (FBitSet(pev->spawnflags, SF_BUTTON_TOUCH_ONLY)) // touchable button
	{
		SetTouch(&CBaseButton::ButtonTouch);
		if (!FBitSet(pev->spawnflags, SF_BUTTON_USEKEY))
			SetUse(&CBaseButton::ButtonUse_IgnorePlayer);
		else
			SetUse(&CBaseButton::ButtonUse);
	}
	else
	{
		SetTouch(NULL);
		if (FBitSet(pev->spawnflags, SF_BUTTON_USEKEY))
			SetUse(&CBaseButton::ButtonUse_IgnorePlayer);
		else
			SetUse(&CBaseButton::ButtonUse);
	}
}

//LRC
void CBaseButton::PostSpawn()
{
	if (m_pMoveWith)
		m_vecPosition1 = pev->origin - m_pMoveWith->pev->origin;
	else
		m_vecPosition1 = pev->origin;
	// Subtract 2 from size because the engine expands bboxes by 1 in all directions
	m_vecPosition2 = m_vecPosition1 + (pev->movedir * (fabs(pev->movedir.x * (pev->size.x - 2)) + fabs(pev->movedir.y * (pev->size.y - 2)) + fabs(pev->movedir.z * (pev->size.z - 2)) - m_flLip));

	// Is this a non-moving button?
	if (((m_vecPosition2 - m_vecPosition1).Length() < 1) || (pev->spawnflags & SF_BUTTON_DONTMOVE))
		m_vecPosition2 = m_vecPosition1;
}

// Button sound table. 
// Also used by CBaseDoor to get 'touched' door lock/unlock sounds

char *ButtonSound(int sound)
{
	char *pszSound;

	switch (sound)
	{
	case 0: pszSound = "common/null.wav";        break;
	case 1: pszSound = "buttons/button1.wav";	break;
	case 2: pszSound = "buttons/button2.wav";	break;
	case 3: pszSound = "buttons/button3.wav";	break;
	case 4: pszSound = "buttons/button4.wav";	break;
	case 5: pszSound = "buttons/button5.wav";	break;
	case 6: pszSound = "buttons/button6.wav";	break;
	case 7: pszSound = "buttons/button7.wav";	break;
	case 8: pszSound = "buttons/button8.wav";	break;
	case 9: pszSound = "buttons/button9.wav";	break;
	case 10: pszSound = "buttons/button10.wav";	break;
	case 11: pszSound = "buttons/button11.wav";	break;
	case 12: pszSound = "buttons/latchlocked1.wav";	break;
	case 13: pszSound = "buttons/latchunlocked1.wav";	break;
	case 14: pszSound = "buttons/lightswitch2.wav"; break;

		// next 6 slots reserved for any additional sliding button sounds we may add

	case 21: pszSound = "buttons/lever1.wav";	break;
	case 22: pszSound = "buttons/lever2.wav";	break;
	case 23: pszSound = "buttons/lever3.wav";	break;
	case 24: pszSound = "buttons/lever4.wav";	break;
	case 25: pszSound = "buttons/lever5.wav";	break;

	default:pszSound = "buttons/button9.wav";	break;
	}

	return pszSound;
}

//
// Makes flagged buttons spark when turned off
//

void CBaseButton::DoSpark(entvars_t *pev, const Vector &location)
{
	Vector tmp = location + pev->size * 0.5;
	UTIL_Sparks(tmp);

	float flVolume = RANDOM_FLOAT(0.25, 0.75) * 0.4;//random volume range
	switch ((int)(RANDOM_FLOAT(0, 1) * 6))
	{
	case 0: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark1.wav", flVolume, ATTN_NORM);	break;
	case 1: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark2.wav", flVolume, ATTN_NORM);	break;
	case 2: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark3.wav", flVolume, ATTN_NORM);	break;
	case 3: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark4.wav", flVolume, ATTN_NORM);	break;
	case 4: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark5.wav", flVolume, ATTN_NORM);	break;
	case 5: EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark6.wav", flVolume, ATTN_NORM);	break;
	}
}

void CBaseButton::ButtonSpark()
{
	SetThink(&CBaseButton::ButtonSpark);
	SetNextThink(0.1 + RANDOM_FLOAT(0, 1.5));// spark again at random interval

	DoSpark(pev, pev->mins);
}


//
// Button's Use function
//
void CBaseButton::ButtonUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Ignore touches if button is moving, or pushed-in and waiting to auto-come-out.
	// UNDONE: Should this use ButtonResponseToTouch() too?
	if (m_toggle_state == TS_GOING_UP || m_toggle_state == TS_GOING_DOWN)
		return;

	m_hActivator = pActivator;
	if (m_toggle_state == TS_AT_TOP)
	{
		if (!m_fStayPushed && FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE))
		{
			EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), VOL_NORM, ATTN_NORM);

			//SUB_UseTargets( m_eoActivator );
			ButtonReturn();
		}
	}
	else
		ButtonActivate();
}

//LRC - they had it set up so that a touch-only button couldn't even be triggered!?
void CBaseButton::ButtonUse_IgnorePlayer(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!pCaller || !pCaller->IsPlayer())
		ButtonUse(pActivator, pCaller, useType, value);
}

CBaseButton::BUTTON_CODE CBaseButton::ButtonResponseToTouch()
{
	// Ignore touches if button is moving, or pushed-in and waiting to auto-come-out.
	if (m_toggle_state == TS_GOING_UP ||
		m_toggle_state == TS_GOING_DOWN ||
		(m_toggle_state == TS_AT_TOP && !m_fStayPushed && !FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE)))
		return BUTTON_NOTHING;

	if (m_toggle_state == TS_AT_TOP)
	{
		if ((FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE)) && !m_fStayPushed)
		{
			return BUTTON_RETURN;
		}
	}
	else
		return BUTTON_ACTIVATE;

	return BUTTON_NOTHING;
}


//
// Touching a button simply "activates" it.
//
void CBaseButton::ButtonTouch(CBaseEntity *pOther)
{
	// Ignore touches by anything but players
	if (!FClassnameIs(pOther->pev, "player"))
		return;

	m_hActivator = pOther;

	BUTTON_CODE code = ButtonResponseToTouch();

	if (code == BUTTON_NOTHING)
		return;

	if (!UTIL_IsMasterTriggered(m_sMaster, pOther))
	{
		// play button locked sound
		PlayLockSounds(pev, &m_ls, TRUE, TRUE);
		return;
	}

	// Temporarily disable the touch function, until movement is finished.
	SetTouch(NULL);

	if (code == BUTTON_RETURN)
	{
		EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), VOL_NORM, ATTN_NORM);
		SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);
		ButtonReturn();
	}
	else	// code == BUTTON_ACTIVATE
		ButtonActivate();
}

//
// Starts the button moving "in/up".
//
void CBaseButton::ButtonActivate()
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), VOL_NORM, ATTN_NORM);

	if (!UTIL_IsMasterTriggered(m_sMaster, m_hActivator))
	{
		// button is locked, play locked sound
		PlayLockSounds(pev, &m_ls, TRUE, TRUE);
		return;
	}
	else
	{
		// button is unlocked, play unlocked sound
		PlayLockSounds(pev, &m_ls, FALSE, TRUE);
	}

	ASSERT(m_toggle_state == TS_AT_BOTTOM);
	m_toggle_state = TS_GOING_UP;

	//LRC - unhelpfully, SF_BUTTON_DONTMOVE is the same value as
	// SF_ROTBUTTON_NOTSOLID, so we have to assume that a rotbutton will
	// never be DONTMOVE.
	if (pev->spawnflags & SF_BUTTON_DONTMOVE && !m_fRotating)
	{
		TriggerAndWait();
	}
	else
	{
		SetMoveDone(&CBaseButton::TriggerAndWait);
		if (!m_fRotating)
			LinearMove(m_vecPosition2, pev->speed);
		else
			AngularMove(m_vecAngle2, pev->speed);
	}
}

//
// Button has reached the "in/up" position.  Activate its "targets", and pause before "popping out".
//
void CBaseButton::TriggerAndWait()
{
	ASSERT(m_toggle_state == TS_GOING_UP);

	if (!UTIL_IsMasterTriggered(m_sMaster, m_hActivator))
		return;

	m_toggle_state = TS_AT_TOP;

	pev->frame = 1;			// use alternate textures
	//LRC
	if (m_iStyle >= 32) LIGHT_STYLE(m_iStyle, "a");
	else if (m_iStyle <= -32) LIGHT_STYLE(-m_iStyle, "z");

	SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);

	// If button automatically comes back out, start it moving out.
	// Else re-instate touch method
	if (m_fStayPushed || FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE))
	{
		if (!FBitSet(pev->spawnflags, SF_BUTTON_TOUCH_ONLY)) // this button only works if USED, not touched!
		{
			// ALL buttons are now use only
			SetTouch(NULL);
		}
		else
			SetTouch(&CBaseButton::ButtonTouch);
	}
	else
	{
		SetThink(&CBaseButton::ButtonReturn);
		if (m_flWait)
		{
			SetNextThink(m_flWait);
		}
		else
		{
			ButtonReturn();
		}
	}
}


//
// Starts the button moving "out/down".
//
void CBaseButton::ButtonReturn()
{
	ASSERT(m_toggle_state == TS_AT_TOP);
	m_toggle_state = TS_GOING_DOWN;

	pev->frame = 0;			// use normal textures

	//LRC
	if (m_iStyle >= 32) LIGHT_STYLE(m_iStyle, "z");
	else if (m_iStyle <= -32) LIGHT_STYLE(-m_iStyle, "a");

	if (pev->spawnflags & SF_BUTTON_DONTMOVE)
	{
		ButtonBackHome();
	}
	else
	{
		SetMoveDone(&CBaseButton::ButtonBackHome);
		if (!m_fRotating)
			LinearMove(m_vecPosition1, pev->speed);
		else
			AngularMove(m_vecAngle1, pev->speed);
	}
}


//
// Button has returned to start state.  Quiesce it.
//
void CBaseButton::ButtonBackHome()
{
	ASSERT(m_toggle_state == TS_GOING_DOWN);
	m_toggle_state = TS_AT_BOTTOM;

	if (FBitSet(pev->spawnflags, SF_BUTTON_TOGGLE))
	{
		//EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), VOL_NORM, ATTN_NORM);

		SUB_UseTargets(m_hActivator, USE_TOGGLE, 0);
	}


	if (!FStringNull(pev->target))
	{
		CBaseEntity *pTarget = NULL;
		for (;;)
		{
			pTarget = UTIL_FindEntityByTargetname(pTarget, STRING(pev->target), m_hActivator);

			if (FNullEnt(pTarget))
				break;

			if (!FClassnameIs(pTarget->pev, "multisource"))
				// LRC- hmm... I see. On returning, a button will only turn off multisources.
				continue;

			pTarget->Use(m_hActivator, this, USE_TOGGLE, 0);
		}
	}

	// Re-instate touch method, movement cycle is complete.
	if (!FBitSet(pev->spawnflags, SF_BUTTON_TOUCH_ONLY)) // this button only works if USED, not touched!
	{
		// All buttons are now use only	
		SetTouch(NULL);
	}
	else
		SetTouch(&CBaseButton::ButtonTouch);

	// reset think for a sparking button
	if (FBitSet(pev->spawnflags, SF_BUTTON_SPARK_IF_OFF))
	{
		SetThink(&CBaseButton::ButtonSpark);
		SetNextThink(0.5);// no hurry.
	}
	else
	{
		DontThink();
	}
}



//
// Rotating button (aka "lever")
//
class CRotButton : public CBaseButton
{
public:
	void Spawn();
	void PostSpawn() {} // don't use the moveWith fix from CBaseButton
	virtual void KeyValue(KeyValueData* pkvd);
};

LINK_ENTITY_TO_CLASS(func_rot_button, CRotButton);

void CRotButton::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "axes"))
	{
		UTIL_StringToVector((float*)(pev->movedir), pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseButton::KeyValue(pkvd);
}

void CRotButton::Spawn()
{
	char *pszSound;
	//----------------------------------------------------
	//determine sounds for buttons
	//a sound of 0 should not make a sound
	//----------------------------------------------------
	pszSound = ButtonSound(m_sounds);
	PRECACHE_SOUND(pszSound);
	pev->noise = ALLOC_STRING(pszSound);

	// set the axis of rotation
	CBaseToggle::AxisDir(pev);

	// check for clockwise rotation
	if (FBitSet(pev->spawnflags, SF_DOOR_ROTATE_BACKWARDS))
		pev->movedir = pev->movedir * -1;

	pev->movetype = MOVETYPE_PUSH;

	if (pev->spawnflags & SF_ROTBUTTON_NOTSOLID)
		pev->solid = SOLID_NOT;
	else
		pev->solid = SOLID_BSP;

	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->speed == 0)
		pev->speed = 40;

	if (m_flWait == 0)
		m_flWait = 1;

	if (pev->health > 0)
	{
		pev->takedamage = DAMAGE_YES;
	}

	m_toggle_state = TS_AT_BOTTOM;
	m_vecAngle1 = pev->angles;
	m_vecAngle2 = pev->angles + pev->movedir * m_flMoveDistance;
	ASSERTSZ(m_vecAngle1 != m_vecAngle2, "rotating button start/end positions are equal");

	m_fStayPushed = (m_flWait == -1 ? TRUE : FALSE);
	m_fRotating = TRUE;

	// if the button is flagged for USE button activation only, take away it's touch function and add a use function
	if (!FBitSet(pev->spawnflags, SF_BUTTON_TOUCH_ONLY))
	{
		SetTouch(NULL);
		if (FBitSet(pev->spawnflags, SF_BUTTON_USEKEY))
			SetUse(&CRotButton::ButtonUse_IgnorePlayer);
		else
			SetUse(&CRotButton::ButtonUse);
	}
	else // touchable button
	{
		SetTouch(&CRotButton::ButtonTouch);
		if (!FBitSet(pev->spawnflags, SF_BUTTON_USEKEY))
			SetUse(&CRotButton::ButtonUse_IgnorePlayer);
		else
			SetUse(&CRotButton::ButtonUse);
	}

	//SetTouch( ButtonTouch );
}


// Make this button behave like a door (HACKHACK)
// This will disable use and make the button solid
// rotating buttons were made SOLID_NOT by default since their were some
// collision problems with them...
#define SF_MOMENTARY_DOOR		0x0001

class CMomentaryRotButton : public CBaseToggle
{
public:
	void	Spawn();
	void	KeyValue(KeyValueData *pkvd);
	virtual int	ObjectCaps()
	{
		int flags = CBaseToggle::ObjectCaps() & (~FCAP_ACROSS_TRANSITION);
		if (pev->spawnflags & SF_MOMENTARY_DOOR)
			return flags;
		return flags | FCAP_CONTINUOUS_USE;
	}
	void	Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	void	EXPORT Off();
	void	EXPORT Return();
	void	UpdateSelf(float value);
	void	UpdateSelfReturn(float value);
	void	UpdateAllButtons(float value, int start);

	void	PlaySound();
	void	UpdateTarget(float value);

	static CMomentaryRotButton *Instance(edict_t *pent) { return (CMomentaryRotButton *)GET_PRIVATE(pent); };
	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);

	static	TYPEDESCRIPTION m_SaveData[];

	int		m_lastUsed;
	int		m_direction;
	float	m_returnSpeed;
	vec3_t	m_start;
	vec3_t	m_end;
	int		m_sounds;
};
TYPEDESCRIPTION CMomentaryRotButton::m_SaveData[] =
{
	DEFINE_FIELD(CMomentaryRotButton, m_lastUsed, FIELD_INTEGER),
	DEFINE_FIELD(CMomentaryRotButton, m_direction, FIELD_INTEGER),
	DEFINE_FIELD(CMomentaryRotButton, m_returnSpeed, FIELD_FLOAT),
	DEFINE_FIELD(CMomentaryRotButton, m_start, FIELD_VECTOR),
	DEFINE_FIELD(CMomentaryRotButton, m_end, FIELD_VECTOR),
	DEFINE_FIELD(CMomentaryRotButton, m_sounds, FIELD_INTEGER),
};

IMPLEMENT_SAVERESTORE(CMomentaryRotButton, CBaseToggle);

LINK_ENTITY_TO_CLASS(momentary_rot_button, CMomentaryRotButton);

void CMomentaryRotButton::Spawn()
{
	CBaseToggle::AxisDir(pev);

	if (pev->speed == 0)
		pev->speed = 100;

	if (m_flMoveDistance < 0)
	{
		m_start = pev->angles + pev->movedir * m_flMoveDistance;
		m_end = pev->angles;
		m_direction = 1;		// This will toggle to -1 on the first use()
		m_flMoveDistance = -m_flMoveDistance;
	}
	else
	{
		m_start = pev->angles;
		m_end = pev->angles + pev->movedir * m_flMoveDistance;
		m_direction = -1;		// This will toggle to +1 on the first use()
	}

	if (pev->spawnflags & SF_MOMENTARY_DOOR)
		pev->solid = SOLID_BSP;
	else
		pev->solid = SOLID_NOT;

	pev->movetype = MOVETYPE_PUSH;
	UTIL_SetOrigin(this, pev->origin);
	SET_MODEL(ENT(pev), STRING(pev->model));

	char *pszSound = ButtonSound(m_sounds);
	PRECACHE_SOUND(pszSound);
	pev->noise = ALLOC_STRING(pszSound);
	m_lastUsed = 0;
}

void CMomentaryRotButton::KeyValue(KeyValueData *pkvd)
{
	if (FStrEq(pkvd->szKeyName, "returnspeed"))
	{
		m_returnSpeed = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "sounds"))
	{
		m_sounds = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "axes"))
	{
		UTIL_StringToVector((float*)(pev->movedir), pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue(pkvd);
}

void CMomentaryRotButton::PlaySound()
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), VOL_NORM, ATTN_NORM);
}

// BUGBUG: This design causes a latentcy.  When the button is retriggered, the first impulse
// will send the target in the wrong direction because the parameter is calculated based on the
// current, not future position.
void CMomentaryRotButton::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (IsLockedByMaster()) return; //LRC
	// the distance between the current angle and the "base" angle.
	pev->ideal_yaw = CBaseToggle::AxisDelta(pev->spawnflags, pev->angles, m_start) / m_flMoveDistance;

	UpdateAllButtons(pev->ideal_yaw, 1);

	float f = m_fNextThink - pev->ltime;
	f = CBaseToggle::AxisDelta(pev->spawnflags, pev->angles + pev->avelocity*f, m_start) / m_flMoveDistance;
	//	ALERT(at_console,"sending update = %f\n", f);
	UpdateTarget(f);
}

void CMomentaryRotButton::UpdateAllButtons(float value, int start)
{
	// Update all rot buttons attached to my target
	// (this includes myself)
	CBaseEntity *pTarget = NULL;
	for (;;)
	{
		pTarget = UTIL_FindEntityByTarget(pTarget, STRING(pev->target));
		if (FNullEnt(pTarget))
			break;

		if (FClassnameIs(pTarget->pev, "momentary_rot_button"))
		{
			CMomentaryRotButton *pEntity = (CMomentaryRotButton*)pTarget;
			if (start)
				pEntity->UpdateSelf(value);
			else
				pEntity->UpdateSelfReturn(value);
		}
	}
}

void CMomentaryRotButton::UpdateSelf(float value)
{
	BOOL fplaysound = FALSE;

	if (!m_lastUsed)
	{
		fplaysound = TRUE;
		m_direction = -m_direction;
	}
	m_lastUsed = 1;

	SetNextThink(0.1);

	//LRC check if we're outside the boundaries
	if (m_direction > 0 && value >= 1.0)
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_end;
		return;
	}

	if (m_direction < 0 && value <= 0)
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_start;
		return;
	}

	if (fplaysound)
		PlaySound();

	// HACKHACK -- If we're going slow, we'll get multiple player packets per frame;
	// bump nexthink on each one to avoid stalling
	//LRC- that is to say: our avelocity will get us to the target point in 0.1 secs.
	// If we're being told to move further than that, wait that much longer.
	if (m_fNextThink < pev->ltime)
		SetNextThink(0.1);
	else
	{
		AbsoluteNextThink(m_fNextThink + 0.1);
	}

	pev->avelocity = (m_direction * pev->speed) * pev->movedir;
	SetThink(&CMomentaryRotButton::Off);
}

void CMomentaryRotButton::UpdateTarget(float value)
{
	if (!FStringNull(pev->target))
	{
		CBaseEntity* pTarget = NULL;
		for (;;)
		{
			pTarget = UTIL_FindEntityByTargetname(pTarget, STRING(pev->target));
			if (!pTarget)
				break;
			pTarget->Use(this, this, USE_SET, value);
		}
	}
}

void CMomentaryRotButton::Off()
{
	pev->avelocity = g_vecZero;
	m_lastUsed = 0;
	if (FBitSet(pev->spawnflags, SF_PENDULUM_AUTO_RETURN) && m_returnSpeed > 0)
	{
		SetThink(&CMomentaryRotButton::Return);
		SetNextThink(0.1);
		m_direction = -1;
	}
	else
		SetThink(NULL);
}

void CMomentaryRotButton::Return()
{
	float value = CBaseToggle::AxisDelta(pev->spawnflags, pev->angles, m_start) / m_flMoveDistance;

	UpdateAllButtons(value, 0);	// This will end up calling UpdateSelfReturn() n times, but it still works right
	if (value > 0)
		UpdateTarget(value);
}


void CMomentaryRotButton::UpdateSelfReturn(float value)
{
	if (value <= 0)
	{
		pev->avelocity = g_vecZero;
		pev->angles = m_start;
		DontThink();
		SetThink(NULL);
	}
	else
	{
		pev->avelocity = -m_returnSpeed * pev->movedir;
		SetNextThink(0.1);
	}
}

//----------------------------------------------------------------
// Button_target
//G-Cont. flag 16 is removed - we don't need this
#define SF_BTARGET_USE		0x0001
#define SF_BTARGET_ON		0x0002
#define SF_BTARGET_SOLIDNOT 0x0004	//AJH - Just testing this at the moment
#define SF_BTARGET_NOSHOT	0x0008	//AJH - So you can't trigger by shooting

class CButtonTarget : public CBaseEntity
{
public:
	virtual void KeyValue(KeyValueData *pkvd);//AJH
	void Spawn();
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	int	ObjectCaps();
	string_t m_sMaster;	//AJH for lockable button_targets

};

LINK_ENTITY_TO_CLASS(button_target, CButtonTarget);

void CButtonTarget::KeyValue(KeyValueData *pkvd) //AJH
{
	if (FStrEq(pkvd->szKeyName, "master"))
	{
		m_sMaster = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue(pkvd);
}

void CButtonTarget::Spawn()
{
	pev->movetype = MOVETYPE_PUSH;
	if (pev->spawnflags&SF_BTARGET_SOLIDNOT) {	//AJH - non solid button targets
		pev->solid = SOLID_NOT;					//note: setting non solid will stop 
	}
	else {										//'trigger on shot' as no collision occurs
		pev->solid = SOLID_BSP;			//Default behaviour is SOLID
	}											//

	SET_MODEL(ENT(pev), STRING(pev->model));

	if (pev->spawnflags&SF_BTARGET_NOSHOT) {		//AJH - Don't allow triggering when shot
		pev->takedamage = DAMAGE_NO;			//Default: allow triggering
	}
	else {										//
		pev->takedamage = DAMAGE_YES;			//
	}											//

	if (FBitSet(pev->spawnflags, SF_BTARGET_ON))
		pev->frame = 1;
}

void CButtonTarget::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	if (!ShouldToggle(useType, (int)pev->frame))
		return;

	if (!UTIL_IsMasterTriggered(m_sMaster, pActivator))		//
		return;										// AJH allows for locked button_targets 

	pev->frame = 1 - pev->frame;
	if (pev->frame)
		SUB_UseTargets(pActivator, USE_ON, 0);
	else
		SUB_UseTargets(pActivator, USE_OFF, 0);
}


int	CButtonTarget::ObjectCaps()
{
	int caps = CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION;

	if (FBitSet(pev->spawnflags, SF_BTARGET_USE))
		return caps | FCAP_IMPULSE_USE;
	else
		return caps;
}


int CButtonTarget::TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	Use(CBaseEntity::Instance(pevAttacker), this, USE_TOGGLE, 0);

	return 1;
}
