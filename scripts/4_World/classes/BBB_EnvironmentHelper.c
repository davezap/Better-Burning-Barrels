//! DayZ Mod : DaveZ's Better Burning Barrels (AKA BBB)
//! 
//! Visit the discord server to report bugs, ask questions or just say hello.
//! 	https://discord.gg/AqDzhMJr
//! 
//!   ******** Repacking is not permitted without prior approval ********
//!
//! DO NOT call methods starting with _ from user code.
//!
//! EnviromentHelper provides date,time,temperature functions.
class BBB_EnvironmentHelper
{
    /**
    \brief Returns DayZ date float
        \return \p float DayZ date float
        @code
            float date = BBB_EH.GetGameDateF();

            >> date = 7.625
        @endcode
    */
    static float GetGameDateF()
    {
        float year, month, day;
        GetGameDateInts(year, month, day);
        return month + ( day / 32.0 );
    }

    /**
    \brief Returns DayZ time float
        \return \p float DayZ time float
        @code
            float date = BBB_EH.GetGameTimeF();

            >> date = 15.3833...
        @endcode
    */
    static float GetGameTimeF()
    {
        int hour, min;
        GetGameTimeInts(hour, min);
        return hour + (min / 60);
    }

    /**
    \brief Returns world date as year, month and day ints
        \param[out] year \p int Year
        \param[out] month \p int Month
        \param[out] day \p int Day
        \return \p void
        @code
            int year, month, day;
            BBB_EH.GetGameDateInts(year, month, day);

            >> year = 23
            >> month = 7
            >> day = 20
        @endcode
    */
    static void GetGameDateInts(out int year, out int month, out int day)
    {
        int hour=0, minute=0;
        GetGame().GetWorld().GetDate( year, month, day, hour, minute );
    }

    /**
    \brief Returns world time as hour and minute ints
        \param[out] hour \p int Hour
        \param[out] min \p int Minute
        \return \p void
        @code
            int hour, min;
            BBB_EH.GetGameTimeInts(hour, min);
           
            >> hour = 15
            >> min = 23
        @endcode
    */
    static void GetGameTimeInts(out int hour, out int min)
    {
        int year, month, day;
        GetGame().GetWorld().GetDate( year, month, day, hour, min );
    }

    /**
    \brief Returns world time as Unix timestamp 

    Note: DayZ Date and Time defaults can be set in mission init.c
        \return \p string Unix timestamp
        @code            
            int unix = BBB_EH.GetGameTimeUnix();

            >> unix = 1690125810
        @endcode
    */
    static int GetGameTimeUnix()
    {
        int year, month, day, hour, minute;
        GetGameDateInts(year, month, day);
        GetGameTimeInts(hour, minute);
        return IntsToUnix(year, month, day, hour, minute, 0);
    }

    /**
    \brief Returns world time as TF datetime
        \return \p string TF format datetime
        @code
            string date = BBB_EH.GetGameTimeTF();

            >> date = "20/07/23 15:23:30"
        @endcode
    */
    static string GetGameTimeTF()
    {
        int year, month, day, hour, minute;
        GetGameDateInts(year, month, day);
        GetGameTimeInts(hour, minute);
        return Ints2TF(year, month, day, hour, minute, 0);
    }

    /**
    \brief Sets world time by TF datetime
        \param tf \p string TF format datetime
        \return \p void
        @code
            BBB_EH.SetGameTime("20/07/23 15:23:30");
        @endcode
    */
    static void SetGameTime(string tf)
    {
        int year, month, day, hour, min, sec; 
        TF2Ints(tf, year, month, day, hour, min, sec);
        GetGame().GetWorld().SetDate(year, month, day, hour, min);
    }


    /**
    \brief Converts TF datetime to Ints
        \param tf \p string TimeFormat
        \param[out] year \p int Year
        \param[out] month \p int Month
        \param[out] day \p int Day
        \param[out] hour \p int Hour
        \param[out] min \p int Minute
        \param[out] sec \p int Second
        \return \p void
        @code
            int year, month, day, hour, min, sec;
            BBB_EH.TF2Ints("20/07/23 15:23:30", year, month, day, hour, min, sec);

            >> year = 23
            >> month = 07
            >> day = 20
            >> hour = 15
            >> min = 23
            >> sec = 30
        @endcode
    */
    static void TF2Ints(string tf, out int year, out int month, out int day, out int hour, out int min, out int sec)
    {
       string token[10];
       int result = tf.ParseString(token);
        
        if(token[1]=="/")
        {
            day = token[0].ToInt();
            month = token[2].ToInt();
            year = token[4].ToInt();
            if(token[4].Length()==2)
            {
                year += 2000;
            }
            
            if(token[6]==":")
            {
                hour = token[5].ToInt();
                min = token[7].ToInt();
                if(token[8] == ":")
                {
                    sec = token[9].ToInt();
                }
            }
        } else if(token[1]==":") {
            hour = token[0].ToInt();
            min = token[2].ToInt();
            if(token[3]==":")
            {
                sec = token[2].ToInt();
            }
        }
    }

