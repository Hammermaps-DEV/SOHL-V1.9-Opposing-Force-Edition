#ifndef CCYCLER_H
#define CCYCLER_H

class CCycler : public CBaseMonster
{
public:
	virtual void GenericCyclerSpawn(char *szModel, Vector vecMin, Vector vecMax);
	virtual int	ObjectCaps() { return (CBaseEntity::ObjectCaps() | FCAP_IMPULSE_USE); }
	virtual int TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	virtual void Spawn();
	virtual void Think();
	//void Pain( float flDamage );
	virtual void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	// Don't treat as a live target
	virtual bool IsAlive() { return false; }

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int			m_animate;
};
#endif // CCYCLER_H
