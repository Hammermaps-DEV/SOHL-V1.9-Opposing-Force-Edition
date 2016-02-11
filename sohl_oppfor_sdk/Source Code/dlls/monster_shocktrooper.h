class CStrooper : public CHGrunt
{
public:
	void Spawn(void);
	void Precache(void);
	int  Classify(void);
	void HandleAnimEvent(MonsterEvent_t *pEvent);

	void SetActivity(Activity NewActivity);

	void DeathSound(void);
	void PainSound(void);
	void IdleSound(void);
	void GibMonster(void);

	int	Save(CSave &save);
	int Restore(CRestore &restore);

	Schedule_t	*GetSchedule(void);
	Schedule_t  *GetScheduleOfType(int Type);

	static TYPEDESCRIPTION m_SaveData[];

	BOOL m_fRightClaw;

	static const char *pGruntSentences[];
};