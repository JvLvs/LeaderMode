/*
*
*   This program is free software; you can redistribute it and/or modify it
*   under the terms of the GNU General Public License as published by the
*   Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This program is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*   General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software Foundation,
*   Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*   In addition, as a special exception, the author gives permission to
*   link the code of this program with the Half-Life Game Engine ("HL
*   Engine") and Modified Game Libraries ("MODs") developed by Valve,
*   L.L.C ("Valve").  You must obey the GNU General Public License in all
*   respects for all of the code used other than the HL Engine and MODs
*   from Valve.  If you modify this file, you may extend this exception
*   to your version of the file, but you are not obligated to do so.  If
*   you do not wish to do so, delete this exception statement from your
*   version.
*
*/

#include "precompiled.h"

// Begin moving to a nearby hidey-hole.
// NOTE: Do not forget this state may include a very long "move-to" time to get to our hidey spot!
void HideState::OnEnter(CCSBot *me)
{
	m_isAtSpot = false;

	// if duration is "infinite", set it to a reasonably long time to prevent infinite camping
	if (m_duration < 0.0f)
	{
		m_duration = RANDOM_FLOAT(30.0f, 60.0f);
	}

	// decide whether to "ambush" or not - never set to false so as not to override external setting
	if (RANDOM_FLOAT(0.0f, 100.0f) < 50.0f)
	{
		m_isHoldingPosition = true;
	}

	// if we are holding position, decide for how long
	if (m_isHoldingPosition)
	{
		m_holdPositionTime = RANDOM_FLOAT(3.0f, 10.0f);
	}
	else
	{
		m_holdPositionTime = 0.0f;
	}

	m_heardEnemy = false;
	m_firstHeardEnemyTime = 0.0f;
	m_retry = 0;

	if (me->IsFollowing())
	{
		m_leaderAnchorPos = me->GetFollowLeader()->pev->origin;
	}
}

