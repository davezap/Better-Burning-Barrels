modded class Fireplace
{
    private bool m_bBetterBurningBarrel = false;

    bool GetBetterBurningBarrel() { return m_bBetterBurningBarrel; }
    void SetBetterBurningBarrel() { m_bBetterBurningBarrel = true; }

    override bool IsKindling(ItemBase item) {
        return super.IsKindling(item);
    }

    override bool IsFuel(ItemBase item) {
        return super.IsFuel(item);
    }

    //// Our Stuff /////////////////////////////////////////////////////////////

    void SetTripod(bool state)
    {
        //m_bTripod = state;
    }

    bool GetTripod()
    {
        //return m_bTripod;
        return false;
    }

    void SetCircle(bool state)
    {
        //m_bTripod = state;
    }

    bool GetCircle()
    {
        //return m_bTripod;
        return false;
    }
}