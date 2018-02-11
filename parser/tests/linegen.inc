// built-in include guard removal
// just in case the user has a local dependency with the same file name
#if defined _inc_linegen
	#undef _inc_linegen
#endif
// custom include-guard to ensure we don't duplicate
#if defined _linegen_included
	#endinput
#endif
#define _linegen_included

#include <a_samp>
#include <logger>
#include <YSI\y_iterate>
#include <YSI\y_timers>
#include <streamer>


#if !defined MAX_LINE_SEGMENT
	#define MAX_LINE_SEGMENT (1024)
#endif

#if !defined MAX_OBJECTS_PER_LINE
	#define MAX_OBJECTS_PER_LINE (512)
#endif

#if !defined INVALID_LINE_SEGMENT_ID
	#define INVALID_LINE_SEGMENT_ID (-1)
#endif

enum E_LINE_DATA {
	line_model,
	line_objCount,
	Float:line_objLength,
	Float:line_objLengthOffset,
	Float:line_maxLength,

	Float:line_posX,
	Float:line_posY,
	Float:line_posZ,

	Float:line_dstX,
	Float:line_dstY,
	Float:line_dstZ,

	Float:line_rotX,
	Float:line_rotY,
	Float:line_rotZ,

	line_world,
	line_interior,
	line_playerid
}

static
	line_Data[MAX_LINE_SEGMENT][E_LINE_DATA],
	line_Objects[MAX_LINE_SEGMENT][MAX_OBJECTS_PER_LINE],
	Iterator:line_Index<MAX_LINE_SEGMENT>;

stock CreateLineSegment(
	modelid, Float:objlength,
	Float:PointX, Float:PointY, Float:PointZ,
	Float:DestX, Float:DestY, Float:DestZ,
	Float:RotX = 0.0, Float:RotY = 0.0, Float:RotZ = 0.0,
	Float:objlengthoffset = 0.0,
	worldid = -1, interiorid = -1, playerid = -1, Float:maxlength = 100.0
) {
	new id = Iter_Free(line_Index);

	if(id == ITER_NONE) {
		err("MAX_LINE_SEGMENT limit reached.");
		return INVALID_LINE_SEGMENT_ID;
	}
	
	line_Data[id][line_maxLength]			= maxlength;
	line_Data[id][line_model]				= modelid;
	line_Data[id][line_objLength]			= objlength;
	line_Data[id][line_objLengthOffset]		= objlengthoffset;

	line_Data[id][line_posX]				= PointX;
	line_Data[id][line_posY]				= PointY;
	line_Data[id][line_posZ]				= PointZ;

	line_Data[id][line_dstX]				= DestX;
	line_Data[id][line_dstY]				= DestY;
	line_Data[id][line_dstZ]				= DestZ;

	line_Data[id][line_rotX]				= RotX;
	line_Data[id][line_rotY]				= RotY;
	line_Data[id][line_rotZ]				= RotZ;

	line_Data[id][line_world]				= worldid;
	line_Data[id][line_interior]			= interiorid;
	line_Data[id][line_playerid]			= playerid;

	Iter_Add(line_Index, id);

	updateLineSegment(id);

	return id;
}

stock DestroyLineSegment(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0;
	}

	for(new i; i < line_Data[lineid][line_objCount]; i++) {
		if(IsValidDynamicObject(line_Objects[lineid][i])) {
			DestroyDynamicObject(line_Objects[lineid][i]);
			line_Objects[lineid][i] = INVALID_OBJECT_ID;
		}
	}

	Iter_Remove(line_Index, lineid);

	return 1;
}


stock bool:IsValidLineSegment(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return false;
	}

	return true;
}

// line_model
stock GetLineSegmentModel(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0;
	}

	return line_Data[lineid][line_model];
}

stock SetLineSegmentModel(lineid, modelid, Float:objlength, Float:objlengthoffset) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_model] = modelid;
	line_Data[lineid][line_objLength] = objlength;
	line_Data[lineid][line_objLengthOffset] = objlengthoffset;

	for(new i; i < line_Data[lineid][line_objCount]; i++) {
		if(IsValidDynamicObject(line_Objects[lineid][i])) {
			Streamer_SetIntData(STREAMER_TYPE_OBJECT, line_Objects[lineid][i], E_STREAMER_MODEL_ID, modelid);
		}
	}

	updateLineSegment(lineid);

	return 0;
}

// line_objCount
stock GetLineSegmentObjectCount(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0;
	}

	return line_Data[lineid][line_objCount];
}

// line_objLength
stock Float:GetLineSegmentObjectLength(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0.0;
	}

	return line_Data[lineid][line_objLength];
}

stock SetLineSegmentObjectLength(lineid, Float:objlength) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_objLength] = objlength;

	return 0;
}

// line_objLengthOffset
stock Float:GetLineSegmentObjectOffset(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0.0;
	}

	return line_Data[lineid][line_objLengthOffset];
}

stock SetLineSegmentObjectOffset(lineid, Float:objlengthoffset) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0;
	}

	line_Data[lineid][line_objLengthOffset] = objlengthoffset;

	return 1;
}

