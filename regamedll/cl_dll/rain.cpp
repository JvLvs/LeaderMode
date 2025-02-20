/***
*
*	Copyright (c) 2005, BUzer.
*
*	Used with permission for Spirit of Half-Life 1.5
*
****/
/*
====== rain.cpp ========================================================
*/

#include "precompiled.h"

#define DRIPSPEED		900		// speed of raindrips (pixel per secs)
#define SNOWSPEED		200		// speed of snowflakes
#define SNOWFADEDIST	500

#define MAXDRIPS 2000	// max raindrops
#define MAXFX    3000	// max effects

#define DRIP_SPRITE_HALFHEIGHT	64
#define DRIP_SPRITE_HALFWIDTH	1
#define SNOW_SPRITE_HALFSIZE	3
#define SPLASH_SPRITE_HALFSIZE	7

// radius water rings
#define MAXRINGHALFSIZE	25

enum class Weather : BYTE
{
	RAIN = 0U,
	SNOW,
};

struct
{
	Vector2D	m_vWindSpeed{ Vector2D(60) };
	Vector2D	m_vRandom{ Vector2D(10) };

	float	m_flSpawnRadius{ 2000 };

	int		m_iDripsPerSecond{ 1500 };
	Weather	m_uiType{ 0 };

	double	m_flCurTime{ 0 };    // current time
	double	m_flOldTime{ 0 };    // last time we have updated drips
	double	m_flTimeDelta{ 0 };  // difference between old time and current time
	double	m_flNextSpawnTime{ 0 };  // when the next drip should be spawned

	hSprite	m_hsprRain{ 0 };
	hSprite	m_hsprSnow{ 0 };
	hSprite	m_hsprRipple{ 0 };
	hSprite	m_hsprSplash1{ 0 };
	hSprite	m_hsprSplash2{ 0 };
	hSprite	m_hsprSplash3{ 0 };

} Rain;

enum class Landing : BYTE
{
	NONE = 0U,
	SPLASH,
	RIPPLE,
	SNOW,
};

struct cl_drip_t
{
	Vector		origin;
	float		birthTime;
	float		minHeight;	// minimal height to kill raindrop
	float		alpha;

	Vector2D	Delta; // side speed
	Landing		land;
};

std::list<cl_drip_t> g_lstDrips;

struct cl_rainfx_t
{
	Vector	origin;
	float	birthTime;
	float	life;
	float	alpha;
	hSprite	hSPR;

	Landing	type;
};

std::list<cl_rainfx_t> g_lstRainFx;


#ifdef _DEBUG
cvar_t* debug_rain = NULL;
#endif

cvar_t* cl_weather = nullptr;
static bool rain_initialized = false;

/*
=================================
WaterLandingEffect
=================================
*/
void LandingEffect(cl_drip_t& drip)
{
	if (drip.land == Landing::NONE)
		return;

	if (g_lstRainFx.size() >= MAXFX)
	{
		//gEngfuncs.Con_Printf( "Rain error: FX limit overflow!\n" );
		return;
	}

	cl_rainfx_t newFX;
	newFX.origin = drip.origin;
	newFX.origin.z = drip.minHeight; // correct position
	newFX.birthTime = Rain.m_flCurTime;
	newFX.type = drip.land;

	switch (newFX.type)
	{
	case Landing::RIPPLE:
		newFX.alpha = gEngfuncs.pfnRandomFloat(0.6, 0.9);
		newFX.hSPR = Rain.m_hsprRipple;
		newFX.life = gEngfuncs.pfnRandomFloat(0.7, 1);
		break;

	case Landing::SPLASH:
		newFX.alpha = gEngfuncs.pfnRandomFloat(0.8, 1);	//gEngfuncs.pfnRandomFloat(0.6, 0.9);
		newFX.hSPR = Rain.m_hsprSplash3;//RANDOM_LONG(0, 1) ? Rain.m_hsprSplash1 : Rain.m_hsprSplash2;
		newFX.life = gEngfuncs.pfnRandomFloat(0.3, 0.5);//gEngfuncs.pfnRandomFloat(0.2, 0.4);
		break;

	case Landing::SNOW:
		newFX.alpha = drip.alpha;	// Inherit the alpha.
		newFX.hSPR = Rain.m_hsprSnow;
		newFX.life = gEngfuncs.pfnRandomFloat(1, 1.5);
		break;

	default:
		return;
	}

	// add to first place in chain
	g_lstRainFx.push_back(newFX);
}

