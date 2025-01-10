modded class Fireplace
{
    private bool m_bBetterBurningBarrel = false;
    private bool m_bPleaseNoHurty = false;
    bool GetBetterBurningBarrel() { return m_bBetterBurningBarrel; }
    void SetBetterBurningBarrel() { m_bBetterBurningBarrel = true; }


    override bool IsKindling(ItemBase item) {
        return super.IsKindling(item);
    }

    override bool IsFuel(ItemBase item) {
        return super.IsFuel(item);
    }

    override void CreateAreaDamage()
    {
        if(m_bPleaseNoHurty) {
            DestroyAreaDamage();
        } else {
            super.CreateAreaDamage();
        }
    }

    //// Our Stuff /////////////////////////////////////////////////////////////

    void SetTripod(bool state)
    {
        //Tripod = state;
    }

    bool GetTripod()
    {
        //return Tripod;
        return false;
    }

    void SetCircle(bool state)
    {
        //Tripod = state;
    }

    bool GetCircle()
    {
        //return Tripod;
        return false;
    }

    void DisableDamage(bool damage)
    {
        m_bPleaseNoHurty = damage;
    }
}