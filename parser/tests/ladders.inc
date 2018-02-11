// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_ladders
	#undef _inc_ladders
#endif
// custom include-guard to ensure we don't duplicate
#if defined _ladders_included
	#endinput
#endif
#define _ladders_included

#include <a_samp>
#include <YSI\y_iterate>
#include <streamer>
#include <action-text>

#include <YSI\y_hooks>


#define MAX_LADDER	(16)
#define CLIMB_SPEED	(0.1)
#define IDLE_SPEED	(0.01)

enum E_LADDER_DATA {
			ldr_areaID,
Float:		ldr_posX,
Float:		ldr_posY,
Float:		ldr_base,
Float:		ldr_top,
Float:		ldr_ang
}

static
	ldr_Data[MAX_LADDER][E_LADDER_DATA],
	ldr_currentAnim[MAX_PLAYERS],
	ldr_currentLadder[MAX_PLAYERS],
	ldr_enterLadderTick[MAX_PLAYERS];

new
	Iterator:ldr_Index<MAX_LADDER>;


hook OnScriptInit() {
	for(new i; i < MAX_PLAYERS; i++) {
		ldr_currentLadder[i] = -1;
	}
}

stock CreateLadder(Float:x, Float:y, Float:z, Float:height, Float:angle, world = -1, interior = -1)
{
	new id = Iter_Free(ldr_Index);

	ldr_Data[id][ldr_areaID] = CreateDynamicCircle(x, y, 1.0, world, interior);
	ldr_Data[id][ldr_posX] = x;
	ldr_Data[id][ldr_posY] = y;
	ldr_Data[id][ldr_base] = z;
	ldr_Data[id][ldr_top] = height;
	ldr_Data[id][ldr_ang] = angle;

	Iter_Add(ldr_Index, id);

	return id;
}

stock DestroyLadder(ladderid)
{
	if(!Iter_Contains(ldr_Index, ladderid))
		return 0;

	new next;

	DestroyDynamicArea(ldr_Data[ladderid][ldr_areaID]);
	ldr_Data[ladderid][ldr_posX] = 0.0;
	ldr_Data[ladderid][ldr_posY] = 0.0;
	ldr_Data[ladderid][ldr_base] = 0.0;
	ldr_Data[ladderid][ldr_top] = 0.0;
	ldr_Data[ladderid][ldr_ang] = 0.0;

	Iter_SafeRemove(ldr_Index, ladderid, next);

	return next;
}


stock IsValidLadder(ladderid)
{
	if(Iter_Contains(ldr_Index, ladderid))
		return 0;

	return 1;
}

// ldr_areaID
stock GetLadderAreaID(ladderid)
{
	if(Iter_Contains(ldr_Index, ladderid))
		return 0;

	return ldr_Data[ladderid][ldr_areaID];
}

// ldr_posX
// ldr_posY
stock GetLadderPos(ladderid, &Float:x, &Float:y)
{
	if(Iter_Contains(ldr_Index, ladderid))
		return 0;

	x = ldr_Data[ladderid][ldr_posX];
	y = ldr_Data[ladderid][ldr_posY];

	return 1;
}

// ldr_base
stock GetLadderBase(ladderid, &Float:z)
{
	if(Iter_Contains(ldr_Index, ladderid))
		return 0;

	z = ldr_Data[ladderid][ldr_base];

	return 1;
}

// ldr_top
stock GetLadderTop(ladderid, &Float:z)
{
	if(Iter_Contains(ldr_Index, ladderid))
		return 0;

	z = ldr_Data[ladderid][ldr_top];

	return 1;
}

// ldr_ang
stock GetLadderAngle(ladderid, &Float:angle)
{
	if(Iter_Contains(ldr_Index, ladderid))
		return 0;

	angle = ldr_Data[ladderid][ldr_ang];

	return 1;
}

stock IsPlayerOnLadder(playerid, ladderid)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(ldr_currentLadder[playerid] == ladderid)
		return 1;

	return 0;
}

stock IsPlayerOnAnyLadder(playerid)
{
	if(!IsPlayerConnected(playerid))
		return 0;

	if(ldr_currentLadder[playerid] == -1)
		return 0;

	return 1;
}

stock GetPlayerLadder(playerid)
{
	if(!IsPlayerConnected(playerid))
		return -1;

	return ldr_currentLadder[playerid];
}


/*==============================================================================

	Internal

==============================================================================*/


hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if(newkeys & 16)
	{
		if(ldr_currentLadder[playerid] != -1)
		{
			ExitLadder(playerid);

			return 1;
		}

		foreach(new i : ldr_Index)
		{
			if(IsPlayerInDynamicArea(playerid, ldr_Data[i][ldr_areaID]))
			{
				if(ldr_currentLadder[playerid] == -1)
				{
					new
						Float:z;

					GetPlayerPos(playerid, z, z, z);

					if(z < ldr_Data[i][ldr_top] + 2.0 && z > ldr_Data[i][ldr_base] - 1.0)
						EnterLadder(playerid, i, z);
				}
			}
		}
	}

	return 1;
}

