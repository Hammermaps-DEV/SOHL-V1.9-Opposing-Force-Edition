//Weapon Cycler
class CWeaponCycler : public CBasePlayerWeapon
{
public:
	void Spawn() override;
	int GetItemInfo(ItemInfo *p) override;

	void PrimaryAttack() override;
	void SecondaryAttack() override;
	BOOL Deploy() override;
	void Holster() override;
	void KeyValue(KeyValueData *pkvd) override;

	int Save(CSave &save) override;
	int Restore(CRestore &restore) override;
	static	TYPEDESCRIPTION m_SaveData[];
private:
	string_t	m_iPlayerModel;
	string_t	m_iWorldModel;
	string_t	m_iViewModel;
};