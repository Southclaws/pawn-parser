// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_zipline
	#undef _inc_zipline
#endif
// custom include-guard to ensure we don't duplicate
#if defined _zipline_included
	#endinput
#endif
#define _zipline_included

#include <a_samp>
#include <mathutil>
#include <YSI\y_iterate>
#include <YSI\y_timers>
#include <streamer>
#include <linegen>

#include <YSI\y_hooks>


#if !defined MAX_ZIPLINE
	#define MAX_ZIPLINE (32)
#endif

#define INVALID_ZIPLINE_ID (-1)

enum E_ZIPLINE_DATA {
	zip_startArea,
	zip_endArea,
	zip_lineID,

	Float:zip_startPosX,
	Float:zip_startPosY,
	Float:zip_startPosZ,

	Float:zip_endPosX,
	Float:zip_endPosY,
	Float:zip_endPosZ,

	Float:zip_vecX,
	Float:zip_vecY,
	Float:zip_vecZ,

	zip_world,
	zip_interior
}

static
	zip_Data[MAX_ZIPLINE][E_ZIPLINE_DATA],
	zip_currentZipline[MAX_PLAYERS] = {INVALID_ZIPLINE_ID, ...},
	Timer:zip_UpdateTimer[MAX_PLAYERS],
	Float:zip_PlayerSpeedMult[MAX_PLAYERS];

new
	Iterator:zip_Index<MAX_ZIPLINE>;

forward Float:GetDistancePointLine(Float:line_x,Float:line_y,Float:line_z,Float:vector_x,Float:vector_y,Float:vector_z,Float:point_x,Float:point_y,Float:point_z);
forward Float:GetAngleToPoint(Float:fPointX, Float:fPointY, Float:fDestX, Float:fDestY);
forward Float:GetAbsoluteAngle(Float:angle);

stock CreateZipline(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2, worldid = 0, interiorid = 0) {
	new id = Iter_Free(zip_Index);

	if(id == ITER_NONE) {
		err("MAX_ZIPLINE limit reached");
		return INVALID_ZIPLINE_ID;
	}

	zip_Data[id][zip_startArea] = CreateDynamicSphere(x1, y1, z1 - 1.0, 2.0, worldid, interiorid);
	zip_Data[id][zip_endArea] = CreateDynamicSphere(x2, y2, z2 - 1.0, 6.0, worldid, interiorid);

	zip_Data[id][zip_lineID] = CreateLineSegment(19087, 2.46,
		x1, y1, z1,
		x2, y2, z2,
		.RotX = 90.0, .objlengthoffset = -(2.46/2), .worldid = worldid, .interiorid = interiorid, .maxlength = 1000.0);

	SetLineSegmentDest(zip_Data[id][zip_lineID], x2, y2, z2);

	zip_Data[id][zip_startPosX] = x1;
	zip_Data[id][zip_startPosY] = y1;
	zip_Data[id][zip_startPosZ] = z1;

	zip_Data[id][zip_endPosX] = x2;
	zip_Data[id][zip_endPosY] = y2;
	zip_Data[id][zip_endPosZ] = z2;

	zip_Data[id][zip_world] = worldid;
	zip_Data[id][zip_interior] = interiorid;

	GetLineSegmentVector(zip_Data[id][zip_lineID],
		zip_Data[id][zip_vecX], zip_Data[id][zip_vecY], zip_Data[id][zip_vecZ]);

	Iter_Add(zip_Index, id);

	return id;
}

stock DestroyZipline(ziplineid) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 0;
	}

	DestroyDynamicArea(zip_Data[ziplineid][zip_startArea]);
	DestroyDynamicArea(zip_Data[ziplineid][zip_endArea]);

	DestroyLineSegment(zip_Data[ziplineid][zip_lineID]);

	zip_Data[ziplineid][zip_startArea] = INVALID_STREAMER_ID;
	zip_Data[ziplineid][zip_endArea] = INVALID_STREAMER_ID;
	zip_Data[ziplineid][zip_lineID] = INVALID_LINE_SEGMENT_ID;

	zip_Data[ziplineid][zip_startPosX] = 0.0;
	zip_Data[ziplineid][zip_startPosY] = 0.0;
	zip_Data[ziplineid][zip_startPosZ] = 0.0;

	zip_Data[ziplineid][zip_endPosX] = 0.0;
	zip_Data[ziplineid][zip_endPosY] = 0.0;
	zip_Data[ziplineid][zip_endPosZ] = 0.0;

	zip_Data[ziplineid][zip_world] = 0;
	zip_Data[ziplineid][zip_interior] = 0;

	Iter_Remove(zip_Index, ziplineid);

	return 1;
}

stock bool:IsValidZipline(ziplineid) {
	return Iter_Contains(zip_Index, ziplineid);
}

// zip_startArea
stock GetZiplineStartAreaID(ziplineid) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 0;
	}

	return zip_Data[ziplineid][zip_startArea];
}

// zip_endArea
stock GetZiplineEndAreaID(ziplineid) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 0;
	}

	return zip_Data[ziplineid][zip_endArea];
}

