//
// A gib is a chunk of a body, or a piece of wood/metal/rocks/etc.
//
class CGib : public CBaseEntity
{
public:
	void Spawn(const char *szGibModel);
	void EXPORT BounceGibTouch(CBaseEntity *pOther);
	void EXPORT StickyGibTouch(CBaseEntity *pOther);
	void EXPORT WaitTillLand();
	void		LimitVelocity();

	virtual int	ObjectCaps() { return (CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE; }
	static	void SpawnHeadGib(entvars_t *pevVictim);
	static	void SpawnHeadGib(entvars_t *pevVictim, const char *szGibModel);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int human);
	static	void SpawnRandomGibs(entvars_t *pevVictim, int cGibs, int notfirst, const char *szGibModel); //LRC
	static  void SpawnStickyGibs(entvars_t *pevVictim, Vector vecOrigin, int cGibs);

	int		m_bloodColor;
	int		m_cBloodDecals;
	int		m_material;
	float	m_lifeTime;
};