/*
=================================
ProcessRain

Must think every frame.
=================================
*/
void ProcessRain(void)
{
	int speed = Rain.m_uiType == Weather::SNOW ? SNOWSPEED : DRIPSPEED;

	Rain.m_flOldTime = Rain.m_flCurTime; // save old time
	Rain.m_flCurTime = g_flClientTime;
	Rain.m_flTimeDelta = g_flClientTimeDelta;

	cl_weather->value = Q_atof(cl_weather->string);

	if (cl_weather->value > 3.0f)
		gEngfuncs.Cvar_Set("cl_weather", "3");

	if (Rain.m_iDripsPerSecond == 0 || !cl_weather->value)
		return; // disabled

	// first frame
	if (Rain.m_flOldTime == 0 || (Rain.m_iDripsPerSecond == 0 && g_lstDrips.empty()))
	{
		// fix first frame bug with nextspawntime
		Rain.m_flNextSpawnTime = Rain.m_flCurTime;
		return;
	}

	if (!Rain.m_flTimeDelta)
		return; // not in pause

	unsigned spawnDrips = (unsigned)round(Rain.m_iDripsPerSecond * cl_weather->value);
	double timeBetweenDrips = 1.0 / (double)(spawnDrips);

#ifdef _DEBUG
	// save debug info
	float debug_lifetime = 0;
	int debug_howmany = 0;
	int debug_attempted = 0;
	int debug_dropped = 0;
#endif

	auto curDrip = g_lstDrips.begin();
	while (curDrip != g_lstDrips.end())
	{
		curDrip->origin.x += Rain.m_flTimeDelta * curDrip->Delta.x;
		curDrip->origin.y += Rain.m_flTimeDelta * curDrip->Delta.y;
		curDrip->origin.z -= Rain.m_flTimeDelta * speed;

		// remove drip if its origin lower than minHeight
		if (curDrip->origin.z < curDrip->minHeight)
		{
			LandingEffect(*curDrip);
#ifdef _DEBUG
			if (debug_rain->value)
			{
				debug_lifetime += (Rain.m_flCurTime - curDrip->birthTime);
				debug_howmany++;
			}
#endif

			curDrip = g_lstDrips.erase(curDrip);
		}
		else
			curDrip++;
	}

	int maxDelta = speed * Rain.m_flTimeDelta; // maximum height randomize distance

	for (; Rain.m_flNextSpawnTime < Rain.m_flCurTime; Rain.m_flNextSpawnTime += timeBetweenDrips)
	{
#ifdef _DEBUG
		if (debug_rain->value)
			debug_attempted++;
#endif

		if (g_lstDrips.size() < spawnDrips) // check for overflow
		{
			float deathHeight;
			Vector vecStart, vecEnd;
			Vector2D Delta(Rain.m_vWindSpeed.x + gEngfuncs.pfnRandomFloat(Rain.m_vRandom.x * -1, Rain.m_vRandom.x),
				Rain.m_vWindSpeed.y + gEngfuncs.pfnRandomFloat(Rain.m_vRandom.y * -1, Rain.m_vRandom.y));
			pmtrace_t pmtrace;

			// First trace: find the hightest point above player.
			vecStart.x = gEngfuncs.pfnRandomFloat(gHUD::m_vecOrigin.x - Rain.m_flSpawnRadius, gHUD::m_vecOrigin.x + Rain.m_flSpawnRadius);
			vecStart.y = gEngfuncs.pfnRandomFloat(gHUD::m_vecOrigin.y - Rain.m_flSpawnRadius, gHUD::m_vecOrigin.y + Rain.m_flSpawnRadius);
			vecStart.z = gHUD::m_vecOrigin.z;

			vecEnd = vecStart;
			vecEnd.z = g_lstDrips.size() < spawnDrips - 10 ? RANDOM_FLOAT(vecStart.z + PM_VEC_VIEW + SNOWFADEDIST, vecStart.z + 1500) : 9999;

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(vecStart, vecEnd, PM_WORLD_ONLY, -1, &pmtrace);

			// Second trace: Check that player have a real sky above him
			const char* s = gEngfuncs.pEventAPI->EV_TraceTexture(pmtrace.ent, vecStart, vecEnd);
			if ((!s || strcmp(s, "sky")) && pmtrace.fraction < 1)
			{
#ifdef _DEBUG
				if (debug_rain->value)
					debug_dropped++;
#endif
				continue;
			}

			vecStart = pmtrace.endpos;
			vecStart.z -= 1;

			// find a point at bottom of map
			vecEnd = vecStart + Vector(Delta) * 5;
			vecEnd.z = -4096;

			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(vecStart, vecEnd, PM_WORLD_ONLY, -1, &pmtrace);

			if (pmtrace.startsolid)
			{
#ifdef _DEBUG
				if (debug_rain->value)
					debug_dropped++;
#endif
				continue; // drip cannot be placed
			}

			// falling to water?
			int contents = gEngfuncs.PM_PointContents(pmtrace.endpos, NULL);
			if (contents == CONTENTS_WATER)
			{
				int waterEntity = gEngfuncs.PM_WaterEntity(pmtrace.endpos);
				if (waterEntity > 0)
				{
					cl_entity_t* pwater = gEngfuncs.GetEntityByIndex(waterEntity);
					if (pwater && (pwater->model != NULL))
					{
						deathHeight = pwater->curstate.maxs[2];
					}
					else
					{
						gEngfuncs.Con_Printf("Rain error: can't get water entity\n");
						continue;
					}
				}
				else
				{
					gEngfuncs.Con_Printf("Rain error: water is not func_water entity\n");
					continue;
				}
			}
			else
			{
				deathHeight = pmtrace.endpos[2];
			}

			// just in case..
			if (deathHeight > vecStart[2])
			{
				gEngfuncs.Con_Printf("Rain error: can't create drip in water\n");
				continue;
			}

			//CBaseParticle* p = new CBaseParticle();
			//Q_strlcpy(p->m_szClassname, "rain_particle");
			//p->InitializeSprite(vecStart, Vector(0, 0, -1), (model_t*)gEngfuncs.GetSpritePointer(Rain.m_hsprRain), 2, 255);
			//p->m_flStretchY = 40.0f;
			//p->m_vVelocity = Vector(Delta, -900);
			//p->m_flGravity = 1;
			//p->m_flMass = 10;
			//p->SetCollisionFlags(TRI_COLLIDEWORLD | TRI_COLLIDEKILL | TRI_WATERTRACE);
			//p->SetCullFlag(CULL_PVS | CULL_FRUSTUM_PLANE);
			////p->SetRenderFlag(RENDER_FACEPLAYER_ROTATEZ);
			//p->SetLightFlag(LIGHT_NONE);
			////g_pParticleMan->SetRender(kRenderTransAdd);
			//p->m_iRendermode = kRenderTransAdd;
			//p->m_iFrame = 0;
			//p->m_iFramerate = 1.0;
			//p->m_iNumFrames = gEngfuncs.GetSpritePointer(Rain.m_hsprSplash1)->numframes;

			cl_drip_t newClDrip;
			vecStart.z -= gEngfuncs.pfnRandomFloat(0, maxDelta); // randomize a bit
			newClDrip.alpha = gEngfuncs.pfnRandomFloat(0.12, 0.2);
			newClDrip.origin = vecStart;
			newClDrip.Delta = Delta;
			newClDrip.birthTime = Rain.m_flCurTime; // store time when it was spawned
			newClDrip.minHeight = deathHeight;

			if (contents == CONTENTS_WATER)
			{
				newClDrip.land = Landing::RIPPLE;
			}
			else if (Rain.m_uiType == Weather::RAIN)	// Splash for rain only.
			{
				newClDrip.land = Landing::SPLASH;
			}
			else if (Rain.m_uiType == Weather::SNOW)	// Snow would stay on ground for a little more.
			{
				newClDrip.land = Landing::SNOW;
			}
			else
			{
				newClDrip.land = Landing::NONE;
			}

			// add to first place in chain
			g_lstDrips.push_back(newClDrip);
		}
		else
		{
			//gEngfuncs.Con_Printf( "Rain error: Drip limit overflow!\n" );
			break;
		}
	}

#ifdef _DEBUG
	if (debug_rain->value) // print debug info
	{
		con_nprint_t info =
		{
			1,
			0.5f,
			{1.0f, 0.6f, 0.0f }
		};
		gEngfuncs.Con_NXPrintf(&info, "Rain info: Drips exist: %i\n", g_lstDrips.size());

		info.index = 2;
		gEngfuncs.Con_NXPrintf(&info, "Rain info: FX's exist: %i\n", g_lstRainFx.size());

		info.index = 3;
		gEngfuncs.Con_NXPrintf(&info, "Rain info: Attempted/Dropped: %i, %i\n", debug_attempted, debug_dropped);

		if (debug_howmany)
		{
			float ave = debug_lifetime / (float)debug_howmany;

			info.index = 4;
			gEngfuncs.Con_NXPrintf(&info, "Rain info: Average drip life time: %f\n", ave);
		}
	}
#endif
}

