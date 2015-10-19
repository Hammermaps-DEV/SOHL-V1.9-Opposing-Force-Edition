#ifndef PIDDRONESPLIT_H
#define PIDDRONESPLIT_H

class CPitDroneSpit : public CBaseEntity {
public:
	void Spawn(void);
	void Precache(void);
	void EXPORT BubbleThink(void);
	void EXPORT SpitTouch(CBaseEntity *pOther);

	int Classify(void) { return	CLASS_ALIEN_BIOWEAPON; }
	static CPitDroneSpit *SpitCreate(void);
};

#endif // PIDDRONESPLIT_H
