#define _ARMA_

class CfgPatches
{
	class BBB
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts"};
	};
};


class CfgMods
{
	class BBB
	{
		type = "mod";
		author = "DaveZ";
		name = "BBB";	
		dependencies[] = {"World","Mission"};
		class defs
		{
			class worldScriptModule
			{
				files[] = {"BBB/scripts/4_World"};
			};
			class missionScriptModule
			{
				files[] = {"BBB/scripts/5_Mission"};
			};
		};
	};
};

