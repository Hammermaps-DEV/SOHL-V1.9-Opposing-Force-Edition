class CHgun : public CBasePlayerWeapon
{
public:
	void Spawn(void);
	void Precache(void);
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack(void);
	void SecondaryAttack(void);
	BOOL Deploy(void);
	BOOL IsUseable(void) { return TRUE; };
	void Holster();
	void Reload(void);
	void WeaponIdle(void);
	float m_flNextAnimTime;

	float m_flRechargeTime;
	int m_iFirePhase;// don't save me.
};
