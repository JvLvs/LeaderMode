#include "precompiled.h"

cvar_t *g_pskill          = nullptr;
cvar_t *g_psv_gravity     = nullptr;
cvar_t *g_psv_aim         = nullptr;
cvar_t *g_footsteps       = nullptr;
cvar_t *g_psv_accelerate  = nullptr;
cvar_t *g_psv_friction    = nullptr;
cvar_t *g_psv_stopspeed   = nullptr;
cvar_t *g_psv_stepsize    = nullptr;
cvar_t *g_psv_clienttrace = nullptr;

cvar_t displaysoundlist      = { "displaysoundlist", "0", 0, 0.0f, nullptr };
cvar_t timelimit             = { "mp_timelimit", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t decalfrequency        = { "decalfrequency", "30", FCVAR_SERVER, 0.0f, nullptr };
cvar_t fadetoblack           = { "mp_fadetoblack", "0", FCVAR_SERVER, 0.0f, nullptr };

cvar_t fragsleft             = { "mp_fragsleft", "0", FCVAR_SERVER | FCVAR_UNLOGGED, 0.0f, nullptr };		// Don't spam console/log files/users with this changing
cvar_t timeleft              = { "mp_timeleft", "0", FCVAR_SERVER | FCVAR_UNLOGGED, 0.0f, nullptr };

cvar_t friendlyfire          = { "mp_friendlyfire", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t infiniteAmmo          = { "mp_infinite_ammo", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t infiniteGrenades      = { "mp_infinite_grenades", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t allowmonsters         = { "mp_allowmonsters", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t roundtime             = { "mp_roundtime", "5", FCVAR_SERVER, 0.0f, nullptr };
cvar_t freezetime            = { "mp_freezetime", "6", FCVAR_SERVER, 0.0f, nullptr };
cvar_t ghostfrequency        = { "mp_ghostfrequency", "0.1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t autokick              = { "mp_autokick", "1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t autokick_timeout      = { "mp_autokick_timeout", "-1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t restartround          = { "sv_restartround", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t sv_restart            = { "sv_restart", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t limitteams            = { "mp_limitteams", "2", FCVAR_SERVER, 0.0f, nullptr };
cvar_t autoteambalance       = { "mp_autoteambalance", "1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t tkpunish              = { "mp_tkpunish", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t hostagepenalty        = { "mp_hostagepenalty", "13", FCVAR_SERVER, 0.0f, nullptr };
cvar_t mirrordamage          = { "mp_mirrordamage", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t logmessages           = { "mp_logmessages", "1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t forcecamera           = { "mp_forcecamera", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t forcechasecam         = { "mp_forcechasecam", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t mapvoteratio          = { "mp_mapvoteratio", "0.66", FCVAR_SERVER, 0.0f, nullptr };
cvar_t logdetail             = { "mp_logdetail", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t startmoney            = { "mp_startmoney", "800", FCVAR_SERVER, 0.0f, nullptr };
cvar_t maxrounds             = { "mp_maxrounds", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t winlimit              = { "mp_winlimit", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t windifference         = { "mp_windifference", "1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t playerid              = { "mp_playerid", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t allow_spectators      = { "allow_spectators", "1.0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t mp_chattime           = { "mp_chattime", "10", FCVAR_SERVER, 0.0f, nullptr };
cvar_t kick_percent          = { "mp_kickpercent", "0.66", FCVAR_SERVER, 0.0f, nullptr };
cvar_t humans_join_team      = { "humans_join_team", "any", FCVAR_SERVER, 0.0f, nullptr };
cvar_t sk_plr_9mm_bullet1    = { "sk_plr_9mm_bullet1", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mm_bullet2    = { "sk_plr_9mm_bullet2", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mm_bullet3    = { "sk_plr_9mm_bullet3", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_357_bullet1    = { "sk_plr_357_bullet1", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_357_bullet2    = { "sk_plr_357_bullet2", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_357_bullet3    = { "sk_plr_357_bullet3", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mmAR_bullet1  = { "sk_plr_9mmAR_bullet1", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mmAR_bullet2  = { "sk_plr_9mmAR_bullet2", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mmAR_bullet3  = { "sk_plr_9mmAR_bullet3", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mmAR_grenade1 = { "sk_plr_9mmAR_grenade1", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mmAR_grenade2 = { "sk_plr_9mmAR_grenade2", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_9mmAR_grenade3 = { "sk_plr_9mmAR_grenade3", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_buckshot1      = { "sk_plr_buckshot1", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_buckshot2      = { "sk_plr_buckshot2", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_buckshot3      = { "sk_plr_buckshot3", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_rpg1           = { "sk_plr_rpg1", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_rpg2           = { "sk_plr_rpg2", "0", 0, 0.0f, nullptr };
cvar_t sk_plr_rpg3           = { "sk_plr_rpg3", "0", 0, 0.0f, nullptr };
cvar_t sk_12mm_bullet1       = { "sk_12mm_bullet1", "0", 0, 0.0f, nullptr };
cvar_t sk_12mm_bullet2       = { "sk_12mm_bullet2", "0", 0, 0.0f, nullptr };
cvar_t sk_12mm_bullet3       = { "sk_12mm_bullet3", "0", 0, 0.0f, nullptr };
cvar_t sk_9mmAR_bullet1      = { "sk_9mmAR_bullet1", "0", 0, 0.0f, nullptr };
cvar_t sk_9mmAR_bullet2      = { "sk_9mmAR_bullet2", "0", 0, 0.0f, nullptr };
cvar_t sk_9mmAR_bullet3      = { "sk_9mmAR_bullet3", "0", 0, 0.0f, nullptr };
cvar_t sk_9mm_bullet1        = { "sk_9mm_bullet1", "0", 0, 0.0f, nullptr };
cvar_t sk_9mm_bullet2        = { "sk_9mm_bullet2", "0", 0, 0.0f, nullptr };
cvar_t sk_9mm_bullet3        = { "sk_9mm_bullet3", "0", 0, 0.0f, nullptr };
cvar_t sk_suitcharger1       = { "sk_suitcharger1", "0", 0, 0.0f, nullptr };
cvar_t sk_suitcharger2       = { "sk_suitcharger2", "0", 0, 0.0f, nullptr };
cvar_t sk_suitcharger3       = { "sk_suitcharger3", "0", 0, 0.0f, nullptr };
cvar_t sk_battery1           = { "sk_battery1", "0", 0, 0.0f, nullptr };
cvar_t sk_battery2           = { "sk_battery2", "0", 0, 0.0f, nullptr };
cvar_t sk_battery3           = { "sk_battery3", "0", 0, 0.0f, nullptr };
cvar_t sk_healthcharger1     = { "sk_healthcharger1", "0", 0, 0.0f, nullptr };
cvar_t sk_healthcharger2     = { "sk_healthcharger2", "0", 0, 0.0f, nullptr };
cvar_t sk_healthcharger3     = { "sk_healthcharger3", "0", 0, 0.0f, nullptr };
cvar_t sk_healthkit1         = { "sk_healthkit1", "0", 0, 0.0f, nullptr };
cvar_t sk_healthkit2         = { "sk_healthkit2", "0", 0, 0.0f, nullptr };
cvar_t sk_healthkit3         = { "sk_healthkit3", "0", 0, 0.0f, nullptr };
cvar_t sk_scientist_heal1    = { "sk_scientist_heal1", "0", 0, 0.0f, nullptr };
cvar_t sk_scientist_heal2    = { "sk_scientist_heal2", "0", 0, 0.0f, nullptr };
cvar_t sk_scientist_heal3    = { "sk_scientist_heal3", "0", 0, 0.0f, nullptr };
cvar_t scoreboard_showmoney  = { "mp_scoreboard_showmoney", "3", FCVAR_SERVER, 0.0f, nullptr };
cvar_t scoreboard_showhealth = { "mp_scoreboard_showhealth", "3", FCVAR_SERVER, 0.0f, nullptr };

cvar_t game_version          = { "game_version", APP_VERSION, FCVAR_SERVER, 0.0f, nullptr };
cvar_t maxmoney              = { "mp_maxmoney", "16000", FCVAR_SERVER, 0.0f, nullptr };
cvar_t round_infinite        = { "mp_round_infinite", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t hegrenade_penetration = { "mp_hegrenade_penetration", "0", 0, 0.0f, nullptr };
cvar_t nadedrops             = { "mp_nadedrops", "0", 0, 0.0f, nullptr };
cvar_t playerrespawn_time     = { "lm_playerrespawn_basetime", "10", 0, 10.0f, nullptr };
cvar_t auto_reload_weapons   = { "mp_auto_reload_weapons", "0", 0, 0.0f, nullptr };
cvar_t refill_bpammo_weapons = { "mp_refill_bpammo_weapons", "0", 0, 0.0f, nullptr }; // Useful for mods like DeathMatch, GunGame, ZombieMod etc
cvar_t freeforall            = { "mp_freeforall", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t auto_join_team        = { "mp_auto_join_team", "0", 0, 0.0f, nullptr };
cvar_t max_teamkills         = { "mp_max_teamkills", "3", 0, 3.0f, nullptr };
cvar_t fraglimit             = { "mp_fraglimit", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t round_restart_delay   = { "mp_round_restart_delay", "5", FCVAR_SERVER, 0.0f, nullptr };
cvar_t showtriggers          = { "showtriggers", "0", 0, 0.0f, nullptr }; // debug cvar shows triggers
                                                                          // TODO: Maybe it's better to register in the engine?
cvar_t roundover                    = { "mp_roundover", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t show_radioicon               = { "mp_show_radioicon", "1", 0, 1.0f, nullptr };
cvar_t old_bomb_defused_sound       = { "mp_old_bomb_defused_sound", "1", 0, 1.0f, nullptr };
cvar_t item_staytime                = { "mp_item_staytime", "300", FCVAR_SERVER, 300.0f, nullptr };
cvar_t legacy_bombtarget_touch      = { "mp_legacy_bombtarget_touch", "1", 0, 1.0f, nullptr };
cvar_t respawn_immunitytime         = { "mp_respawn_immunitytime", "0", FCVAR_SERVER, 0.0f, nullptr };
cvar_t respawn_immunity_effects     = { "mp_respawn_immunity_effects", "1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t respawn_immunity_force_unset = { "mp_respawn_immunity_force_unset", "1", FCVAR_SERVER, 0.0f, nullptr };
cvar_t kill_filled_spawn            = { "mp_kill_filled_spawn", "1", 0, 0.0f, nullptr };
cvar_t weapons_allow_map_placed     = { "mp_weapons_allow_map_placed", "1", FCVAR_SERVER, 0.0f, nullptr };

cvar_t allow_point_servercommand    = { "mp_allow_point_servercommand", "0", 0, 0.0f, nullptr };
cvar_t hullbounds_sets              = { "mp_hullbounds_sets", "1", 0, 0.0f, nullptr };
cvar_t unduck_method                = { "mp_unduck_method", "0", 0, 0.0f, nullptr };

cvar_t ff_damage_reduction_bullets      = { "ff_damage_reduction_bullets",      "0.35", FCVAR_SERVER, 0.0f, nullptr };
cvar_t ff_damage_reduction_grenade      = { "ff_damage_reduction_grenade",      "0.25", FCVAR_SERVER, 0.0f, nullptr };
cvar_t ff_damage_reduction_grenade_self = { "ff_damage_reduction_grenade_self", "1",    FCVAR_SERVER, 0.0f, nullptr };
cvar_t ff_damage_reduction_other        = { "ff_damage_reduction_other",        "0.25", FCVAR_SERVER, 0.0f, nullptr };

cvar_t radio_timeout           = { "mp_radio_timeout", "1.5", FCVAR_SERVER, 1.5f, nullptr };
cvar_t radio_maxinround        = { "mp_radio_maxinround", "60", FCVAR_SERVER, 60.0f, nullptr };

cvar_t clientcvar_query_interval	= { "mp_client_cvar_query_inv", "10", FCVAR_SERVER, 10.0f, nullptr };

// new cvars from leader mod.
cvar_t commander_maxhealth		= { "lm_commander_maxhealth", "1000", FCVAR_SERVER, 1000.0f, nullptr };
cvar_t godfather_maxhealth		= { "lm_godfather_maxhealth", "1000", FCVAR_SERVER, 1000.0f, nullptr };
cvar_t human_role_priority		= { "lm_human_role_priority", "1", FCVAR_SERVER, 1.0f, nullptr };
cvar_t menpower_per_player		= { "lm_menpower_per_player", "1", FCVAR_SERVER, 1.0f, nullptr };
cvar_t ballot_boxes_opening_interval = { "lm_ballot_boxes_opening_interval", "20.0", FCVAR_SERVER, 20.0f, nullptr };
cvar_t swat_max_armour			= { "lm_swat_max_armour", "200", FCVAR_SERVER, 200.0f, nullptr };


void GameDLL_Version_f()
{
	if (Q_stricmp(CMD_ARGV(1), "version") != 0)
		return;

	// print version
	CONSOLE_ECHO("ReGameDLL version: " APP_VERSION "\n");
	CONSOLE_ECHO("Build date: " APP_COMMIT_TIME " " APP_COMMIT_DATE "\n");
}

void GameDLL_EndRound_f()
{
	if (CMD_ARGC() == 2)
	{
		const char *pCmd = CMD_ARGV(1);

		if (pCmd[0] == '1' || !Q_stricmp(pCmd, "T"))
		{
			CSGameRules()->OnRoundEnd(WINSTATUS_TERRORISTS, ROUND_TERRORISTS_WIN, CSGameRules()->GetRoundRestartDelay());
			return;
		}
		else if (pCmd[0] == '2' || !Q_stricmp(pCmd, "CT"))
		{
			CSGameRules()->OnRoundEnd(WINSTATUS_CTS, ROUND_CTS_WIN, CSGameRules()->GetRoundRestartDelay());
			return;
		}
	}

	CSGameRules()->OnRoundEnd(WINSTATUS_DRAW, ROUND_END_DRAW, CSGameRules()->GetRoundRestartDelay());
}

void GameDLL_SwapTeams_f()
{
	CSGameRules()->SwapAllPlayers();
	CVAR_SET_FLOAT("sv_restartround", 1.0);
}

void EXT_FUNC GameDLLInit()
{
	g_pskill          = CVAR_GET_POINTER("skill");
	g_psv_gravity     = CVAR_GET_POINTER("sv_gravity");
	g_psv_aim         = CVAR_GET_POINTER("sv_aim");
	g_footsteps       = CVAR_GET_POINTER("mp_footsteps");
	g_psv_accelerate  = CVAR_GET_POINTER("sv_accelerate");
	g_psv_friction    = CVAR_GET_POINTER("sv_friction");
	g_psv_stopspeed   = CVAR_GET_POINTER("sv_stopspeed");
	g_psv_stepsize    = CVAR_GET_POINTER("sv_stepsize");
	g_psv_clienttrace = CVAR_GET_POINTER("sv_clienttrace");

	CVAR_REGISTER(&displaysoundlist);
	CVAR_REGISTER(&timelimit);
	CVAR_REGISTER(&friendlyfire);
	CVAR_REGISTER(&infiniteAmmo);
	CVAR_REGISTER(&infiniteGrenades);

	CVAR_REGISTER(&decalfrequency);

	CVAR_REGISTER(&roundtime);
	CVAR_REGISTER(&freezetime);

	CVAR_REGISTER(&autokick);
	CVAR_REGISTER(&autokick_timeout);
	CVAR_REGISTER(&restartround);
	CVAR_REGISTER(&sv_restart);
	CVAR_REGISTER(&limitteams);
	CVAR_REGISTER(&autoteambalance);
	CVAR_REGISTER(&tkpunish);
	CVAR_REGISTER(&hostagepenalty);
	CVAR_REGISTER(&mirrordamage);
	CVAR_REGISTER(&logmessages);
	CVAR_REGISTER(&forcecamera);
	CVAR_REGISTER(&forcechasecam);
	CVAR_REGISTER(&mapvoteratio);
	CVAR_REGISTER(&maxrounds);
	CVAR_REGISTER(&winlimit);
	CVAR_REGISTER(&windifference);
	CVAR_REGISTER(&fadetoblack);
	CVAR_REGISTER(&logdetail);
	CVAR_REGISTER(&startmoney);
	CVAR_REGISTER(&playerid);
	CVAR_REGISTER(&allow_spectators);
	CVAR_REGISTER(&mp_chattime);
	CVAR_REGISTER(&kick_percent);
	CVAR_REGISTER(&fragsleft);
	CVAR_REGISTER(&timeleft);
	CVAR_REGISTER(&humans_join_team);
	CVAR_REGISTER(&scoreboard_showhealth);
	CVAR_REGISTER(&scoreboard_showmoney);

	ADD_SERVER_COMMAND("game", GameDLL_Version_f);
	ADD_SERVER_COMMAND("endround", GameDLL_EndRound_f);
	ADD_SERVER_COMMAND("mp_swapteams", GameDLL_SwapTeams_f);

	CVAR_REGISTER(&game_version);
	CVAR_REGISTER(&maxmoney);
	CVAR_REGISTER(&round_infinite);
	CVAR_REGISTER(&hegrenade_penetration);
	CVAR_REGISTER(&nadedrops);
	CVAR_REGISTER(&playerrespawn_time);
	CVAR_REGISTER(&auto_reload_weapons);
	CVAR_REGISTER(&refill_bpammo_weapons);
	CVAR_REGISTER(&freeforall);
	CVAR_REGISTER(&auto_join_team);
	CVAR_REGISTER(&max_teamkills);
	CVAR_REGISTER(&fraglimit);
	CVAR_REGISTER(&round_restart_delay);

	CVAR_REGISTER(&showtriggers);
	CVAR_REGISTER(&roundover);
	CVAR_REGISTER(&show_radioicon);

	CVAR_REGISTER(&old_bomb_defused_sound);
	CVAR_REGISTER(&item_staytime);
	CVAR_REGISTER(&legacy_bombtarget_touch);
	CVAR_REGISTER(&respawn_immunitytime);
	CVAR_REGISTER(&respawn_immunity_effects);
	CVAR_REGISTER(&respawn_immunity_force_unset);
	CVAR_REGISTER(&kill_filled_spawn);
	CVAR_REGISTER(&allow_point_servercommand);
	CVAR_REGISTER(&hullbounds_sets);
	CVAR_REGISTER(&unduck_method);
	CVAR_REGISTER(&weapons_allow_map_placed);

	CVAR_REGISTER(&ff_damage_reduction_bullets);
	CVAR_REGISTER(&ff_damage_reduction_grenade);
	CVAR_REGISTER(&ff_damage_reduction_grenade_self);
	CVAR_REGISTER(&ff_damage_reduction_other);

	CVAR_REGISTER(&radio_timeout);
	CVAR_REGISTER(&radio_maxinround);

	CVAR_REGISTER(&clientcvar_query_interval);

	CVAR_REGISTER(&commander_maxhealth);
	CVAR_REGISTER(&godfather_maxhealth);
	CVAR_REGISTER(&human_role_priority);
	CVAR_REGISTER(&menpower_per_player);
	CVAR_REGISTER(&ballot_boxes_opening_interval);
	CVAR_REGISTER(&swat_max_armour);

	// print version
	CONSOLE_ECHO("ReGameDLL version: " APP_VERSION "\n");

	Bot_RegisterCVars();

	VoiceGameMgr_RegisterCVars();

	// execute initial pre-configurations
	SERVER_COMMAND("exec game_init.cfg\n");
	SERVER_EXECUTE();
}
