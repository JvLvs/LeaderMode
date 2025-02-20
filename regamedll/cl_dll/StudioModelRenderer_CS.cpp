/*

Created Date: 07 Mar 2020

*/

#include "precompiled.h"

engine_studio_api_t IEngineStudio;

CGameStudioModelRenderer g_StudioRenderer;

float g_flTimeViewModelAnimStart = 0.0f;	// LUNA: stupit engine. You can't just override g_pViewEnt->curstate.animtime anywhere, you must override it right before it's render.
float g_flViewModelFramerate = 1.0f;	// LUNA: same as above.
float g_flStartScaleTime;
int iPrevRenderState;
int iRenderStateChanged;
cl_entity_t* g_pViewEnt = nullptr;
Vector g_rgvecViewModelAttachments[4];

typedef struct
{
	Vector origin;
	Vector angles;

	Vector realangles;

	float animtime;
	float frame;
	int sequence;
	int gaitsequence;
	float framerate;

	int m_fSequenceLoops;
	int m_fSequenceFinished;

	byte controller[4];
	byte blending[2];

	latchedvars_t lv;
} client_anim_state_t;

static client_anim_state_t g_state;
static client_anim_state_t g_clientstate;

const char* s_boneNames[BONE_MAX] =
{
	"Bip01 Head",
	"Bip01 Pelvis",
	"Bip01 Spine1",
	"Bip01 Spine2",
	"Bip01 Spine3"
};

// ===================== Local Funcs =====================

int g_rseq;
int g_gaitseq;
Vector g_clorg;
Vector g_clang;

void CounterStrike_GetSequence(int* seq, int* gaitseq)
{
	*seq = g_rseq;
	*gaitseq = g_gaitseq;
}

void CounterStrike_SetSequence(int seq, int gaitseq)
{
	g_rseq = seq;
	g_gaitseq = gaitseq;
}

void CounterStrike_GetOrientation(float* o, float* a)
{
	VectorCopy(g_clorg, o);
	VectorCopy(g_clang, a);
}

void CounterStrike_SetOrientation(float* o, float* a)
{
	VectorCopy(o, g_clorg);
	VectorCopy(a, g_clang);
}

void GetSequenceInfo(void* pmodel, client_anim_state_t* pev, float* pflFrameRate, float* pflGroundSpeed)
{
	studiohdr_t* pstudiohdr;
	pstudiohdr = (studiohdr_t*)pmodel;

	if (!pstudiohdr)
		return;

	mstudioseqdesc_t* pseqdesc;

	if (pev->sequence >= pstudiohdr->numseq)
	{
		*pflFrameRate = 0.0;
		*pflGroundSpeed = 0.0;
		return;
	}

	pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	if (pseqdesc->numframes > 1)
	{
		*pflFrameRate = 256 * pseqdesc->fps / (pseqdesc->numframes - 1);
		*pflGroundSpeed = Q_sqrt(pseqdesc->linearmovement[0] * pseqdesc->linearmovement[0] + pseqdesc->linearmovement[1] * pseqdesc->linearmovement[1] + pseqdesc->linearmovement[2] * pseqdesc->linearmovement[2]);
		*pflGroundSpeed = *pflGroundSpeed * pseqdesc->fps / (pseqdesc->numframes - 1);
	}
	else
	{
		*pflFrameRate = 256.0;
		*pflGroundSpeed = 0.0;
	}
}

int GetSequenceFlags(void* pmodel, client_anim_state_t* pev)
{
	studiohdr_t* pstudiohdr;
	pstudiohdr = (studiohdr_t*)pmodel;

	if (!pstudiohdr || pev->sequence >= pstudiohdr->numseq)
		return 0;

	mstudioseqdesc_t* pseqdesc;
	pseqdesc = (mstudioseqdesc_t*)((byte*)pstudiohdr + pstudiohdr->seqindex) + (int)pev->sequence;

	return pseqdesc->flags;
}

float StudioFrameAdvance(client_anim_state_t* st, float framerate, float flInterval)
{
	if (flInterval == 0.0)
	{
		flInterval = (gEngfuncs.GetClientTime() - st->animtime);

		if (flInterval <= 0.001)
		{
			st->animtime = gEngfuncs.GetClientTime();
			return 0.0;
		}
	}

	if (!st->animtime)
		flInterval = 0.0;

	st->frame += flInterval * framerate * st->framerate;
	st->animtime = gEngfuncs.GetClientTime();

	if (st->frame < 0.0 || st->frame >= 256.0)
	{
		if (st->m_fSequenceLoops)
			st->frame -= (int)(st->frame / 256.0) * 256.0;
		else
			st->frame = (st->frame < 0.0) ? 0 : 255;

		st->m_fSequenceFinished = TRUE;
	}

	return flInterval;
}

bool WeaponHasAttachments(entity_state_t* pplayer)
{
	studiohdr_t* modelheader = NULL;
	model_t* pweaponmodel;

	if (!pplayer)
		return false;

	pweaponmodel = IEngineStudio.GetModelByIndex(pplayer->weaponmodel);
	modelheader = (studiohdr_t*)IEngineStudio.Mod_Extradata(pweaponmodel);

	return (modelheader->numattachments != 0);
}

// ===================== Hands texture dynamic changing =====================

namespace gViewModelHandsTexture
{
	bool	m_bAvailable = false;
	model_t* m_pLastViewModel = nullptr;
	GLuint	m_iCTHandTexture = NULL;
	GLuint	m_iTRHandTexture = NULL;