/*
=================================
ProcessFXObjects

Remove all fx objects with out time to live
Call every frame before ProcessRain
=================================
*/
void ProcessFXObjects(void)
{
	for (auto curFX = g_lstRainFx.begin(); curFX != g_lstRainFx.end();)
	{
		// delete current?
		if (curFX->birthTime + curFX->life < Rain.m_flCurTime)
		{
			curFX = g_lstRainFx.erase(curFX);
		}
		else
			curFX++;
	}
}

/*
=================================
ResetRain

clear memory, delete all objects
=================================
*/
void ResetRain(void)
{
	// delete all drips
	g_lstDrips.clear();

	// delete all FX objects
	g_lstRainFx.clear();

	InitRain();
	return;
}

void Rain_MsgFunc_ReceiveW(int iWeatherType)
{
	if (iWeatherType == 0)
	{
		ResetRain();
		return;
	}

	Rain.m_flSpawnRadius = 1800;
	Rain.m_iDripsPerSecond = 1500;
	Rain.m_vWindSpeed.x = Rain.m_vWindSpeed.y = 40;
	Rain.m_vRandom.x = Rain.m_vRandom.y = 10;
	Rain.m_uiType = Weather(iWeatherType - 1);
}

/*
=================================
InitRain
initialze system
=================================
*/
void InitRain(void)
{
	memset(&Rain, NULL, sizeof(Rain));

	if (!rain_initialized)
	{
		rain_initialized = true;

		cl_weather = CVAR_CREATE("cl_weather", "1", FCVAR_ARCHIVE);

#ifdef _DEBUG
		debug_rain = CVAR_CREATE("Rain.debug.InfoDisplay", "0", 0);
		gEngfuncs.pfnAddCommand("Rain.debug.Set", []() { Rain_MsgFunc_ReceiveW(Q_atoi(gEngfuncs.Cmd_Argv(1))); });
#endif // _DEBUG
	}

	Rain.m_hsprRain = gEngfuncs.pfnSPR_Load("sprites/effects/rain.spr");
	Rain.m_hsprSnow = gEngfuncs.pfnSPR_Load("sprites/effects/snowflake.spr");
	Rain.m_hsprRipple = gEngfuncs.pfnSPR_Load("sprites/effects/ripple.spr");
	Rain.m_hsprSplash1 = gEngfuncs.pfnSPR_Load("sprites/VFX/rain_splash_01.spr");
	Rain.m_hsprSplash2 = gEngfuncs.pfnSPR_Load("sprites/VFX/rain_splash_02.spr");
	Rain.m_hsprSplash3 = gEngfuncs.pfnSPR_Load("sprites/wsplash3.spr");	// Original CS1.6/CZ splash.
}

