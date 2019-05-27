class CCyclerSprite : public CBaseEntity
{
public:
	void Spawn();
	void Think();
	void Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	virtual int	ObjectCaps() { return (CBaseEntity::ObjectCaps() | FCAP_DONT_SAVE | FCAP_IMPULSE_USE); }
	virtual int	TakeDamage(entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType);
	void	Animate(float frames);

	virtual int		Save(CSave &save);
	virtual int		Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	inline int		ShouldAnimate() { return m_animate && m_maxFrame > 1.0; }
	int			m_animate;
	float		m_lastTime;
	float		m_maxFrame;
};