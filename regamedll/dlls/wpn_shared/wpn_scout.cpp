#include "precompiled.h"

LINK_ENTITY_TO_CLASS(weapon_scout, CSCOUT)

void CSCOUT::Spawn()
{
	Precache();

	m_iId = WEAPON_M200;
	SET_MODEL(edict(), "models/w_scout.mdl");

	m_iDefaultAmmo = iinfo()->m_iMaxClip;

	// Get ready to fall down
	FallInit();

	// extend
	CBasePlayerWeapon::Spawn();
}

void CSCOUT::Precache()
{
	PRECACHE_MODEL("models/v_scout.mdl");
	PRECACHE_MODEL("models/w_scout.mdl");

	PRECACHE_SOUND("weapons/scout_fire-1.wav");
	PRECACHE_SOUND("weapons/scout_bolt.wav");
	PRECACHE_SOUND("weapons/scout_clipin.wav");
	PRECACHE_SOUND("weapons/scout_clipout.wav");
	PRECACHE_SOUND("weapons/zoom.wav");

	m_iShellId = m_iShell = PRECACHE_MODEL("models/rshell_big.mdl");
	m_usFireScout = PRECACHE_EVENT(1, "events/scout.sc");
}

BOOL CSCOUT::Deploy()
{
	if (DefaultDeploy("models/v_scout.mdl", "models/p_scout.mdl", SCOUT_DRAW, "rifle", UseDecrement() != FALSE))
	{
		m_flNextPrimaryAttack = m_pPlayer->m_flNextAttack = GetNextAttackDelay(1.25);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0f;

		return TRUE;
	}

	return FALSE;
}

void CSCOUT::SecondaryAttack()
{
	switch (m_pPlayer->m_iFOV)
	{
	case 90: m_pPlayer->m_iFOV = m_pPlayer->pev->fov = 40; break;
	case 40: m_pPlayer->m_iFOV = m_pPlayer->pev->fov = 15; break;
	default: m_pPlayer->m_iFOV = m_pPlayer->pev->fov = 90; break;
	}

	if (TheBots)
	{
		TheBots->OnEvent(EVENT_WEAPON_ZOOMED, m_pPlayer);
	}

	m_pPlayer->ResetMaxSpeed();
	EMIT_SOUND(m_pPlayer->edict(), CHAN_ITEM, "weapons/zoom.wav", 0.2, 2.4);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.3;
}

void CSCOUT::PrimaryAttack()
{
	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		SCOUTFire(0.2, 1.25, FALSE);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 170)
	{
		SCOUTFire(0.075, 1.25, FALSE);
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING)
	{
		SCOUTFire(0, 1.25, FALSE);
	}
	else
	{
		SCOUTFire(0.007, 1.25, FALSE);
	}
}

void CSCOUT::SCOUTFire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	Vector vecAiming, vecSrc, vecDir;
	int flag;

	if (m_pPlayer->pev->fov != DEFAULT_FOV)
	{
		m_pPlayer->m_bResumeZoom = true;
		m_pPlayer->m_iLastZoom = m_pPlayer->m_iFOV;

		// reset a fov
		m_pPlayer->m_iFOV = DEFAULT_FOV;
	}
	else
	{
		flSpread += 0.025f;
	}

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = GetNextAttackDelay(0.2);
		}

		if (TheBots)
		{
			TheBots->OnEvent(EVENT_WEAPON_FIRED_ON_EMPTY, m_pPlayer);
		}

		return;
	}

	m_iClip--;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_flEjectBrass = gpGlobals->time + 0.56f;
	m_pPlayer->m_iWeaponVolume = BIG_EXPLOSION_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	vecSrc = m_pPlayer->GetGunPosition();
	vecAiming = gpGlobals->v_forward;

	float flBaseDamage = SCOUT_DAMAGE;
	vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, 8192, 3, BULLET_PLAYER_762MM, flBaseDamage, SCOUT_RANGE_MODIFER, m_pPlayer->pev, true, m_pPlayer->random_seed);

#ifdef CLIENT_WEAPONS
	flag = FEV_NOTHOST;
#else
	flag = 0;
#endif

	PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usFireScout, 0, (float *)&g_vecZero, (float *)&m_pPlayer->pev->angles, (vecDir.x * 1000), (vecDir.y * 1000),
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.x * 100), FALSE, FALSE);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.8f;
	m_pPlayer->pev->punchangle.x -= 2.0f;
}

void CSCOUT::Reload()
{
	if (DefaultReload(iinfo()->m_iMaxClip, SCOUT_RELOAD, SCOUT_RELOAD_TIME))
	{
		if (m_pPlayer->pev->fov != DEFAULT_FOV)
		{
			m_pPlayer->pev->fov = m_pPlayer->m_iFOV = 15;
			SecondaryAttack();
		}

		m_pPlayer->SetAnimation(PLAYER_RELOAD);
	}
}

void CSCOUT::WeaponIdle()
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
	{
		return;
	}

	if (m_iClip)
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 60.0f;
		SendWeaponAnim(SCOUT_IDLE, UseDecrement() != FALSE);
	}
}

float CSCOUT::GetMaxSpeed()
{
	return (m_pPlayer->m_iFOV == DEFAULT_FOV) ? SCOUT_MAX_SPEED : SCOUT_MAX_SPEED_ZOOM;
}