    /**
    \brief Converts Ints to TF datetime
        \param tf \p string TimeFormat
        \param[out] year \p int Year
        \param[out] month \p int Month
        \param[out] day \p int Day
        \param[out] hour \p int Hour
        \param[out] min \p int Minute
        \param[out] sec \p int Second
        \return \p string TF format datetime
        @code
            string date = BBB_EH.Ints2TF(2023, 7, 20, 15, 23, 30);

            >> date = "20/07/23 15:23:30"
        @endcode
    */
    static string Ints2TF(int year, int month, int day, int hour, int min, int sec)
    {
        return YMD2TF(year, month, day) + " " + HMS2TF(hour, min, sec);
    }

    /**
    \brief Converts year, month and day ints to TF datetime
        \param year \p int Year
        \param month \p int Month
        \param day \p int Day
        \return \p string TF format datetime
        @code
            string date = BBB_EH.YMD2TF(2023, 07, 20);

            >> date = "20/07/23"
        @endcode
    */
    static string YMD2TF(int year, int month, int day)
    {
        return day.ToStringLen(2) + "/" + month.ToStringLen(2) + "/" + year.ToStringLen(2);
    }

    /**
    \brief Converts hour, minute and second ints to TF datetime
        \param hour \p int Hour
        \param min \p int Minute
        \param sec \p int Second
        \return \p string TF format datetime
        @code
            string date = BBB_EH.HMS2TF(15, 23, 30);

            >> date = "15:23:30"
        @endcode
    */
    static string HMS2TF(int hour, int min, int sec)
    {
        if(sec >= 0)
        {
            return hour.ToStringLen(2) + ":" + min.ToStringLen(2) + ":" + sec.ToStringLen(2);
        }
        return hour.ToStringLen(2) + ":" + min.ToStringLen(2);
    }

    /**
    \brief Converts TF datetime format to DayZ time float

    Calculated as hour + (min / 60) + (sec / 3600 ). 
    Used in our configs to express time of day for OnTimeFrom and OnTimeTo options.
    DayZ does not give seconds but we compute it for completeness.
        \param ft \p string TF format datetime
        \return \p float DayZ time
        @code
            float time = BBB_EH.TF2F("20/07/23 15:23:30");

            >> time = 15.3833....

            time = BBB_EH.TF2F("15:23:30");

            >> time = 15.3833....

            time = BBB_EH.TF2F("23:15");

            >> time = 23.25
        @endcode
    */
    static float TF2F(string tf)
    {
        string token[10];
        int hour, min, sec; 
        int result = tf.ParseString(token);

        if(token[6]==":")
        {
            hour = token[5].ToInt();
            min = token[7].ToInt();
            if(token[8]==":")
            {
                sec = token[9].ToInt();
            }
        } else {
            hour = token[0].ToInt();
            min = token[2].ToInt();
            if(token[3]==":")
            {
                sec = token[4].ToInt();
            }
        }
        return hour + (min / 60) + (sec / 3600 );
    }

    /**
    \brief Returns true if the current game datetime is between From and To.

    Does not accept date only TF strings like "20/07/23" you must provide the full
    string including date and time.
    Tests if game time is >= tfFrom and < tfTo
        \param tfFrom \p string TF format datetime
        \param tfTo \p string TF format datetime
        \return \p bool
        @code
            if(BBB_EH.GameTimeBetween("20/07/23 12:50:59", "25/07/23 12:50"))
            {
                BBB_Log.LogEx("Is Between");
            }
        @endcode
    */
    static bool GameTimeBetween(string tfFrom, string tfTo)
    {
        int gameTime = GetGameTimeUnix();
        int from = TF2Unix(tfFrom);
        int to = TF2Unix(tfTo);
        //BBB_Log.LogEx("From " + from + " Game " + gameTime + " To " + to);
        if(from <= to)
        {
            if(from <= gameTime && gameTime < to) return true;
        } else {
            if(to <= gameTime && gameTime < from) return true;
        }
        return false;
    }

