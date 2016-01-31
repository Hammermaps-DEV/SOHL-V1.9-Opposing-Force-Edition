/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*
*	This product contains software technology licensed from Id
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc.
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/

enum bgrap_e {
	BGRAP_BREATHE = 0,
	BGRAP_LONGIDLE,
	BGRAP_SHORTIDLE,
	BGRAP_COUGH,
	BGRAP_DOWN,
	BGRAP_UP,
	BGRAP_FIRE,
	BGRAP_FIREWAITING,
	BGRAP_FIREREACHED,
	BGRAP_FIRETRAVEL,
	BGRAP_FIRERELEASE,
};

class CGrappleTonguetip;
class CGrapple : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	int iItemSlot(void) { return 1; }
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack(void);
	BOOL Deploy(void);
	void Holster(void);
	void WeaponIdle(void);
	void ItemPostFrame(void);

	virtual BOOL ShouldWeaponIdle(void) { return TRUE; }

	void Fire(void);
	void FireWait(void);
	void FireReach(void);
	void FireTravel(void);
	void FireRelease(void);

	void Fire2(void);

	void OnTongueTipHitSurface(const Vector& vecTarget);
	void OnTongueTipHitEntity(CBaseEntity* pEntity);

	void StartPull(void);
	void StopPull(void);
	void Pull(void);

	BOOL IsTongueColliding(const Vector& vecShootOrigin, const Vector& vecTipPos);
	void CheckFireEligibility(void);
	BOOL CheckTargetProximity(void);

	void CreateTongueTip(void);
	void DestroyTongueTip(void);
	void UpdateTongueTip(void);

	void CreateBeam(CBaseEntity* pTongueTip);
	void DestroyBeam(void);
	void UpdateBeam(void);

	void StartPullSound(void);
	void UpdatePullSound(void);
	void ResetPullSound(void);

	BOOL CanAttack(float attack_time, float curtime, BOOL isPredicted);

	enum GRAPPLE_FIRESTATE
	{
		FIRESTATE_NONE = 0,
		FIRESTATE_FIRE,
		FIRESTATE_FIRE2,
		FIRESTATE_WAIT,
		FIRESTATE_REACH,
		FIRESTATE_TRAVEL,
		FIRESTATE_RELEASE,
	};

	int		m_iFirestate;
	int		m_iHitFlags;
	BOOL	m_fTipHit;
	CGrappleTonguetip* m_pTongueTip;
	CBeam*	m_pBeam;
	float	m_flNextPullSoundTime;
	BOOL	m_fPlayPullSound;
private:
};
