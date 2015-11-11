#ifndef PIDDRONESPLIT_H
#define PIDDRONESPLIT_H

class CPitDroneSpit : public CBaseMonster
{
public:
	void Spawn(void);
	void Precache(void);
	int	 Classify(void);
	int  IRelationship(CBaseEntity *pTarget);

	void IgniteTrail(void);
	void EXPORT StartTrack(void);
	void EXPORT StartDart(void);
	void EXPORT TrackTarget(void);
	void EXPORT TrackTouch(CBaseEntity *pOther);
	void EXPORT DieTouch(CBaseEntity *pOther);

	static const char *pHitSounds[];
	static const char *pMissSounds[];

	int iDroneSpitTrail;
};

#endif // PIDDRONESPLIT_H