void SetPoint(float x, float y, float z, float(*matrix)[4])
{
	Vector point(x, y, z), result;

	VectorTransform(point, matrix, result);

	gEngfuncs.pTriAPI->Vertex3fv(result);
}

/*
=================================
DrawRain

draw raindrips and snowflakes
=================================
*/
void DrawRain(void)
{
	if (g_lstDrips.empty())
		return; // no drips to draw

	cl_entity_t* player = gEngfuncs.GetLocalPlayer();

	if (Rain.m_uiType == Weather::RAIN) // draw rain
	{
		const model_s* pTexture = gEngfuncs.GetSpritePointer(Rain.m_hsprRain);
		gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)pTexture, 0);
		gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
		gEngfuncs.pTriAPI->CullFace(TRI_NONE);

		for (auto Drip = g_lstDrips.begin(); Drip != g_lstDrips.end(); ++Drip)
		{
			Vector2D toPlayer, shift(Drip->Delta * DRIP_SPRITE_HALFHEIGHT / DRIPSPEED);
			toPlayer.x = (player->origin.x - Drip->origin.x) * DRIP_SPRITE_HALFWIDTH;
			toPlayer.y = (player->origin.y - Drip->origin.y) * DRIP_SPRITE_HALFWIDTH;
			toPlayer = toPlayer.Normalize();

			// --- draw triangle --------------------------
			gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, Drip->alpha);
			gEngfuncs.pTriAPI->Begin(TRI_TRIANGLES);

			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			gEngfuncs.pTriAPI->Vertex3f(Drip->origin.x - toPlayer.y - shift.x,
				Drip->origin.y + toPlayer.x - shift.y,
				Drip->origin.z + DRIP_SPRITE_HALFHEIGHT);

			gEngfuncs.pTriAPI->TexCoord2f(0.5, 1);
			gEngfuncs.pTriAPI->Vertex3f(Drip->origin.x + shift.x,
				Drip->origin.y + shift.y,
				Drip->origin.z - DRIP_SPRITE_HALFHEIGHT);

			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			gEngfuncs.pTriAPI->Vertex3f(Drip->origin.x + toPlayer.y - shift.x,
				Drip->origin.y - toPlayer.x - shift.y,
				Drip->origin.z + DRIP_SPRITE_HALFHEIGHT);

			gEngfuncs.pTriAPI->End();
			// --- draw triangle end ----------------------
		}
	}

	else	// draw snow
	{
		const model_s* pTexture = gEngfuncs.GetSpritePointer(Rain.m_hsprSnow);
		float visibleHeight = gHUD::m_vecOrigin.z + PM_VEC_VIEW + SNOWFADEDIST;
		vec3_t normal;
		float  matrix[3][4];

		gEngfuncs.GetViewAngles(normal);
		AngleMatrix(normal, matrix);	// calc view matrix

		gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)pTexture, 0);
		gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
		gEngfuncs.pTriAPI->CullFace(TRI_NONE);

		for (auto Drip = g_lstDrips.begin(); Drip != g_lstDrips.end(); ++Drip)
		{
			matrix[0][3] = Drip->origin.x; // write origin to matrix
			matrix[1][3] = Drip->origin.y;
			matrix[2][3] = Drip->origin.z;

			// apply start fading effect
			float alpha = (Drip->origin.z <= visibleHeight) ?
				Drip->alpha :
				Drip->alpha - (Drip->origin.z - visibleHeight) * 0.001;

			if (alpha <= FLT_EPSILON)
				continue;

			// --- draw quad --------------------------
			gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, alpha);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);

			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			SetPoint(0, SNOW_SPRITE_HALFSIZE, SNOW_SPRITE_HALFSIZE, matrix);

			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			SetPoint(0, SNOW_SPRITE_HALFSIZE, -SNOW_SPRITE_HALFSIZE, matrix);

			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			SetPoint(0, -SNOW_SPRITE_HALFSIZE, -SNOW_SPRITE_HALFSIZE, matrix);

			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			SetPoint(0, -SNOW_SPRITE_HALFSIZE, SNOW_SPRITE_HALFSIZE, matrix);

			gEngfuncs.pTriAPI->End();
			// --- draw quad end ----------------------
		}
	}
}