EnterLadder(playerid, ladder, Float:z)
{
	new Float:zOffset;

	if(floatabs(z - ldr_Data[ladder][ldr_top]) < 2.0)
		zOffset = ldr_Data[ladder][ldr_top] - 2.0926;

	else if(floatabs(z - ldr_Data[ladder][ldr_base]) < 2.0)
		zOffset = ldr_Data[ladder][ldr_base] + 1.5;

	else
		zOffset = z;

	ClearAnimations(playerid);
	SetPlayerFacingAngle(playerid, ldr_Data[ladder][ldr_ang]);
	SetPlayerPos(playerid, ldr_Data[ladder][ldr_posX], ldr_Data[ladder][ldr_posY], zOffset);

	ApplyAnimation(playerid, "PED", "CLIMB_IDLE", 3.0, 0, 0, 0, 1, 0, 1);
	ldr_enterLadderTick[playerid] = GetTickCount();
	ldr_currentLadder[playerid] = ladder;
}

ExitLadder(playerid)
{
	ClearAnimations(playerid);
	SetPlayerFacingAngle(playerid, ldr_Data[ldr_currentLadder[playerid]][ldr_ang]);

	SetPlayerVelocity(playerid,
		0.1*floatsin(-ldr_Data[ldr_currentLadder[playerid]][ldr_ang], degrees),
		0.1*floatcos(-ldr_Data[ldr_currentLadder[playerid]][ldr_ang], degrees), 0.1);

	ldr_currentLadder[playerid] = -1;
	return 1;
}

hook OnPlayerEnterDynArea(playerid, areaid)
{
	foreach(new i : ldr_Index)
	{
		if(areaid == ldr_Data[i][ldr_areaID])
		{
			new
				Float:z;

			GetPlayerPos(playerid, z, z, z);

			if(z < ldr_Data[i][ldr_top] + 2.0 && z > ldr_Data[i][ldr_base] - 1.0)
				ShowActionText(playerid, "Press F to climb", 0, 120);
		}
	}

	return 1;
}

hook OnPlayerLeaveDynArea(playerid, areaid)
{
	HideActionText(playerid);

	return 1;
}

hook OnPlayerUpdate(playerid)
{
	if(ldr_currentLadder[playerid] != -1)
	{
		new
			k,
			ud,
			lr,
			Float:z;

		GetPlayerKeys(playerid, k, ud, lr);
		GetPlayerPos(playerid, z, z, z);

		if(GetTickCount()-ldr_enterLadderTick[playerid] > 1000 &&
			( z-ldr_Data[ldr_currentLadder[playerid]][ldr_base]<0.5 || z >= ldr_Data[ldr_currentLadder[playerid]][ldr_top]-0.5) )
		{
			ExitLadder(playerid);
			return 1;
		}

		if(ud == KEY_UP)
		{
			if(ldr_currentAnim[playerid])
			{
				ApplyAnimation(playerid, "FINALE", "FIN_HANG_LOOP", 4.0, 1, 0, 0, 0, 0);
				// ApplyAnimation(playerid, "PED", "CLIMB_JUMP", 3.0, 0, 0, 0, 1, 0, 1); // Alternative animation
				ldr_currentAnim[playerid]=0;
				SetPlayerVelocity(playerid, 0.0, 0.0, CLIMB_SPEED);
			}
			else
			{
				// ApplyAnimation(playerid, "FINALE", "FIN_HANG_LOOP", 4.0, 1, 0, 0, 0, 0); // Alternative animation
				ApplyAnimation(playerid, "PED", "CLIMB_IDLE", 3.0, 0, 0, 0, 1, 0, 1);
				ldr_currentAnim[playerid]=1;
			}
		}
		else if(ud == KEY_DOWN)
		{
			if(ldr_currentAnim[playerid])
			{
				ApplyAnimation(playerid, "FINALE", "FIN_HANG_LOOP", 4.0, 1, 0, 0, 0, 0);
				// ApplyAnimation(playerid, "PED", "CLIMB_JUMP", 3.0, 0, 0, 0, 1, 0, 1); // Alternative animation
				ldr_currentAnim[playerid]=0;
			}
			else
			{
				// ApplyAnimation(playerid, "FINALE", "FIN_HANG_LOOP", 4.0, 1, 0, 0, 0, 0); // Alternative animation
				ApplyAnimation(playerid, "PED", "CLIMB_IDLE", 3.0, 0, 0, 0, 1, 0, 1);
				ldr_currentAnim[playerid]=1;
				SetPlayerVelocity(playerid, 0.0, 0.0, -(CLIMB_SPEED*0.7));
			}
		}
		else
		{
			ApplyAnimation(playerid, "PED", "CLIMB_IDLE", 3.0, 0, 0, 0, 1, 0, 1);
			SetPlayerVelocity(playerid, 0.0, 0.0, IDLE_SPEED);
		}
	}

	return 1;
}