// Move to a nearby hidey-hole.
// NOTE: Do not forget this state may include a very long "move-to" time to get to our hidey spot!
void HideState::OnUpdate(CCSBot *me)
{
	// wait until finished reloading to leave hide state
	if (!me->IsActiveWeaponReloading())
	{
		if (gpGlobals->time - me->GetStateTimestamp() > m_duration)
		{
			me->Idle();
			return;
		}

		// if we are momentarily hiding while following someone, check to see if he has moved on
		if (me->IsFollowing())
		{
			CBasePlayer *pLeader = me->GetFollowLeader();

			// BOTPORT: Determine walk/run velocity thresholds
			float runThreshold = 200.0f;
			if (pLeader->pev->velocity.IsLengthGreaterThan(runThreshold))
			{
				// leader is running, stay with him
				me->Follow(pLeader);
				return;
			}

			// if leader has moved, stay with him
			const float followRange = 250.0f;
			if ((m_leaderAnchorPos - pLeader->pev->origin).IsLengthGreaterThan(followRange))
			{
				me->Follow(pLeader);
				return;
			}
		}

		// if we see a nearby buddy in combat, join him
		// TODO: Perhaps tie in to TakeDamage(), so it works for human players, too

		// Scenario logic
		// TODO: design for each charactoers.

		bool isSettledInSniper = (me->IsSniper() && m_isAtSpot) ? true : false;

		// only investigate noises if we are initiating attacks, and we aren't a "settled in" sniper
		// dont investigate noises if we are reloading
		if (!me->IsActiveWeaponReloading() &&
			!isSettledInSniper &&
			me->GetDisposition() == CCSBot::ENGAGE_AND_INVESTIGATE)
		{
			// if we are holding position, and have heard the enemy nearby, investigate after our hold time is up
			if (m_isHoldingPosition && m_heardEnemy && (gpGlobals->time - m_firstHeardEnemyTime > m_holdPositionTime))
			{
				// TODO: We might need to remember specific location of last enemy noise here
				me->InvestigateNoise();
				return;
			}

			// investigate nearby enemy noises
			if (me->ShouldInvestigateNoise())
			{
				// if we are holding position, check if enough time has elapsed since we first heard the enemy
				if (m_isAtSpot && m_isHoldingPosition)
				{
					if (!m_heardEnemy)
					{
						// first time we heard the enemy
						m_heardEnemy = true;
						m_firstHeardEnemyTime = gpGlobals->time;
						me->PrintIfWatched("Heard enemy, holding position for %f2.1 seconds...\n", m_holdPositionTime);
					}
				}
				else
				{
					// not holding position - investigate enemy noise
					me->InvestigateNoise();
					return;
				}
			}
		}
	}

	// look around
	me->UpdateLookAround();

	// if we are at our hiding spot, crouch and wait
	if (m_isAtSpot)
	{
		me->Crouch();

		// if we have a shield, hide behind it
		if (me->HasShield() && !me->IsProtectedByShield())
			me->SecondaryAttack();

		// while sitting at our hiding spot, if we are being attacked but can't see our attacker, move somewhere else
		const float hurtRecentlyTime = 1.0f;
		if (!me->IsEnemyVisible() && me->GetTimeSinceAttacked() < hurtRecentlyTime)
		{
			me->Idle();
			return;
		}

		// encourage the human player to do something.
		if (!me->IsDoingScenario())
		{
			if (me->m_iTeam == CT)
			{
			}
		}
	}
	else
	{
		// if a Player is using this hiding spot, give up
		float range;
		CBasePlayer *pCamper = UTIL_GetClosestPlayer(&m_hidingSpot, &range);

		const float closeRange = 75.0f;
		if (pCamper && pCamper != me && range < closeRange && me->IsVisible(pCamper, CHECK_FOV))
		{
			// player is in our hiding spot
			me->PrintIfWatched("Someone's in my hiding spot - picking another...\n");

			const int maxRetries = 3;
			if (m_retry++ >= maxRetries)
			{
				me->PrintIfWatched("Can't find a free hiding spot, giving up.\n");
				me->Idle();
				return;
			}

			// pick another hiding spot near where we were planning on hiding
			me->Hide(TheNavAreaGrid.GetNavArea(&m_hidingSpot));
			return;
		}

		Vector toSpot;
		toSpot.x = m_hidingSpot.x - me->pev->origin.x;
		toSpot.y = m_hidingSpot.y - me->pev->origin.y;
		toSpot.z = m_hidingSpot.z - me->GetFeetZ(); // use feet location
		float dist = toSpot.Length();

		const float crouchDist = 200.0f;
		if (dist < crouchDist)
			me->Crouch();

		const float atDist = 20.0f;
		if (dist < atDist)
		{
			m_isAtSpot = true;

			// make sure our approach points are valid, since we'll be watching them
			me->ComputeApproachPoints();

			// ready our weapon and prepare to attack
			me->EquipBestWeapon(MUST_EQUIP);
			me->SetDisposition(CCSBot::OPPORTUNITY_FIRE);

			// if we are a sniper, update our task
			if (me->GetTask() == CCSBot::MOVE_TO_SNIPER_SPOT)
			{
				me->SetTask(CCSBot::SNIPING);
			}

			// determine which way to look
			TraceResult result;
			float outAngle = 0.0f;
			float outAngleRange = 0.0f;
			for (float angle = 0.0f; angle < 360.0f; angle += 45.0f)
			{
				UTIL_TraceLine(me->GetEyePosition(), me->GetEyePosition() + 1000.0f * Vector(BotCOS(angle), BotSIN(angle), 0.0f), ignore_monsters, ignore_glass, ENT(me->pev), &result);

				if (result.flFraction > outAngleRange)
				{
					outAngle = angle;
					outAngleRange = result.flFraction;
				}
			}

			me->SetLookAheadAngle(outAngle);
		}

		// move to hiding spot
		if (me->UpdatePathMovement() != CCSBot::PROGRESSING && !m_isAtSpot)
		{
			// we couldn't get to our hiding spot - pick another
			me->PrintIfWatched("Can't get to my hiding spot - finding another...\n");

			// search from hiding spot, since we know it was valid
			const Vector *pos = FindNearbyHidingSpot(me, &m_hidingSpot, m_searchFromArea, m_range, me->IsSniper());
			if (!pos)
			{
				// no available hiding spots
				me->PrintIfWatched("No available hiding spots - hiding where I'm at.\n");

				// hide where we are
				m_hidingSpot.x = me->pev->origin.x;
				m_hidingSpot.y = me->pev->origin.y;
				m_hidingSpot.z = me->GetFeetZ();
			}
			else
			{
				m_hidingSpot = *pos;
			}

			// build a path to our new hiding spot
			if (me->ComputePath(TheNavAreaGrid.GetNavArea(&m_hidingSpot), &m_hidingSpot, FASTEST_ROUTE) == false)
			{
				me->PrintIfWatched("Can't pathfind to hiding spot\n");
				me->Idle();
				return;
			}
		}
	}
}

void HideState::OnExit(CCSBot *me)
{
	m_isHoldingPosition = false;

	me->StandUp();
	me->ResetStuckMonitor();
	me->ClearLookAt();
	me->ClearApproachPoints();

	// if we have a shield, put it away
	if (me->HasShield() && me->IsProtectedByShield())
		me->SecondaryAttack();
}