    /**
    \brief Returns true if the current game datetime is between From and To.

    This is the Unix overload version.

    Tests if game time is >= tfFrom and < tfTo
        \param unixFrom \p int Unix timestamp
        \param unixTo \p int Unix timestamp
        \return \p bool
        @code
            if(BBB_EH.GameTimeBetween(1690116659, 1690289400))
            {
                BBB_Log.LogEx("Is Between");
            }
        @endcode
    */
    static bool GameTimeBetween(int unixFrom, int unixTo)
    {
        int gameTime = GetGameTimeUnix();
        int from = unixFrom;
        int to = unixTo;
        //BBB_Log.LogEx("From " + from + " Game " + gameTime + " To " + to);
        if(from <= to)
        {
            if(from <= gameTime && gameTime < to) return true;
        } else {
            if(to <= gameTime && gameTime < from) return true;
        }
        return false;
    }

    /**
    \brief Converts Ints to Unix timestamp
        \param year \p int Year
        \param month \p int Month
        \param day \p int Day
        \param hour \p int Hour
        \param min \p int Minute
        \param sec \p int Second
        \return \p int Unix timestamp
        @code
            int epoch = BBB_EH.IntsToUnix(2023, 7, 20, 15, 23, 30);

            >> epoch = 1689866610
        @endcode
    */
    static int IntsToUnix(int year, int month, int day, int hour, int min, int sec)
    {
        // shortcut for times.
        if(!year || !month || !day)
        {
            return hour * 3600 + min * 60 + sec;
        }


        // With thanks https://gist.github.com/orca-zhang/bfd4a08dbf9a99b4130ffee2cb70a0eb
        int daysFromYearStart[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
        int leap = 0;
        if ((year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) && month >= 3)
        {
            leap = 1; // February 29
        }

        //BBB_Log.Log(YMD2TF(year, month, day) + " " + HMS2TF(hour, min, sec));
        //BBB_Log.Log("Year=" + year);
        
        //Enforce ints are 32 bit so from −2,147,483,648 to +2,147,483,647
        return ((365*year - 719528 + day - 1 + (year+3)/4 - (year+99)/100 + (year+399)/400 + daysFromYearStart[month - 1] + leap) * 86400 + hour * 3600 + min * 60 + sec);
    }

    /**
    \brief Converts TF format datetime to Unix timestamp
        \param tf \p string TF format datetime
        \return \p int Unix timestamp
        @code
            int epoch = BBB_EH.TF2Unix("20/07/23 15:23:30");

            >> epoch = 1689866610
        @endcode
    */
    static int TF2Unix(string tf)
    {
 
        int year, month, day, hour, min, sec; 
        TF2Ints(tf, year, month, day, hour, min, sec);
        return IntsToUnix(year, month, day, hour, min, sec);
    }
    
    /**
    \brief Returns server time as Unix timestamp
        \return \p int Unix timestamp
        @code
            int epoch = BBB_EH.GetServerTime();

            >> epoch = 1689866610
        @endcode
    */
    static int GetServerTime()
    {
        int year, month, day, hour, min, sec; 
        GetHourMinuteSecondUTC(hour, min, sec); 
        GetYearMonthDay(year, month, day);
        return IntsToUnix(year, month, day, hour, min, sec);
    }


    /**
    \brief Returns world Sunrise time as DayZ time float.
        \return \p float DayZ time.
        @code
            int time = BBB_EH.GetSunriseTimeF();

            >> time = 7.0
        @endcode
    */
    static float GetSunriseTimeF()
    {
        int year, month, day;
        GetGameDateInts(year, month, day);
        return GetGame().GetMission().GetWorldData().GetApproxSunriseTime(month + ( day / 32.0 ));
    }

    /**
    \brief Returns world Sunset time as DayZ time float.
        \return \p float DayZ time.
        @code
            int time = BBB_EH.GetSunsetTimeF();

            >> time = 17.0
        @endcode
    */
    static float GetSunsetTimeF()
    {
        int year, month, day;
        GetGameDateInts(year, month, day);
        return GetGame().GetMission().GetWorldData().GetApproxSunsetTime(month + ( day / 32.0 ));
    }


    /**
    \brief Returns month and day ints from DayZ date float.
        \param[out] month \p int Month
        \param[out] day \p int Day
        \return \p float DayZ time.
        @code
            int month, day;
            BBB_EH.DateFtoInts(15.3833, month, day);

            >> month = 7
            >> day = 20
        @endcode
    */
    static void DateFtoInts(float monthDayF, out int month, out int day)
    {
        month = Math.Floor(monthDayF);
        day = Math.Floor((monthDayF - month)*32);
    }

    /**
    \brief Returns month and day ints from DayZ date float.
        \param[out] hour \p int Hour
        \param[out] min \p int Minute
        \return \p float DayZ time.
        @code
            int hour, min;
            BBB_EH.TimeFtoInts(15.3833, hour, min);

            >> hour = 15
            >> min = 23
        @endcode
    */
    static void TimeFtoInts(float timeF, out int hour, out int min)
    {
        hour = Math.Floor(timeF);
        min = Math.Round((timeF - hour)*60);
    }

    /**
    \brief Returns true if game time is between sunrise and sunset
        \return \p bool DayZ day time.
    */
    static bool IsDay() 
    {
        return (GetSunriseTimeF() < GetGameTimeF()) && (GetGameTimeF() < GetSunsetTimeF());
    }


    /**
    \brief Prints a bunch of EnvironmentHelper info to log
        \return \p void
    */
    static void Debug()
    {
        int sunriseHour, sunriseMin;
        TimeFtoInts(GetSunriseTimeF(), sunriseHour, sunriseMin);
        int sunsetHour, sunsetMin;
        TimeFtoInts(GetSunsetTimeF(), sunsetHour, sunsetMin);
        float temp = GetTemp();
        bool daytime = IsDay();
        int year, month, day, hour, minute;
        GetGameDateInts(year, month, day);
        GetGameTimeInts(hour, minute);
        string line = "Sunrise " + sunriseHour + ":" + sunriseMin + ", Sunset " + sunsetHour + ":" + sunsetMin;
        line += ", Temp " + temp + ", DateTime " + year + " " + month + " " + day + " " + hour + ":" + minute + " daytime=" + daytime;
        BBB_Log.Log(line);
	}


    //// Weather ///////////////////////////////////////////////////////////////

    /**
    \brief Returns curent game temperature in degrees celsius.
        \return \p float Temperature (c)
    */
    static float GetTemp()
    {
        return GetGame().GetMission().GetWorldData().GetBaseEnvTemperature();
    }

    
    static bool IsWorldLoaded()
    {     
        vector from	= "3685.851 500.0 5983.208";
        vector to = {from[0], 0, from[2]};
        vector contact_pos;
        vector contact_norm;
        Object ignoreObj;
        Object hitObject;        
        float fraction;
        
        // PhxInteractionLayers.CHARACTER | PhxInteractionLayers.ITEM_SMALL | PhxInteractionLayers.DOOR | PhxInteractionLayers.ITEM_LARGE  | PhxInteractionLayers.AI
        PhxInteractionLayers hit_mask = PhxInteractionLayers.BUILDING | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.FENCE;

        if(DayZPhysics.RayCastBullet(from, to, hit_mask, ignoreObj, hitObject, contact_pos, contact_norm, fraction))
        {        
            //BBB_Log.Log("here=" + here + " contact_pos=" + contact_pos);
            if( contact_pos && contact_pos[1]!=0 )
            {
                BBB_Log.LogEx("World Loaded: YES");
                return true;
            }
        }

        BBB_Log.LogEx("World Loaded: NO");
        return false;
    }

    static float GetGroundBelow(vector here)
    {
        // on top of office [3706.100098, 405.365662, 6006.825684]
        // inside building [3686.253906, 402.691589, 5983.330566]
        // up tower [3714.787109, 444.497467, 5989.814453]
        // top of building [3682.552979, 410.952850, 5994.813965]

        if(here[1] == 0)
        {
            here[1] = GetGame().SurfaceY( here[0], here[2] ) + 1000;
        } else {
            here[1] = here[1] + 0.5;
        }

        vector from	= here;
        vector to = {here[0], 0, here[2]};
        vector contact_pos;
        vector contact_norm;
        Object ignoreObj;
        Object hitObject;        
        float fraction;
        
        // PhxInteractionLayers.CHARACTER | PhxInteractionLayers.ITEM_SMALL | PhxInteractionLayers.DOOR | PhxInteractionLayers.ITEM_LARGE  | PhxInteractionLayers.AI
        PhxInteractionLayers hit_mask = PhxInteractionLayers.BUILDING | PhxInteractionLayers.VEHICLE | PhxInteractionLayers.ROADWAY | PhxInteractionLayers.TERRAIN | PhxInteractionLayers.FENCE;

        if(DayZPhysics.RayCastBullet(from, to, hit_mask, ignoreObj, hitObject, contact_pos, contact_norm, fraction))
        {        
            //BBB_Log.Log("here=" + here + " contact_pos=" + contact_pos);
            if( contact_pos && contact_pos[1]!=0 )
            {
                return contact_pos[1];// - 0.121094;   // Why does raycast hit this much above surface?
            }
        }
        //BBB_Log.Log("here=" + here + " contact_pos=0");
        // If all else fails just stick it on the ground.
        return GetGame().SurfaceY( here[0], here[2] );

    }
}

// Short alias
class BBB_EH: BBB_EnvironmentHelper
{
    
}