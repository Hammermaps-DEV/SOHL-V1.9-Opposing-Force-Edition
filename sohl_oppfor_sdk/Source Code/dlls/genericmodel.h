class CGenericModel : public CBaseAnimating
{
public:

	virtual void Spawn(void);
	virtual void Precache(void);
	virtual void KeyValue(KeyValueData* pkvd);

	virtual void EXPORT IdleThink(void);

	virtual int	Save(CSave &save);
	virtual int	Restore(CRestore &restore);
	static	TYPEDESCRIPTION m_SaveData[];

	char RandomMDL[100];
	string_t m_iszSequence;
};