	void	Initiation		(void)
	{
		if (m_bAvailable)
			return;

		m_iCTHandTexture = LoadDDS("texture/Models/Hands/v_hands_CT.dds");
		m_iTRHandTexture = LoadDDS("texture/Models/Hands/v_hands_TER.dds");

		m_bAvailable = !!(m_iCTHandTexture && m_iTRHandTexture);
	}

	void	Think			(void)
	{
		if (!m_bAvailable || !g_pViewEnt || !g_pViewEnt->model || g_pViewEnt->model == m_pLastViewModel)
			return;

		studiohdr_t* pStudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(g_pViewEnt->model);
		mstudiotexture_t* pTexture = (mstudiotexture_t*)(((byte*)pStudio) + pStudio->textureindex);

		for (int i = 0; i < pStudio->numtextures; i++)
		{
			if (!Q_strcmp(pTexture->name, "v_hands.bmp"))
			{
				pTexture->index = ((g_iTeam == TEAM_CT) ? m_iCTHandTexture : m_iTRHandTexture);
				break;
			}

			pTexture++;
		}

		m_pLastViewModel = g_pViewEnt->model;
	}

	model_t* m_pLastInfViewModel = nullptr;

	void	InferiorThink	(void)
	{
		if (!m_bAvailable || !gSecViewModelMgr.m_bVisible || !gSecViewModelMgr.m_pModel || gSecViewModelMgr.m_pModel == m_pLastInfViewModel)
			return;

		studiohdr_t* pStudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(gSecViewModelMgr.m_pModel);
		mstudiotexture_t* pTexture = (mstudiotexture_t*)(((byte*)pStudio) + pStudio->textureindex);

		for (int i = 0; i < pStudio->numtextures; i++)
		{
			if (!Q_strcmp(pTexture->name, "v_hands.bmp"))
			{
				pTexture->index = ((g_iTeam == TEAM_CT) ? m_iCTHandTexture : m_iTRHandTexture);
				break;
			}

			pTexture++;
		}

		m_pLastInfViewModel = gSecViewModelMgr.m_pModel;
	}
};

// ===================== CGameStudioModelRenderer =====================

CGameStudioModelRenderer::CGameStudioModelRenderer(void)
{
	m_bLocal = false;
}