// zip_lineID
stock GetZiplineLineID(ziplineid) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 0;
	}

	return zip_Data[ziplineid][zip_lineID];
}

// zip_startPosX
// zip_startPosY
// zip_startPosZ
stock GetZiplineStartPos(ziplineid, &Float:x, &Float:y, &Float:z) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 1;
	}

	x = zip_Data[ziplineid][zip_startPosX];
	y = zip_Data[ziplineid][zip_startPosY];
	z = zip_Data[ziplineid][zip_startPosZ];

	return 0;
}

stock SetZiplineStartPos(ziplineid, Float:x, Float:y, Float:z) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 1;
	}

	zip_Data[ziplineid][zip_startPosX] = x;
	zip_Data[ziplineid][zip_startPosY] = y;
	zip_Data[ziplineid][zip_startPosZ] = z;

	SetLineSegmentPoint(zip_Data[ziplineid][zip_lineID], x, y, z);
	GetLineSegmentVector(zip_Data[ziplineid][zip_lineID], zip_Data[ziplineid][zip_vecX], zip_Data[ziplineid][zip_vecY], zip_Data[ziplineid][zip_vecZ]);

	return 0;
}

// zip_endPosX
// zip_endPosY
// zip_endPosZ
stock GetZiplineEndPos(ziplineid, &Float:x, &Float:y, &Float:z) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 1;
	}

	x = zip_Data[ziplineid][zip_endPosX];
	y = zip_Data[ziplineid][zip_endPosY];
	z = zip_Data[ziplineid][zip_endPosZ];

	return 0;
}

stock SetZiplineEndPos(ziplineid, Float:x, Float:y, Float:z) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 1;
	}

	zip_Data[ziplineid][zip_endPosX] = x;
	zip_Data[ziplineid][zip_endPosY] = y;
	zip_Data[ziplineid][zip_endPosZ] = z;

	SetLineSegmentDest(zip_Data[ziplineid][zip_lineID], x, y, z);
	GetLineSegmentVector(zip_Data[ziplineid][zip_lineID], zip_Data[ziplineid][zip_vecX], zip_Data[ziplineid][zip_vecY], zip_Data[ziplineid][zip_vecZ]);

	return 0;
}

// zip_vecX
// zip_vecY
// zip_vecZ
stock GetZiplineVector(ziplineid, &Float:x, &Float:y, &Float:z) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 1;
	}

	x = zip_Data[ziplineid][zip_vecX];
	y = zip_Data[ziplineid][zip_vecY];
	z = zip_Data[ziplineid][zip_vecZ];

	return 0;
}

// zip_world
stock GetZiplineWorld(ziplineid)
{
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 0;
	}

	return zip_Data[ziplineid][zip_world];
}

stock SetZiplineWorld(ziplineid, worldid) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 1;
	}

	zip_Data[ziplineid][zip_world] = worldid;

	Streamer_SetIntData(STREAMER_TYPE_AREA, zip_Data[ziplineid][zip_startArea], E_STREAMER_WORLD_ID, worldid);
	Streamer_SetIntData(STREAMER_TYPE_AREA, zip_Data[ziplineid][zip_endArea], E_STREAMER_WORLD_ID, worldid);

	SetLineSegmentWorld(zip_Data[ziplineid][zip_lineID], worldid);

	return 0;
}

// zip_interior
stock GetZiplineInterior(ziplineid) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return -1;
	}

	return zip_Data[ziplineid][zip_interior];
}

stock SetZiplineInterior(ziplineid, interiorid) {
	if(!Iter_Contains(zip_Index, ziplineid)) {
		return 1;
	}

	zip_Data[ziplineid][zip_interior] = interiorid;

	Streamer_SetIntData(STREAMER_TYPE_AREA, zip_Data[ziplineid][zip_startArea], E_STREAMER_INTERIOR_ID, interiorid);
	Streamer_SetIntData(STREAMER_TYPE_AREA, zip_Data[ziplineid][zip_endArea], E_STREAMER_INTERIOR_ID, interiorid);

	SetLineSegmentInterior(zip_Data[ziplineid][zip_lineID], interiorid);

	return 0;
}

stock bool:IsPlayerOnZipline(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return false;
	}

	return (zip_currentZipline[playerid] != INVALID_ZIPLINE_ID);
}

stock GetPlayerZipline(playerid) {
	if(!IsPlayerConnected(playerid)) {
		return INVALID_ZIPLINE_ID;
	}

	return zip_currentZipline[playerid];
}

/*
	Internal
*/

