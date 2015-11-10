#ifndef GONOMESPLIT_H
#define GONOMESPLIT_H

class CGonomeSpit : public CBaseEntity {
public:
	void Spawn(void);
	void Precache(void);
	static void Shoot(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
	void Touch(CBaseEntity *pOther);
	void EXPORT Animate(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	int iGonomeSpitSprite;
	int  m_maxFrame;
};

#endif // GONOMESPLIT_H