// line_maxLength
stock Float:GetLineSegmentMaxLength(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0.0;
	}

	return line_Data[lineid][line_maxLength];
}

stock SetLineSegmentMaxLength(lineid, Float:maxlength) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_maxLength] = maxlength;

	updateLineSegment(lineid);

	return 0;
}

// line_posX
// line_posY
// line_posZ
stock GetLineSegmentPoint(lineid, &Float:PointX, &Float:PointY, &Float:PointZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	PointX = line_Data[lineid][line_posX];
	PointY = line_Data[lineid][line_posY];
	PointZ = line_Data[lineid][line_posZ];

	updateLineSegment(lineid);

	return 0;
}

stock SetLineSegmentPoint(lineid, Float:PointX, Float:PointY, Float:PointZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_posX] = PointX;
	line_Data[lineid][line_posY] = PointY;
	line_Data[lineid][line_posZ] = PointZ;

	updateLineSegment(lineid);

	return 0;
}

// line_dstX
// line_dstY
// line_dstZ
stock GetLineSegmentDest(lineid, &Float:DestX, &Float:DestY, &Float:DestZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	DestX = line_Data[lineid][line_dstX];
	DestY = line_Data[lineid][line_dstY];
	DestZ = line_Data[lineid][line_dstZ];

	updateLineSegment(lineid);

	return 0;
}

stock SetLineSegmentDest(lineid, Float:DestX, Float:DestY, Float:DestZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_dstX] = DestX;
	line_Data[lineid][line_dstY] = DestY;
	line_Data[lineid][line_dstZ] = DestZ;

	updateLineSegment(lineid);

	return 0;
}

// line_rotX
// line_rotY
// line_rotZ
stock GetLineSegmentModelAngles(lineid, &Float:RotX, &Float:RotY, &Float:RotZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	RotX = line_Data[lineid][line_rotX];
	RotY = line_Data[lineid][line_rotY];
	RotZ = line_Data[lineid][line_rotZ];

	updateLineSegment(lineid);

	return 0;
}

stock SetLineSegmentModelAngles(lineid, Float:RotX, Float:RotY, Float:RotZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_rotX] = RotX;
	line_Data[lineid][line_rotY] = RotY;
	line_Data[lineid][line_rotZ] = RotZ;

	updateLineSegment(lineid);

	return 0;
}

// line_world
stock GetLineSegmentWorld(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return -1;
	}

	return line_Data[lineid][line_world];
}

stock SetLineSegmentWorld(lineid, world) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_world] = world;

	for(new i; i < line_Data[lineid][line_objCount]; i++) {
		if(IsValidDynamicObject(line_Objects[lineid][i])) {
			Streamer_SetIntData(STREAMER_TYPE_OBJECT, line_Objects[lineid][i], E_STREAMER_WORLD_ID, world);
		}
	}

	updateLineSegment(lineid);

	return 0;
}

// line_interior
stock GetLineSegmentInterior(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0;
	}

	return line_Data[lineid][line_interior];
}

stock SetLineSegmentInterior(lineid, interior) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_interior] = interior;

	for(new i; i < line_Data[lineid][line_objCount]; i++) {
		if(IsValidDynamicObject(line_Objects[lineid][i])) {
			Streamer_SetIntData(STREAMER_TYPE_OBJECT, line_Objects[lineid][i], E_STREAMER_INTERIOR_ID, interior);
		}
	}

	updateLineSegment(lineid);

	return 0;
}

// line_playerid
stock GetLineSegmentPlayerID(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0;
	}

	return line_Data[lineid][line_playerid];
}

stock SetLineSegmentPlayerID(lineid, playerid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	line_Data[lineid][line_playerid] = playerid;

	for(new i; i < line_Data[lineid][line_objCount]; i++) {
		if(IsValidDynamicObject(line_Objects[lineid][i])) {
			Streamer_SetIntData(STREAMER_TYPE_OBJECT, line_Objects[lineid][i], E_STREAMER_PLAYER_ID, playerid);
		}
	}

	updateLineSegment(lineid);

	return 0;
}

stock GetLineSegmentVector(lineid, &Float:x, &Float:y, &Float:z) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 1;
	}

	new
		Float:vx = line_Data[lineid][line_dstX] - line_Data[lineid][line_posX],
		Float:vy = line_Data[lineid][line_dstY] - line_Data[lineid][line_posY],
		Float:vz = line_Data[lineid][line_dstZ] - line_Data[lineid][line_posZ],
		Float:rx,
		Float:rz;

	rx = -(floatabs(atan2(floatsqroot(floatpower(vx, 2.0) + floatpower(vy, 2.0)), vz))-90.0);
	rz = -(atan2(vy, vx)-90.0);

	x = floatsin(rz, degrees) * floatcos(rx, degrees);
	y = floatcos(rz, degrees) * floatcos(rx, degrees);
	z = floatsin(rx, degrees);

	return 0;
}