hook OnPlayerEnterDynArea(playerid, areaid) {
	foreach(new i : zip_Index) {
		if(areaid == zip_Data[i][zip_endArea] && zip_currentZipline[playerid] != INVALID_ZIPLINE_ID) {
			_zipline_exit(playerid);
		}
	}

	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {
	if(newkeys & 16) {
		if(zip_currentZipline[playerid] == INVALID_ZIPLINE_ID) {
			foreach(new i : zip_Index) {
				if(IsPlayerInDynamicArea(playerid, zip_Data[i][zip_startArea])) {
					_zipline_enter(playerid, i);
					return Y_HOOKS_CONTINUE_RETURN_1;
				} else if(IsPlayerInDynamicArea(playerid, zip_Data[i][zip_endArea])) {
					return Y_HOOKS_CONTINUE_RETURN_1;
				} else {
					new
						Float:x,
						Float:y,
						Float:z,
						Float:angleto,
						Float:angleof,
						Float:anglesum;
				
					GetPlayerPos(playerid, x, y, z);

					angleto = GetAngleToPoint(zip_Data[i][zip_startPosX], zip_Data[i][zip_startPosY], x, y);
					angleof = GetAbsoluteAngle(-(90-(atan2(zip_Data[i][zip_vecY], zip_Data[i][zip_vecX]))));
					anglesum = GetAbsoluteAngle(angleto - angleof);

					if(anglesum > 270.0 || anglesum < 90.0) {
						angleto = GetAngleToPoint(zip_Data[i][zip_endPosX], zip_Data[i][zip_endPosY], x, y);
						angleof = GetAbsoluteAngle((-(90-(atan2(zip_Data[i][zip_vecY], zip_Data[i][zip_vecX])))) - 180.0);
						anglesum = GetAbsoluteAngle(angleto - angleof);

						if(anglesum > 270.0 || anglesum < 90.0) {
							new Float:dist = GetDistancePointLine(
								zip_Data[i][zip_startPosX], zip_Data[i][zip_startPosY], zip_Data[i][zip_startPosZ]-1.0,
								zip_Data[i][zip_vecX], zip_Data[i][zip_vecY], zip_Data[i][zip_vecZ],
								x, y, z);

							if(dist < 2.0) {
								_zipline_enter(playerid, i);
								return 1;
							}
						}
					}
				}
			}
		} else {
			_zipline_exit(playerid);
		}
	}

	return 1;
}

_zipline_enter(playerid, id) {
	new
		Float:x,
		Float:y,
		Float:z,
		Float:dist;

	GetPlayerPos(playerid, x, y, z);
	dist = GetDistanceToLineSegmentPoint(id, x, y, z);

	SetPlayerPos(playerid,
		zip_Data[id][zip_startPosX] + (zip_Data[id][zip_vecX] * dist),
		zip_Data[id][zip_startPosY] + (zip_Data[id][zip_vecY] * dist),
		zip_Data[id][zip_startPosZ] + (zip_Data[id][zip_vecZ] * dist) - 1.0);

	SetPlayerFacingAngle(playerid, -(atan2(zip_Data[id][zip_vecX], zip_Data[id][zip_vecY])));

	zip_PlayerSpeedMult[playerid] = 0.2;

	ApplyAnimation(playerid, "PED", "CLIMB_IDLE", 4.0, 1, 0, 0, 0, 0, 1);
	zip_currentZipline[playerid] = id;

	zip_UpdateTimer[playerid] = repeat ZiplineUpdate(playerid);

	return 1;
}

_zipline_exit(playerid) {
	new tmpid = zip_currentZipline[playerid];

	ClearAnimations(playerid, 1);
	zip_currentZipline[playerid] = INVALID_ZIPLINE_ID;

	SetPlayerVelocity(playerid,
		zip_Data[tmpid][zip_vecX] * zip_PlayerSpeedMult[playerid],
		zip_Data[tmpid][zip_vecY] * zip_PlayerSpeedMult[playerid],
		(zip_Data[tmpid][zip_vecZ] + 0.05) * zip_PlayerSpeedMult[playerid]);

	stop zip_UpdateTimer[playerid];

	return 1;
}

//hook OnPlayerUpdate(playerid)
timer ZiplineUpdate[50](playerid) {
	if(zip_currentZipline[playerid] == INVALID_ZIPLINE_ID) {
		return;
	}

	new
		Float:x,
		Float:y,
		Float:z,
		Float:dist;

	GetPlayerPos(playerid, x, y, z);

	dist = GetDistancePointLine(
		zip_Data[zip_currentZipline[playerid]][zip_startPosX],
		zip_Data[zip_currentZipline[playerid]][zip_startPosY],
		zip_Data[zip_currentZipline[playerid]][zip_startPosZ]-1.2,
		zip_Data[zip_currentZipline[playerid]][zip_vecX],
		zip_Data[zip_currentZipline[playerid]][zip_vecY],
		zip_Data[zip_currentZipline[playerid]][zip_vecZ],
		x, y, z);

	if(dist > 5.0) {
		_zipline_exit(playerid);
		return;
	}

	SetPlayerVelocity(playerid,
		zip_Data[zip_currentZipline[playerid]][zip_vecX] * zip_PlayerSpeedMult[playerid],
		zip_Data[zip_currentZipline[playerid]][zip_vecY] * zip_PlayerSpeedMult[playerid],
		(zip_Data[zip_currentZipline[playerid]][zip_vecZ] + 0.05) * zip_PlayerSpeedMult[playerid]);

	if(zip_PlayerSpeedMult[playerid] < 0.5) {
		zip_PlayerSpeedMult[playerid] += 0.01;
	}

	return;
}
