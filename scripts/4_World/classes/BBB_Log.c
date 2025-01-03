//! DayZ Mod : DaveZ's Better Burning Barrels (AKA BBB)
//! 
//! Visit the discord server to report bugs, ask questions or just say hello.
//! 	https://discord.gg/AqDzhMJr
//! 
//!   ******** Repacking is not permitted without prior approval ********
//!
//! DO NOT call methods starting with _ from user code.
//!
//! Logging...
class BBB_Log
{
    //--------------------------------------------------------------------------
	//! Write to server ServerProfiles\script*.log
    //! \param note \p string
    /**
        @code
            BBB_Log.Log("[DZS] Hello");
        @endcode
    */
    static void Log(string note)
    {
        int year, month, day, hour, min, sec;

        GetHourMinuteSecond(hour, min, sec);
        GetYearMonthDay(year, month, day);

        Print("[BBB] " + day.ToStringLen(2) + "/" + month.ToStringLen(2) + "/" + year.ToStringLen(2) + " " + hour.ToStringLen(2) + ":" + min.ToStringLen(2) + ":" + sec.ToStringLen(2) + "  " + note);
    }

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    static bool s_bDebugging;

    //--------------------------------------------------------------------------
	//! DO NOT CALL - This method is to be used internally.
    static void DebugMode(int d)
    {
        if(d) {
            s_bDebugging = true;
        } else {
            s_bDebugging = false;
        }
    }

    //--------------------------------------------------------------------------
	//! Write to server ServerProfiles\script*.log only if DebugBarrels is set
    //! \param note \p string
    /**
        @code
            BBB_LogEx.LogEx("Extra Debugging info");
        @endcode
    */
    static void LogEx(string note)
    {
        if(s_bDebugging) Log(note);
    }
}