stock Float:GetDistanceToLineSegmentPoint(lineid, Float:FromX, Float:FromY, Float:FromZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0.0;
	}

	new
		Float:vx = FromX - line_Data[lineid][line_posX],
		Float:vy = FromY - line_Data[lineid][line_posY],
		Float:vz = FromZ - line_Data[lineid][line_posZ];

	return floatsqroot( (vx * vx) + (vy * vy) + (vz * vz) );
}

stock Float:GetDistanceToLineSegmentDest(lineid, Float:FromX, Float:FromY, Float:FromZ) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0.0;
	}

	new
		Float:vx = FromX - line_Data[lineid][line_dstX],
		Float:vy = FromY - line_Data[lineid][line_dstY],
		Float:vz = FromZ - line_Data[lineid][line_dstZ];

	return floatsqroot( (vx * vx) + (vy * vy) + (vz * vz) );
}

stock Float:GetLineSegmentLength(lineid) {
	if(!Iter_Contains(line_Index, lineid)) {
		return 0.0;
	}

	new
		Float:vx = line_Data[lineid][line_dstX] - line_Data[lineid][line_posX],
		Float:vy = line_Data[lineid][line_dstY] - line_Data[lineid][line_posY],
		Float:vz = line_Data[lineid][line_dstZ] - line_Data[lineid][line_posZ];

	return floatsqroot( (vx * vx) + (vy * vy) + (vz * vz) );
}

/*
	Internal
*/

stock updateLineSegment(lineid) {
	new
		Float:rx,
		Float:ry,
		Float:rz,
		Float:vx = line_Data[lineid][line_dstX] - line_Data[lineid][line_posX],
		Float:vy = line_Data[lineid][line_dstY] - line_Data[lineid][line_posY],
		Float:vz = line_Data[lineid][line_dstZ] - line_Data[lineid][line_posZ],
		Float:tmpdist,
		Float:distToDest,
		count;

	rz = -(atan2(vy, vx)-90.0);
	rx = -(floatabs(atan2(floatsqroot(floatpower(vx, 2.0) + floatpower(vy, 2.0)), vz))-90.0);

	distToDest = floatsqroot( (vx*vx) + (vy*vy) + (vz*vz) );
	count = floatround(distToDest / line_Data[lineid][line_objLength], floatround_ceil) + 1;

	for(new i; i < count; i++) {
		if(i == 0) {
			tmpdist = (
				(line_Data[lineid][line_objLength] / 2.0) +
				line_Data[lineid][line_objLengthOffset]
			);
		} else if(i == count - 1) {
			tmpdist = (
				(distToDest - (line_Data[lineid][line_objLength] / 2.0)) +
				line_Data[lineid][line_objLengthOffset]
			);
		} else {
			tmpdist = (
				(line_Data[lineid][line_objLength] * (i - 1)) +
				(line_Data[lineid][line_objLength] / 2.0) +
				line_Data[lineid][line_objLengthOffset]
			);
		}


		if(tmpdist > distToDest) {
			break;
		}

		if(!IsValidDynamicObject(line_Objects[lineid][i])) {
			line_Objects[lineid][i] = CreateDynamicObject(line_Data[lineid][line_model],
				line_Data[lineid][line_posX] + ( tmpdist * floatsin(rz, degrees) * floatcos(rx, degrees) ),
				line_Data[lineid][line_posY] + ( tmpdist * floatcos(rz, degrees) * floatcos(rx, degrees) ),
				line_Data[lineid][line_posZ] + ( tmpdist * floatsin(rx, degrees) ),
				rx + line_Data[lineid][line_rotX],
				ry + line_Data[lineid][line_rotY],
				-rz + line_Data[lineid][line_rotZ],
				line_Data[lineid][line_world], line_Data[lineid][line_interior], line_Data[lineid][line_playerid]);
		} else {
			SetDynamicObjectPos(line_Objects[lineid][i],
				line_Data[lineid][line_posX] + ( tmpdist * floatsin(rz, degrees) * floatcos(rx, degrees) ),
				line_Data[lineid][line_posY] + ( tmpdist * floatcos(rz, degrees) * floatcos(rx, degrees) ),
				line_Data[lineid][line_posZ] + ( tmpdist * floatsin(rx, degrees) ) );

			SetDynamicObjectRot(line_Objects[lineid][i],
				rx + line_Data[lineid][line_rotX],
				ry + line_Data[lineid][line_rotY],
				-rz + line_Data[lineid][line_rotZ]);
		}

		if(tmpdist > line_Data[lineid][line_maxLength]) {
			break;
		}
	}

	if(count < line_Data[lineid][line_objCount]) {
		for(new i = count; i < MAX_OBJECTS_PER_LINE; i++) {
			if(IsValidDynamicObject(line_Objects[lineid][i])) {
				DestroyDynamicObject(line_Objects[lineid][i]);
				line_Objects[lineid][i] = INVALID_OBJECT_ID;
			}
		}
	}

	line_Data[lineid][line_objCount] = count;

	return 1;
}