/*
=================================
DrawFXObjects
=================================
*/
void DrawFXObjects(void)
{
	vec3_t normal;
	float  matrix[3][4], alpha, size;

	gEngfuncs.GetViewAngles(normal);
	AngleMatrix(normal, matrix);	// calc view matrix

	// go through objects list
	for (auto curFX = g_lstRainFx.begin(); curFX != g_lstRainFx.end(); ++curFX)
	{
		switch (curFX->type)
		{
		case Landing::RIPPLE:
		{
			gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)gEngfuncs.GetSpritePointer(curFX->hSPR), 0);
			gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			// fadeout
			alpha = ((curFX->birthTime + curFX->life - Rain.m_flCurTime) / curFX->life) * curFX->alpha;
			size = (Rain.m_flCurTime - curFX->birthTime) * MAXRINGHALFSIZE;

			// --- draw quad --------------------------
			gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, alpha);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);

			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin.x - size, curFX->origin.y - size, curFX->origin.z);

			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin.x - size, curFX->origin.y + size, curFX->origin.z);

			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin.x + size, curFX->origin.y + size, curFX->origin.z);

			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			gEngfuncs.pTriAPI->Vertex3f(curFX->origin.x + size, curFX->origin.y - size, curFX->origin.z);

			gEngfuncs.pTriAPI->End();
			// --- draw quad end ----------------------
			break;
		}

		case Landing::SPLASH:
		{
			auto pModel = gEngfuncs.GetSpritePointer(curFX->hSPR);
			auto lifePercentage = (curFX->birthTime + curFX->life - Rain.m_flCurTime) / curFX->life;
			//gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)pModel, (int)round(lifePercentage * pModel->numframes * 5) % pModel->numframes);	// play the SPR anim.
			gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)pModel, (int)round(lifePercentage * pModel->numframes));	// play the SPR anim.
			gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			matrix[0][3] = curFX->origin.x; // write origin to matrix
			matrix[1][3] = curFX->origin.y;
			matrix[2][3] = curFX->origin.z;

			// fadeout
			alpha = lifePercentage * curFX->alpha;

			// --- draw quad --------------------------
			gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, alpha);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);

			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			SetPoint(0, SPLASH_SPRITE_HALFSIZE, 2 * SPLASH_SPRITE_HALFSIZE, matrix);	// the splash effect must be entirely drew on-ground.

			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			SetPoint(0, SPLASH_SPRITE_HALFSIZE, 0, matrix);

			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			SetPoint(0, -SPLASH_SPRITE_HALFSIZE, 0, matrix);

			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			SetPoint(0, -SPLASH_SPRITE_HALFSIZE, 2 * SPLASH_SPRITE_HALFSIZE, matrix);

			gEngfuncs.pTriAPI->End();
			// --- draw quad end ----------------------
			break;
		}

		case Landing::SNOW:
		{
			gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)gEngfuncs.GetSpritePointer(curFX->hSPR), 0);
			gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
			gEngfuncs.pTriAPI->CullFace(TRI_NONE);

			matrix[0][3] = curFX->origin.x; // write origin to matrix
			matrix[1][3] = curFX->origin.y;
			matrix[2][3] = curFX->origin.z;

			// fadeout
			alpha = ((curFX->birthTime + curFX->life - Rain.m_flCurTime) / curFX->life) * curFX->alpha;

			// --- draw quad --------------------------
			gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, alpha);
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);

			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			SetPoint(0, SNOW_SPRITE_HALFSIZE, 2 * SNOW_SPRITE_HALFSIZE, matrix);	// the snow effect must be entirely drew on-ground.

			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			SetPoint(0, SNOW_SPRITE_HALFSIZE, 0, matrix);

			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			SetPoint(0, -SNOW_SPRITE_HALFSIZE, 0, matrix);

			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			SetPoint(0, -SNOW_SPRITE_HALFSIZE, 2 * SNOW_SPRITE_HALFSIZE, matrix);

			gEngfuncs.pTriAPI->End();
			// --- draw quad end ----------------------
			break;
		}

		default:
			break;
		}
	}
}