void CGameStudioModelRenderer::StudioSetupBones(void)
{
	int i;
	double f;

	mstudiobone_t* pbones;
	mstudioseqdesc_t* pseqdesc;
	mstudioanim_t* panim;

	static float pos[MAXSTUDIOBONES][3];
	static vec4_t q[MAXSTUDIOBONES];
	float bonematrix[3][4];

	static float pos2[MAXSTUDIOBONES][3];
	static vec4_t q2[MAXSTUDIOBONES];
	static float pos3[MAXSTUDIOBONES][3];
	static vec4_t q3[MAXSTUDIOBONES];
	static float pos4[MAXSTUDIOBONES][3];
	static vec4_t q4[MAXSTUDIOBONES];

	if (!m_pCurrentEntity->player)
	{
		CStudioModelRenderer::StudioSetupBones();
		return;
	}

	if (m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq)
		m_pCurrentEntity->curstate.sequence = 0;

	pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;
	panim = StudioGetAnim(m_pRenderModel, pseqdesc);

	if (m_pPlayerInfo)
	{
		int playerNum = m_pCurrentEntity->index - 1;

		if (m_nPlayerGaitSequences[playerNum] != ANIM_JUMP_SEQUENCE && m_pPlayerInfo->gaitsequence == ANIM_JUMP_SEQUENCE)
			m_pPlayerInfo->gaitframe = 0.0;

		m_nPlayerGaitSequences[playerNum] = m_pPlayerInfo->gaitsequence;
	}

	f = StudioEstimateFrame(pseqdesc);

	if (m_pPlayerInfo->gaitsequence == ANIM_WALK_SEQUENCE)
	{
		if (m_pCurrentEntity->curstate.blending[0] <= 26)
		{
			m_pCurrentEntity->curstate.blending[0] = 0;
			m_pCurrentEntity->latched.prevseqblending[0] = m_pCurrentEntity->curstate.blending[0];
		}
		else
		{
			m_pCurrentEntity->curstate.blending[0] -= 26;
			m_pCurrentEntity->latched.prevseqblending[0] = m_pCurrentEntity->curstate.blending[0];
		}
	}

	if (pseqdesc->numblends == 9)
	{
		float s = m_pCurrentEntity->curstate.blending[0];
		float t = m_pCurrentEntity->curstate.blending[1];

		if (s <= 127.0)
		{
			s = (s * 2.0);

			if (t <= 127.0)
			{
				t = (t * 2.0);

				StudioCalcRotations(pos, q, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 1);
				StudioCalcRotations(pos2, q2, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 3);
				StudioCalcRotations(pos3, q3, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 4);
				StudioCalcRotations(pos4, q4, pseqdesc, panim, f);
			}
			else
			{
				t = 2.0 * (t - 127.0);

				panim = LookupAnimation(pseqdesc, 3);
				StudioCalcRotations(pos, q, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 4);
				StudioCalcRotations(pos2, q2, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 6);
				StudioCalcRotations(pos3, q3, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 7);
				StudioCalcRotations(pos4, q4, pseqdesc, panim, f);
			}
		}
		else
		{
			s = 2.0 * (s - 127.0);

			if (t <= 127.0)
			{
				t = (t * 2.0);

				panim = LookupAnimation(pseqdesc, 1);
				StudioCalcRotations(pos, q, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 2);
				StudioCalcRotations(pos2, q2, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 4);
				StudioCalcRotations(pos3, q3, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 5);
				StudioCalcRotations(pos4, q4, pseqdesc, panim, f);
			}
			else
			{
				t = 2.0 * (t - 127.0);

				panim = LookupAnimation(pseqdesc, 4);
				StudioCalcRotations(pos, q, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 5);
				StudioCalcRotations(pos2, q2, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 7);
				StudioCalcRotations(pos3, q3, pseqdesc, panim, f);
				panim = LookupAnimation(pseqdesc, 8);
				StudioCalcRotations(pos4, q4, pseqdesc, panim, f);
			}
		}

		s /= 255.0;
		t /= 255.0;

		StudioSlerpBones(q, pos, q2, pos2, s);
		StudioSlerpBones(q3, pos3, q4, pos4, s);
		StudioSlerpBones(q, pos, q3, pos3, t);
	}
	else
	{
		StudioCalcRotations(pos, q, pseqdesc, panim, f);
	}

	if (m_fDoInterp && m_pCurrentEntity->latched.sequencetime && (m_pCurrentEntity->latched.sequencetime + 0.2 > m_clTime) && (m_pCurrentEntity->latched.prevsequence < m_pStudioHeader->numseq))
	{
		static float pos1b[MAXSTUDIOBONES][3];
		static vec4_t q1b[MAXSTUDIOBONES];
		float s = m_pCurrentEntity->latched.prevseqblending[0];
		float t = m_pCurrentEntity->latched.prevseqblending[1];

		pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->latched.prevsequence;
		panim = StudioGetAnim(m_pRenderModel, pseqdesc);

		if (pseqdesc->numblends == 9)
		{
			if (s <= 127.0)
			{
				s = (s * 2.0);

				if (t <= 127.0)
				{
					t = (t * 2.0);

					StudioCalcRotations(pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 1);
					StudioCalcRotations(pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 3);
					StudioCalcRotations(pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 4);
					StudioCalcRotations(pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
				}
				else
				{
					t = 2.0 * (t - 127.0);

					panim = LookupAnimation(pseqdesc, 3);
					StudioCalcRotations(pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 4);
					StudioCalcRotations(pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 6);
					StudioCalcRotations(pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 7);
					StudioCalcRotations(pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
				}
			}
			else
			{
				s = 2.0 * (s - 127.0);

				if (t <= 127.0)
				{
					t = (t * 2.0);

					panim = LookupAnimation(pseqdesc, 1);
					StudioCalcRotations(pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 2);
					StudioCalcRotations(pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 4);
					StudioCalcRotations(pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 5);
					StudioCalcRotations(pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
				}
				else
				{
					t = 2.0 * (t - 127.0);

					panim = LookupAnimation(pseqdesc, 4);
					StudioCalcRotations(pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 5);
					StudioCalcRotations(pos2, q2, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 7);
					StudioCalcRotations(pos3, q3, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
					panim = LookupAnimation(pseqdesc, 8);
					StudioCalcRotations(pos4, q4, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
				}
			}

			s /= 255.0;
			t /= 255.0;

			StudioSlerpBones(q1b, pos1b, q2, pos2, s);
			StudioSlerpBones(q3, pos3, q4, pos4, s);
			StudioSlerpBones(q1b, pos1b, q3, pos3, t);
		}
		else
		{
			StudioCalcRotations(pos1b, q1b, pseqdesc, panim, m_pCurrentEntity->latched.prevframe);
		}

		s = 1.0 - (m_clTime - m_pCurrentEntity->latched.sequencetime) / 0.2;
		StudioSlerpBones(q, pos, q1b, pos1b, s);
	}
	else
	{
		m_pCurrentEntity->latched.prevframe = f;
	}

	pbones = (mstudiobone_t*)((byte*)m_pStudioHeader + m_pStudioHeader->boneindex);

	if (m_pPlayerInfo && (m_pCurrentEntity->curstate.sequence < ANIM_FIRST_DEATH_SEQUENCE || m_pCurrentEntity->curstate.sequence > ANIM_LAST_DEATH_SEQUENCE) && (m_pCurrentEntity->curstate.sequence < ANIM_FIRST_EMOTION_SEQUENCE || m_pCurrentEntity->curstate.sequence > ANIM_LAST_EMOTION_SEQUENCE) && m_pCurrentEntity->curstate.sequence != ANIM_SWIM_1 && m_pCurrentEntity->curstate.sequence != ANIM_SWIM_2)
	{
		int copy = 1;

		if (m_pPlayerInfo->gaitsequence >= m_pStudioHeader->numseq)
			m_pPlayerInfo->gaitsequence = 0;

		pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pPlayerInfo->gaitsequence;

		panim = StudioGetAnim(m_pRenderModel, pseqdesc);
		StudioCalcRotations(pos2, q2, pseqdesc, panim, m_pPlayerInfo->gaitframe);

		for (i = 0; i < m_pStudioHeader->numbones; i++)
		{
			if (!Q_stricmp(pbones[i].name, "Bip01 Spine"))
				copy = 0;
			else if (!Q_stricmp(pbones[pbones[i].parent].name, "Bip01 Pelvis"))
				copy = 1;

			if (copy)
			{
				Q_memcpy(pos[i], pos2[i], sizeof(pos[i]));
				Q_memcpy(q[i], q2[i], sizeof(q[i]));
			}
		}
	}

	for (i = 0; i < m_pStudioHeader->numbones; i++)
	{
		QuaternionMatrix(q[i], bonematrix);

		bonematrix[0][3] = pos[i][0];
		bonematrix[1][3] = pos[i][1];
		bonematrix[2][3] = pos[i][2];

		if (pbones[i].parent == -1)
		{
			if (IEngineStudio.IsHardware())
			{
				ConcatTransforms((*m_protationmatrix), bonematrix, (*m_pbonetransform)[i]);
				MatrixCopy((*m_pbonetransform)[i], (*m_plighttransform)[i]);
			}
			else
			{
				ConcatTransforms((*m_paliastransform), bonematrix, (*m_pbonetransform)[i]);
				ConcatTransforms((*m_protationmatrix), bonematrix, (*m_plighttransform)[i]);
			}

			StudioFxTransform(m_pCurrentEntity, (*m_pbonetransform)[i]);
		}
		else
		{
			ConcatTransforms((*m_pbonetransform)[pbones[i].parent], bonematrix, (*m_pbonetransform)[i]);
			ConcatTransforms((*m_plighttransform)[pbones[i].parent], bonematrix, (*m_plighttransform)[i]);
		}
	}
}

void CGameStudioModelRenderer::StudioEstimateGait(entity_state_t* pplayer)
{
	float dt;
	vec3_t est_velocity;

	dt = (m_clTime - m_clOldTime);
	dt = Q_max(0.0f, dt);
	dt = Q_min(1.0f, dt);

	if (dt == 0 || m_pPlayerInfo->renderframe == m_nFrameCount)
	{
		m_flGaitMovement = 0;
		return;
	}

	if (m_fGaitEstimation)
	{
		VectorSubtract(m_pCurrentEntity->origin, m_pPlayerInfo->prevgaitorigin, est_velocity);
		VectorCopy(m_pCurrentEntity->origin, m_pPlayerInfo->prevgaitorigin);
		m_flGaitMovement = Length(est_velocity);

		if (dt <= 0 || m_flGaitMovement / dt < 5)
		{
			m_flGaitMovement = 0;
			est_velocity[0] = 0;
			est_velocity[1] = 0;
		}
	}
	else
	{
		VectorCopy(pplayer->velocity, est_velocity);
		m_flGaitMovement = Length(est_velocity) * dt;
	}

	if (est_velocity[1] == 0 && est_velocity[0] == 0)
	{
		float flYawDiff = m_pCurrentEntity->angles[YAW] - m_pPlayerInfo->gaityaw;
		flYawDiff = flYawDiff - (int)(flYawDiff / 360) * 360;

		if (flYawDiff > 180)
			flYawDiff -= 360;

		if (flYawDiff < -180)
			flYawDiff += 360;

		if (dt < 0.25)
			flYawDiff *= dt * 4;
		else
			flYawDiff *= dt;

		m_pPlayerInfo->gaityaw += flYawDiff;
		m_pPlayerInfo->gaityaw = m_pPlayerInfo->gaityaw - (int)(m_pPlayerInfo->gaityaw / 360) * 360;

		m_flGaitMovement = 0;
	}
	else
	{
		m_pPlayerInfo->gaityaw = (atan2(est_velocity[1], est_velocity[0]) * 180 / M_PI);

		if (m_pPlayerInfo->gaityaw > 180)
			m_pPlayerInfo->gaityaw = 180;

		if (m_pPlayerInfo->gaityaw < -180)
			m_pPlayerInfo->gaityaw = -180;
	}
}

void CGameStudioModelRenderer::StudioProcessGait(entity_state_t* pplayer)
{
	mstudioseqdesc_t* pseqdesc;
	float dt;

	CalculateYawBlend(pplayer);
	CalculatePitchBlend(pplayer);

	dt = (m_clTime - m_clOldTime);
	dt = Q_max(0.0f, dt);
	dt = Q_min(1.0f, dt);

	pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + pplayer->gaitsequence;

	if (pseqdesc->linearmovement[0] > 0)
		m_pPlayerInfo->gaitframe += (m_flGaitMovement / pseqdesc->linearmovement[0]) * pseqdesc->numframes;
	else
		m_pPlayerInfo->gaitframe += pseqdesc->fps * dt * m_pCurrentEntity->curstate.framerate;

	m_pPlayerInfo->gaitframe = m_pPlayerInfo->gaitframe - (int)(m_pPlayerInfo->gaitframe / pseqdesc->numframes) * pseqdesc->numframes;

	if (m_pPlayerInfo->gaitframe < 0)
		m_pPlayerInfo->gaitframe += pseqdesc->numframes;
}

int CGameStudioModelRenderer::StudioDrawPlayer(int flags, entity_state_t* pplayer)
{
	int iret = 0;
	bool isLocalPlayer = false;

	if (m_bLocal && IEngineStudio.GetCurrentEntity() == gEngfuncs.GetLocalPlayer())
		isLocalPlayer = true;

	if (isLocalPlayer)
	{
		SavePlayerState(pplayer);
		SetupClientAnimation(pplayer);
	}

	iret = _StudioDrawPlayer(flags, pplayer);

	if (isLocalPlayer)
		RestorePlayerState(pplayer);

	return iret;
}

int CGameStudioModelRenderer::_StudioDrawPlayer(int flags, entity_state_t* pplayer)
{
	m_pCurrentEntity = IEngineStudio.GetCurrentEntity();

	IEngineStudio.GetTimes(&m_nFrameCount, &m_clTime, &m_clOldTime);
	IEngineStudio.GetViewInfo(m_vRenderOrigin, m_vUp, m_vRight, m_vNormal);
	IEngineStudio.GetAliasScale(&m_fSoftwareXScale, &m_fSoftwareYScale);

	m_nPlayerIndex = pplayer->number - 1;

	if (m_nPlayerIndex < 0 || m_nPlayerIndex >= gEngfuncs.GetMaxClients())
		return 0;

	m_pRenderModel = IEngineStudio.SetupPlayerModel(m_nPlayerIndex);

	if (m_pRenderModel == NULL)
		return 0;

	m_pStudioHeader = (studiohdr_t*)IEngineStudio.Mod_Extradata(m_pRenderModel);
	IEngineStudio.StudioSetHeader(m_pStudioHeader);
	IEngineStudio.SetRenderModel(m_pRenderModel);

	if (m_pCurrentEntity->curstate.sequence >= m_pStudioHeader->numseq)
		m_pCurrentEntity->curstate.sequence = 0;

	if (pplayer->sequence >= m_pStudioHeader->numseq)
		pplayer->sequence = 0;

	if (m_pCurrentEntity->curstate.gaitsequence >= m_pStudioHeader->numseq)
		m_pCurrentEntity->curstate.gaitsequence = 0;

	if (pplayer->gaitsequence >= m_pStudioHeader->numseq)
		pplayer->gaitsequence = 0;

	if (pplayer->gaitsequence)
	{
		vec3_t orig_angles;
		m_pPlayerInfo = IEngineStudio.PlayerInfo(m_nPlayerIndex);

		VectorCopy(m_pCurrentEntity->angles, orig_angles);

		StudioProcessGait(pplayer);

		m_pPlayerInfo->gaitsequence = pplayer->gaitsequence;
		m_pPlayerInfo = NULL;

		StudioSetUpTransform(0);
		VectorCopy(orig_angles, m_pCurrentEntity->angles);
	}
	else
	{
		m_pCurrentEntity->curstate.controller[0] = 127;
		m_pCurrentEntity->curstate.controller[1] = 127;
		m_pCurrentEntity->curstate.controller[2] = 127;
		m_pCurrentEntity->curstate.controller[3] = 127;
		m_pCurrentEntity->latched.prevcontroller[0] = m_pCurrentEntity->curstate.controller[0];
		m_pCurrentEntity->latched.prevcontroller[1] = m_pCurrentEntity->curstate.controller[1];
		m_pCurrentEntity->latched.prevcontroller[2] = m_pCurrentEntity->curstate.controller[2];
		m_pCurrentEntity->latched.prevcontroller[3] = m_pCurrentEntity->curstate.controller[3];

		m_pPlayerInfo = IEngineStudio.PlayerInfo(m_nPlayerIndex);

		CalculatePitchBlend(pplayer);
		CalculateYawBlend(pplayer);

		m_pPlayerInfo->gaitsequence = 0;
		StudioSetUpTransform(0);
	}

	if (flags & STUDIO_RENDER)
	{
		if (!IEngineStudio.StudioCheckBBox())
			return 0;

		(*m_pModelsDrawn)++;
		(*m_pStudioModelCount)++;

		if (m_pStudioHeader->numbodyparts == 0)
			return 1;
	}

	m_pPlayerInfo = IEngineStudio.PlayerInfo(m_nPlayerIndex);

	StudioSetupBones();
	StudioSaveBones();

	m_pPlayerInfo->renderframe = m_nFrameCount;
	m_pPlayerInfo = NULL;

	if (flags & STUDIO_EVENTS && (!(flags & STUDIO_RENDER) || !pplayer->weaponmodel || !WeaponHasAttachments(pplayer)))
	{
		StudioCalcAttachments();
		IEngineStudio.StudioClientEvents();

		if (m_pCurrentEntity->index > 0)
		{
			cl_entity_t* ent = gEngfuncs.GetEntityByIndex(m_pCurrentEntity->index);
			memcpy(ent->attachment, m_pCurrentEntity->attachment, sizeof(vec3_t) * 4);
		}
	}

	if (flags & STUDIO_RENDER)
	{
		alight_t lighting;
		vec3_t dir;

		lighting.plightvec = dir;
		IEngineStudio.StudioDynamicLight(m_pCurrentEntity, &lighting);
		IEngineStudio.StudioEntityLight(&lighting);
		IEngineStudio.StudioSetupLighting(&lighting);

		m_pPlayerInfo = IEngineStudio.PlayerInfo(m_nPlayerIndex);
		m_nTopColor = m_pPlayerInfo->topcolor;

		if (m_nTopColor < 0)
			m_nTopColor = 0;

		if (m_nTopColor > 360)
			m_nTopColor = 360;

		m_nBottomColor = m_pPlayerInfo->bottomcolor;

		if (m_nBottomColor < 0)
			m_nBottomColor = 0;

		if (m_nBottomColor > 360)
			m_nBottomColor = 360;

		IEngineStudio.StudioSetRemapColors(m_nTopColor, m_nBottomColor);

		StudioRenderModel();
		m_pPlayerInfo = NULL;

		if (pplayer->weaponmodel)
		{
			studiohdr_t* saveheader = m_pStudioHeader;
			cl_entity_t saveent = *m_pCurrentEntity;

			model_t* pweaponmodel = IEngineStudio.GetModelByIndex(pplayer->weaponmodel);

			m_pStudioHeader = (studiohdr_t*)IEngineStudio.Mod_Extradata(pweaponmodel);
			IEngineStudio.StudioSetHeader(m_pStudioHeader);

			StudioMergeBones(pweaponmodel);

			IEngineStudio.StudioSetupLighting(&lighting);

			StudioRenderModel();

			StudioCalcAttachments();

			if (m_pCurrentEntity->index > 0)
				Q_memcpy(saveent.attachment, m_pCurrentEntity->attachment, sizeof(vec3_t) * m_pStudioHeader->numattachments);

			*m_pCurrentEntity = saveent;
			m_pStudioHeader = saveheader;
			IEngineStudio.StudioSetHeader(m_pStudioHeader);

			if (flags & STUDIO_EVENTS)
				IEngineStudio.StudioClientEvents();
		}

		if (cl_shadows->value)
		{
			Vector pos;

			if (GetPlayerBoneWorldPosition(BONE_PELVIS, &pos))
				StudioDrawShadow(pos, 20.0);
		}
	}

	return 1;
}

void CGameStudioModelRenderer::StudioFxTransform(cl_entity_t* ent, float transform[3][4])
{
	switch (ent->curstate.renderfx)
	{
	case kRenderFxDistort:
	case kRenderFxHologram:
	{
		if (gEngfuncs.pfnRandomLong(0, 49) == 0)
		{
			int axis = gEngfuncs.pfnRandomLong(0, 1);

			if (axis == 1)
				axis = 2;

			VectorScale(transform[axis], gEngfuncs.pfnRandomFloat(1, 1.484), transform[axis]);
		}
		else if (gEngfuncs.pfnRandomLong(0, 49) == 0)
		{
			float offset;
			int axis = gEngfuncs.pfnRandomLong(0, 1);

			if (axis == 1)
				axis = 2;

			offset = gEngfuncs.pfnRandomFloat(-10, 10);
			transform[gEngfuncs.pfnRandomLong(0, 2)][3] += offset;
		}

		break;
	}

	case kRenderFxExplode:
	{
		if (iRenderStateChanged)
		{
			g_flStartScaleTime = m_clTime;
			iRenderStateChanged = FALSE;
		}

		float flTimeDelta = m_clTime - g_flStartScaleTime;

		if (flTimeDelta > 0)
		{
			float flScale = 0.001;

			if (flTimeDelta <= 2.0)
				flScale = 1.0 - (flTimeDelta / 2.0);

			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
					transform[i][j] *= flScale;
			}
		}

		break;
	}
	}
}

void CGameStudioModelRenderer::StudioPlayerBlend(mstudioseqdesc_t* pseqdesc, int* pBlend, float* pPitch)
{
	float range = 45.0;

	*pBlend = (*pPitch * 3);

	if (*pBlend <= -range)
		*pBlend = 255;
	else if (*pBlend >= range)
		*pBlend = 0;
	else
		*pBlend = 255 * (range - *pBlend) / (2 * range);

	*pPitch = 0;
}

void CGameStudioModelRenderer::CalculateYawBlend(entity_state_t* pplayer)
{
	float dt;
	float flYaw;

	dt = (m_clTime - m_clOldTime);
	dt = Q_max(0.0f, dt);
	dt = Q_min(1.0f, dt);

	StudioEstimateGait(pplayer);

	flYaw = m_pCurrentEntity->angles[YAW] - m_pPlayerInfo->gaityaw;
	flYaw = Q_fmod(flYaw, 360.0f);

	if (flYaw < -180)
		flYaw = flYaw + 360;
	else if (flYaw > 180)
		flYaw = flYaw - 360;

	float maxyaw = 120.0;

	if (flYaw > maxyaw)
	{
		m_pPlayerInfo->gaityaw = m_pPlayerInfo->gaityaw - 180;
		m_flGaitMovement = -m_flGaitMovement;
		flYaw = flYaw - 180;
	}
	else if (flYaw < -maxyaw)
	{
		m_pPlayerInfo->gaityaw = m_pPlayerInfo->gaityaw + 180;
		m_flGaitMovement = -m_flGaitMovement;
		flYaw = flYaw + 180;
	}

	float blend_yaw = (flYaw / 90.0) * 128.0 + 127.0;

	blend_yaw = Q_min(255.0f, blend_yaw);
	blend_yaw = Q_max(0.0f, blend_yaw);

	blend_yaw = 255.0 - blend_yaw;

	m_pCurrentEntity->curstate.blending[0] = (int)(blend_yaw);
	m_pCurrentEntity->latched.prevblending[0] = m_pCurrentEntity->curstate.blending[0];
	m_pCurrentEntity->latched.prevseqblending[0] = m_pCurrentEntity->curstate.blending[0];

	m_pCurrentEntity->angles[YAW] = m_pPlayerInfo->gaityaw;

	if (m_pCurrentEntity->angles[YAW] < -0)
		m_pCurrentEntity->angles[YAW] += 360;

	m_pCurrentEntity->latched.prevangles[YAW] = m_pCurrentEntity->angles[YAW];
}

void CGameStudioModelRenderer::CalculatePitchBlend(entity_state_t* pplayer)
{
	mstudioseqdesc_t* pseqdesc;
	int iBlend;

	pseqdesc = (mstudioseqdesc_t*)((byte*)m_pStudioHeader + m_pStudioHeader->seqindex) + m_pCurrentEntity->curstate.sequence;

	StudioPlayerBlend(pseqdesc, &iBlend, &m_pCurrentEntity->angles[PITCH]);

	m_pCurrentEntity->latched.prevangles[PITCH] = m_pCurrentEntity->angles[PITCH];
	m_pCurrentEntity->curstate.blending[1] = iBlend;
	m_pCurrentEntity->latched.prevblending[1] = m_pCurrentEntity->curstate.blending[1];
	m_pCurrentEntity->latched.prevseqblending[1] = m_pCurrentEntity->curstate.blending[1];
}

void CGameStudioModelRenderer::SavePlayerState(entity_state_t* pplayer)
{
	client_anim_state_t* st;
	cl_entity_t* ent = IEngineStudio.GetCurrentEntity();

	assert(ent);

	if (!ent)
		return;

	st = &g_state;

	st->angles = ent->curstate.angles;
	st->origin = ent->curstate.origin;

	st->realangles = ent->angles;

	st->sequence = ent->curstate.sequence;
	st->gaitsequence = pplayer->gaitsequence;
	st->animtime = ent->curstate.animtime;
	st->frame = ent->curstate.frame;
	st->framerate = ent->curstate.framerate;

	Q_memcpy(st->blending, ent->curstate.blending, 2);
	Q_memcpy(st->controller, ent->curstate.controller, 4);

	st->lv = ent->latched;
}

void CGameStudioModelRenderer::SetupClientAnimation(entity_state_t* pplayer)
{
	static double oldtime;
	double curtime, dt;

	client_anim_state_t* st;
	float fr, gs;

	cl_entity_t* ent = IEngineStudio.GetCurrentEntity();
	assert(ent);

	if (!ent)
		return;

	curtime = gEngfuncs.GetClientTime();
	dt = curtime - oldtime;
	dt = min(1.0, max(0.0, dt));

	oldtime = curtime;
	st = &g_clientstate;

	st->framerate = 1.0;

	int oldseq = st->sequence;
	CounterStrike_GetSequence(&st->sequence, &st->gaitsequence);
	CounterStrike_GetOrientation((float*)&st->origin, (float*)&st->angles);
	st->realangles = st->angles;

	if (st->sequence != oldseq)
	{
		st->frame = 0.0;
		st->lv.prevsequence = oldseq;
		st->lv.sequencetime = st->animtime;

		Q_memcpy(st->lv.prevseqblending, st->blending, 2);
		Q_memcpy(st->lv.prevcontroller, st->controller, 4);
	}

	void* pmodel = (studiohdr_t*)IEngineStudio.Mod_Extradata(ent->model);

	GetSequenceInfo(pmodel, st, &fr, &gs);
	st->m_fSequenceLoops = ((GetSequenceFlags(pmodel, st) & STUDIO_LOOPING) != 0);
	StudioFrameAdvance(st, fr, dt);

	VectorCopy(st->realangles, ent->angles);
	VectorCopy(st->angles, ent->curstate.angles);
	VectorCopy(st->origin, ent->curstate.origin);

	ent->curstate.sequence = st->sequence;
	pplayer->gaitsequence = st->gaitsequence;
	ent->curstate.animtime = st->animtime;
	ent->curstate.frame = st->frame;
	ent->curstate.framerate = st->framerate;

	Q_memcpy(ent->curstate.blending, st->blending, 2);
	Q_memcpy(ent->curstate.controller, st->controller, 4);

	ent->latched = st->lv;
}

void CGameStudioModelRenderer::RestorePlayerState(entity_state_t* pplayer)
{
	client_anim_state_t* st;
	cl_entity_t* ent = IEngineStudio.GetCurrentEntity();
	assert(ent);

	if (!ent)
		return;

	st = &g_clientstate;

	st->angles = ent->curstate.angles;
	st->origin = ent->curstate.origin;
	st->realangles = ent->angles;

	st->sequence = ent->curstate.sequence;
	st->gaitsequence = pplayer->gaitsequence;
	st->animtime = ent->curstate.animtime;
	st->frame = ent->curstate.frame;
	st->framerate = ent->curstate.framerate;

	Q_memcpy(st->blending, ent->curstate.blending, 2);
	Q_memcpy(st->controller, ent->curstate.controller, 4);

	st->lv = ent->latched;

	st = &g_state;

	VectorCopy(st->angles, ent->curstate.angles);
	VectorCopy(st->origin, ent->curstate.origin);
	VectorCopy(st->realangles, ent->angles);

	ent->curstate.sequence = st->sequence;
	pplayer->gaitsequence = st->gaitsequence;
	ent->curstate.animtime = st->animtime;
	ent->curstate.frame = st->frame;
	ent->curstate.framerate = st->framerate;

	Q_memcpy(ent->curstate.blending, st->blending, 2);
	Q_memcpy(ent->curstate.controller, st->controller, 4);

	ent->latched = st->lv;
}

mstudioanim_t* CGameStudioModelRenderer::LookupAnimation(mstudioseqdesc_t* pseqdesc, int index)
{
	mstudioanim_t* panim = NULL;

	panim = StudioGetAnim(m_pRenderModel, pseqdesc);

	assert(panim);

	if (index < 0)
		return panim;

	if (index > (pseqdesc->numblends - 1))
		return panim;

	panim += index * m_pStudioHeader->numbones;
	return panim;
}

void CGameStudioModelRenderer::CachePlayerBoneIndices(void)
{
	mstudiobone_t* pbones;
	int cacheIdx;
	int boneIdx;

	if (m_isBoneCacheValid != false)
		return;

	pbones = (mstudiobone_t*)((byte*)m_pStudioHeader + m_pStudioHeader->boneindex);

	for (cacheIdx = BONE_HEAD; cacheIdx < BONE_MAX; cacheIdx++)
	{
		m_boneIndexCache[cacheIdx] = -1;

		for (boneIdx = 0; boneIdx < m_pStudioHeader->numbones; boneIdx++)
		{
			if (Q_stricmp(pbones[boneIdx].name, s_boneNames[cacheIdx]) == 0)
			{
				m_boneIndexCache[cacheIdx] = boneIdx;
				break;
			}
		}
	}
}

int CGameStudioModelRenderer::GetPlayerBoneIndex(BoneIndex whichBone)
{
	return m_boneIndexCache[whichBone];
}

bool CGameStudioModelRenderer::GetPlayerBoneWorldPosition(BoneIndex whichBone, Vector* pos)
{
	CachePlayerBoneIndices();

	if (m_boneIndexCache[whichBone] < 0 || m_boneIndexCache[whichBone] >= m_pStudioHeader->numbones)
		return false;

	(*pos).x = m_rgCachedBoneTransform[m_boneIndexCache[whichBone]][0][3];
	(*pos).y = m_rgCachedBoneTransform[m_boneIndexCache[whichBone]][1][3];
	(*pos).z = m_rgCachedBoneTransform[m_boneIndexCache[whichBone]][2][3];
	return true;
}

// ===================== Secondary V-Model Manager =====================

CSecondaryViewModelManager gSecViewModelMgr;

void CSecondaryViewModelManager::Reset(void)
{
	m_bIsDrawing = NULL;
	m_pModel = NULL;
	m_iSequence = NULL;
	m_flAnimtime = NULL;
	m_flFramerate = NULL;
	m_flFrame = NULL;
	m_bReflect = NULL;
	m_piBody = NULL;
	m_bVisible = NULL;
	m_vecOfs = g_vecZero;
	m_vecRawOrg = g_vecZero;

	Q_memset(m_iController, NULL, sizeof(m_iController));
	Q_memset(m_vecAttachments, NULL, sizeof(m_vecAttachments));

	//memset(&m_sAnimStack, NULL, sizeof(m_sAnimStack));
}

void CSecondaryViewModelManager::SetModel(const char* sz)
{
	m_pModel = IEngineStudio.Mod_ForName(sz, TRUE);
}

void CSecondaryViewModelManager::Draw(int flags)
{
	static cl_entity_t save;
	static Vector vecTemp;

	if (!m_pModel || !m_bVisible)
		return;

	save = *g_pViewEnt;

	//prevent from bothering by others.
	m_bIsDrawing = true;

	g_pViewEnt->model = m_pModel;
	g_pViewEnt->curstate.sequence = m_iSequence;
	g_pViewEnt->curstate.animtime = m_flAnimtime;
	g_pViewEnt->curstate.framerate = m_flFramerate;
	g_pViewEnt->curstate.frame = m_flFrame;
	g_pViewEnt->curstate.body = (m_piBody ? (*m_piBody) : 0);

	// reflecting model
	cl_righthand->value = m_bReflect;

	// UNDONE
	// for inferior weapon hand texture.
	gViewModelHandsTexture::InferiorThink();

	// draw
	g_StudioRenderer.StudioDrawModel(flags);

	// restore
	*g_pViewEnt = save;

	m_bIsDrawing = false;
}

void CSecondaryViewModelManager::SetAnim(int iSeq)
{
	m_iSequence = iSeq;
	m_flAnimtime = gEngfuncs.GetClientTime();
	m_flFramerate = 1;
	m_flFrame = 0;
}

void CSecondaryViewModelManager::PushAnim(void)
{
	m_Stack.m_flAnimtime	= m_flAnimtime;
	m_Stack.m_flFrame		= m_flFrame;
	m_Stack.m_flFramerate	= m_flFramerate;
	m_Stack.m_iSequence		= m_iSequence;
}

void CSecondaryViewModelManager::PopAnim(void)
{
	m_flAnimtime	= m_Stack.m_flAnimtime;
	m_flFrame		= m_Stack.m_flFrame;
	m_flFramerate	= m_Stack.m_flFramerate;
	m_iSequence		= m_Stack.m_iSequence;
}

// ===================== EXPORT FUNCS =====================

int R_StudioDrawPlayer(int flags, entity_state_t* pplayer)
{
	return g_StudioRenderer.StudioDrawPlayer(flags, pplayer);
}

int R_StudioDrawModel(int flags)
{
	if (IEngineStudio.GetCurrentEntity() == g_pViewEnt)
	{
		// draw secondary model first.
		gSecViewModelMgr.Draw(flags);

		// LUNA: fucking engine. when you apply anim via gEngfuncs.pfnWeaponAnim(), it secretly save a time without letting you know.
		// then before the VMDL gets rendered, the engine suddenly re-apply this value back.
		// this prevents you from start an anim from its half.
		// however, this is the fix.
		g_pViewEnt->curstate.animtime = g_flTimeViewModelAnimStart;
		g_pViewEnt->curstate.framerate = g_flViewModelFramerate;

		// special treatment for certain weapons.
		if (g_pCurWeapon)
			cl_righthand->value = g_pCurWeapon->UsingInvertedVMDL();

		// detecting vmdl changing and swapping the vmdl v_hands.bmp texture.
		gViewModelHandsTexture::Think();
	}

	return g_StudioRenderer.StudioDrawModel(flags);
}

void R_StudioInit(void)
{
	g_StudioRenderer.Init();
	gSecViewModelMgr.Reset();
}

r_studio_interface_t studio =
{
	STUDIO_INTERFACE_VERSION,
	R_StudioDrawModel,
	R_StudioDrawPlayer,
};

BOOL HUD_GetStudioModelInterface2(int version, r_studio_interface_s** ppinterface, engine_studio_api_s* pstudio)
{
	if (version != STUDIO_INTERFACE_VERSION)
		return FALSE;

	// Point the engine to our callbacks
	*ppinterface = &studio;

	// Copy in engine helper functions
	IEngineStudio = *pstudio;

	// Initialize local variables, etc.
	R_StudioInit();

	// Success
	return TRUE;
}
