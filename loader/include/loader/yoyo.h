#pragma once

#include <string>

enum
{
	GML_TYPE_REAL = 0,
	GML_TYPE_STRING = 1,
	GML_TYPE_ARRAY = 2,
	GML_TYPE_POINTER = 3,
	GML_TYPE_UNDEFINED = 5,
	GML_TYPE_OBJECT = 6,
	GML_TYPE_INT32 = 7,
	GML_TYPE_VEC4 = 8,
	GML_TYPE_VEC44 = 9,
	GML_TYPE_INT64 = 10,
	GML_TYPE_ACCESSOR = GML_TYPE_INT64 | GML_TYPE_STRING,
	GML_TYPE_NULL = 12,
	GML_TYPE_BOOL = 13,
	GML_TYPE_ITERATOR = 14,
};

enum
{
	OBJECT_KIND_YYOBJECTBASE = 0,
	OBJECT_KIND_CINSTANCE = 1,
	OBJECT_KIND_ACCESSOR = 2,
	OBJECT_KIND_SCRIPTREF = 3,
	OBJECT_KIND_PROPERTY = 4,
	OBJECT_KIND_ARRAY = 5,
	OBJECT_KIND_WEAKREF = 6,
	OBJECT_KIND_CONTAINER = 7,
	OBJECT_KIND_SEQUENCE = 8,
	OBJECT_KIND_SEQUENCEINSTANCE = 9,
	OBJECT_KIND_SEQUENCETRACK = 10,
	OBJECT_KIND_SEQUENCECURVE = 11,
	OBJECT_KIND_SEQUENCECURVECHANNEL = 12,
	OBJECT_KIND_SEQUENCECURVEPOINT = 13,
	OBJECT_KIND_SEQUENCEKEYFRAMESTORE = 14,
	OBJECT_KIND_SEQUENCEKEYFRAME = 15,
	OBJECT_KIND_SEQUENCEKEYFRAMEDATA = 16,
	OBJECT_KIND_SEQUENCEEVALTREE = 17,
	OBJECT_KIND_SEQUENCEEVALNODE = 18,
	OBJECT_KIND_SEQUENCEEVENT = 19,
	OBJECT_KIND_NINESLICE = 20,
	OBJECT_KIND_FILTERHOST = 21,
	OBJECT_KIND_EFFECTINSTANCE = 22,
	OBJECT_KIND_SKELETON_SKIN = 23,
	OBJECT_KIND_AUDIOBUS = 24,
	OBJECT_KIND_AUDIOEFFECT = 25,
	OBJECT_KIND_MAX = 26
};

template <typename TKey, typename TValue, int TInitialSize>
struct CHashMap;
class CEvent;
struct CPhysicsObject;
struct CSkeletonInstance;
struct CCode;
struct CInstance;
struct CInstancePadded;

struct GMLStringRef
{
	const char* string;
	int refCountGML;
	int size;

	GMLStringRef(const char* str)
	{
		string = str;
		size = std::strlen(str);
		refCountGML = 1;
	}

	void free()
	{
		if (string != NULL)
		{
			delete[] string;
		}
	}
};

#pragma pack(push, 1)
struct RValue
{
	union
	{
		int value_s32; // int32
		long long value_s64; // int64
		double value_real; // number
		GMLStringRef* value_stringref; // string
		void* value_array; // array
		void* value_ptr; // ptr
	};

	int flags = 0; // Not sure what this is used for
	int	type = GML_TYPE_UNDEFINED;

	inline void set_real(double value)
	{
		free_stringref();
		type = GML_TYPE_REAL;
		value_real = value;
	}

	double get_s32()
	{
		switch (type)
		{
		case GML_TYPE_INT32: return value_s32;
		case GML_TYPE_INT64: return (int)(value_s64 & 0xffffffff);
		case GML_TYPE_BOOL: return (int)(value_s32 > 0);
		case GML_TYPE_REAL: return (int)value_real;
		case GML_TYPE_NULL: return 0;
		}

		return 0;
	}

	inline void set_s32(int value)
	{
		free_stringref();
		type = GML_TYPE_INT32;
		value_s32 = value;
	}

	inline void set_s64(long long value)
	{
		free_stringref();
		type = GML_TYPE_INT64;
		value_s64 = value;
	}

	inline void set_bool(bool value)
	{
		free_stringref();
		type = GML_TYPE_BOOL;
		value_real = value ? 1 : 0;
	}

	inline void set_stringref(const char* value)
	{
		free_stringref();
		type = GML_TYPE_STRING;
		value_stringref = new GMLStringRef(_strdup(value));
	}

	inline void free_and_undefine()
	{
		free_stringref();
		type = GML_TYPE_UNDEFINED;
		value_ptr = NULL;
	}

	double get_real()
	{
		switch (type)
		{
		case GML_TYPE_INT32: return (double)value_s32;
		case GML_TYPE_INT64: return (double)value_s64;
		case GML_TYPE_BOOL: return (double)(value_s32 > 0);
		case GML_TYPE_REAL: return value_real;
		case GML_TYPE_NULL: return 0;
		}

		return 0;
	}

	const char* get_cstr()
	{
		if (type == GML_TYPE_STRING)
			if (value_stringref->string != nullptr)
			{
				return value_stringref->string;
			}

		return "";
	}

	inline std::string get_string()
	{
		return std::string(get_cstr());
	}

	RValue() { value_s64 = NULL; }

	RValue(double value)
	{
		value_s64 = NULL;
		set_real(value);
	}

	RValue(float value)
	{
		value_s64 = NULL;
		set_real(value);
	}

	RValue(int value)
	{
		value_s64 = NULL;
		set_s32(value);
	}

	RValue(long long value)
	{
		value_s64 = NULL;
		set_s64(value);
	}

	RValue(bool value)
	{
		value_s64 = NULL;
		set_real(value ? 1 : 0);
	}

	RValue(const char* value)
	{
		type = GML_TYPE_STRING;
		value_stringref = new GMLStringRef(_strdup(value));
	}

	RValue(std::string value)
	{
		type = GML_TYPE_STRING;
		value_stringref = new GMLStringRef(_strdup(value.c_str()));
	}

	std::string as_string()
	{
		switch (type)
		{
		case GML_TYPE_REAL: return std::to_string(value_real);
		case GML_TYPE_BOOL: return truthy() ? "true" : "false";
		case GML_TYPE_INT32: return std::to_string(value_s32);
		case GML_TYPE_INT64: return std::to_string(value_s64);
		case GML_TYPE_POINTER: return "*" + std::to_string((int64_t)value_ptr);
		case GML_TYPE_UNDEFINED: return "<undefined>";
		case GML_TYPE_STRING: return value_stringref->string;
		case GML_TYPE_ARRAY: return "array";
		default: return "unknown";
		}
	}

	const char* as_cstr()
	{
		char ret[32];

		switch (type)
		{
		case GML_TYPE_REAL: sprintf_s(ret, "%d", value_real); break;
		case GML_TYPE_BOOL: return truthy() ? "true" : "false"; break;
		case GML_TYPE_INT32: sprintf_s(ret, "%i", value_s32); break;
		case GML_TYPE_INT64: sprintf_s(ret, "%l", value_s64); break;
		case GML_TYPE_POINTER: sprintf_s(ret, "*%08x", value_ptr); break;
		case GML_TYPE_UNDEFINED: return "<undefined>"; break;
		case GML_TYPE_STRING: return value_stringref->string;  break;
		case GML_TYPE_ARRAY: return "<array>"; break;
		default: return "unknown";
		}

		return ret;
	}

	inline bool truthy()
	{
		// Returns whether the value casts to true in GML
		return value_real > 0.5;
	}

	inline void free_stringref()
	{
		if (type == GML_TYPE_STRING)
		{
			if (value_stringref->string)
			{
				value_stringref->free();
				delete value_stringref;
			}

			type = GML_TYPE_UNDEFINED;
			value_ptr = NULL;
		}
	}

};

// @deprecated use RValue instead
using GMLVar = RValue;

struct CInstanceBase
{
	RValue* m_YYVars;
};
static_assert(sizeof(CInstanceBase) == 0x4, "sizeof(CInstance) != 0x4");

template <typename T>
struct LinkedList
{
	T* m_First;
	T* m_Last;
	int32_t m_Count;
};
static_assert(sizeof(LinkedList<char>) == 0xc, "sizeof(LinkedList<>) != 0xc");

struct CPhysicsDataGM
{
	float* m_PhysicsVertices;
	bool m_IsPhysicsObject;
	bool m_IsPhysicsSensor;
	bool m_IsPhysicsAwake;
	bool m_IsPhysicsKinematic;
	int m_PhysicsShape;
	int m_PhysicsGroup;
	float m_PhysicsDensity;
	float m_PhysicsRestitution;
	float m_PhysicsLinearDamping;
	float m_PhysicsAngularDamping;
	float m_PhysicsFriction;
	int m_PhysicsVertexCount;
};
static_assert(sizeof(CPhysicsDataGM) == 0x28, "sizeof(CPhysicsDataGM) != 0x28");

struct CObjectGM
{
	const char* m_Name;
	CObjectGM* m_ParentObject;
	CHashMap<int, CObjectGM*, 2>* m_ChildrenMap;
	CHashMap<int64_t, CEvent*, 3>* m_EventsMap;
	CPhysicsDataGM m_PhysicsData;
	LinkedList<CInstancePadded> m_Instances;
	LinkedList<CInstancePadded> m_InstancesRecursive;
	uint32_t m_Flags;
	int32_t m_SpriteIndex;
	int32_t m_Depth;
	int32_t m_Parent;
	int32_t m_Mask;
	int32_t m_ID;
};
static_assert(sizeof(CObjectGM) == 0x68, "sizeof(CObjectGM) != 0x68");

struct SLink
{
	void* first;
	void* last;
	void* list;
};
static_assert(sizeof(SLink) == 0xc, "sizeof(SLink) != 0xc");

struct tagYYRECT
{
	float left;
	float right;
	float top;
	float bottom;
};

struct YYObjectBase
{ /* PlaceHolder Structure */
	void* vtable;
	struct CInstanceBase base;
	struct YYObjectBase* m_pNextObject;
	struct YYObjectBase* m_pPrevObject;
	struct YYObjectBase* m_prototype;
	char* m_Class;
	void* m_GetProperty;
	void* m_DeleteOwnProperty;
	void* m_DefineOwnProperty;
	struct CHashMap<int, RValue*, 3>* m_YYVarsMap;
	void* m_WeakRef;
	uint32_t m_WeakRefCount;
	uint32_t m_VariableCount;
	uint32_t m_Flags;
	uint32_t m_Capacity;
	uint32_t m_Visited;
	uint32_t m_VisitedGC;
	int32_t m_GCGeneration;
	int32_t m_GCCreationFrame;
	int32_t m_Slot;
	int32_t m_ObjectKind;
	int32_t m_RValueInitType;
	int32_t m_CurrentSlot;
};
static_assert(sizeof(YYObjectBase) == 0x5c, "sizeof(YYObjectBase) != 0x5c");

struct CInstance : YYObjectBase
{
	uint32_t _unk_0x5c;
	uint64_t create_counter;
	CObjectGM* object;
	CPhysicsObject* physics_object;
	CSkeletonInstance* skeleton_animation;
	void* sequence_instance;
	int id; /* Created by Rename Structure Field action */
	int object_index;
	int sprite_id;
	float image_index;
	float image_speed; /* Created by Rename Structure Field action */
	float image_scale_x;
	float image_scale_y;
	float image_angle;
	float image_alpha; /* Created by Rename Structure Field action */
	uint32_t image_blend; /* Created by Rename Structure Field action */
	float x;
	float y;
	float x_start; /* Created by Rename Structure Field action */
	float y_start; /* Created by Rename Structure Field action */
	float x_previous; /* Created by Rename Structure Field action */
	float y_previous; /* Created by Rename Structure Field action */
	float direction; /* Created by Rename Structure Field action */
	float speed;
	float friction;
	float gravity_dir;
	float gravity;
	float hspeed;
	float vspeed;
	tagYYRECT bbox;
	int timers[12];
	int* timeline_path;
	CCode* init_code;
	CCode* precreate_code;
	CObjectGM* old_object;
	int32_t layer_id;
	int32_t mask_index;
	int16_t mouse_over_count;
	CInstance* next;
	CInstance* prev;
	SLink collision_link;
	SLink dirty_link;
	SLink with_link;
	float depth;
	float current_depth;
	float last_image_number;
	uint32_t collision_test_number;
};
static_assert(sizeof(CInstance) == 0x16a, "sizeof(CInstance) != 0x16a");

// i dont know why this is the way it is
struct CInstancePadded
{
	uint32_t padding_0x00;
	uint32_t padding_0x04;
	CInstance* instance;
};
static_assert(sizeof(CInstancePadded) == 0xc, "sizeof(CInstancePadded) != 0xc");
#pragma pack(pop)

// @deprecated use CInstance instead
using GMLInstance = CInstance;

typedef RValue& (*PFUNC_YYGMLScript)(
	_In_ CInstance* self,
	_In_ CInstance* other,
	_Out_ RValue& out,
	_In_ int argc,
	_In_ RValue* argv
	);

typedef void (*PFUNC_YYGML)(
	_In_ CInstance* self,
	_In_ CInstance* other
	);

typedef void (*PFUNC_RAW)();

using FNVariable = bool(*)(
	CInstance* Instance,
	int Index,
	RValue* Value
	);

// opaque structs
struct CBackGM;
struct CViewGM;
struct CPhysicsWorld;
struct RTile;
struct YYRoomTiles;
struct YYRoomInstances;
struct CLayerEffectInfo;

// Represents a GameMaker layer.
struct CLayer;

// Represents a layer element. Can either be an instance, or a sprite.
struct CLayerElementBase;

// Represents a layer element. Refers to an instance.
struct CLayerInstanceElement;

// Represents a layer element. Refers to a sprite.
struct CLayerSpriteElement;

struct RVariableRoutine
{
	const char* m_Name;
	FNVariable m_GetVariable;
	FNVariable m_SetVariable;
	bool m_CanBeSet;
};
static_assert(sizeof(RVariableRoutine) == 0x10, "sizeof(RVariableRoutine) != 0x10");

struct RToken
{
	int m_Kind;
	unsigned int m_Type;
	int m_Ind;
	int m_Ind2;
	RValue m_Value;
	int m_ItemNumber;
	RToken* m_Items;
	int m_Position;
};
static_assert(sizeof(RToken) == 0x2c, "sizeof(RToken) != 0x2c");

struct YYGMLFuncs
{
	const char* m_Name;
	union
	{
		PFUNC_YYGMLScript m_ScriptFunction;
		PFUNC_YYGML m_CodeFunction;
		PFUNC_RAW m_RawFunction;
	};
	void* m_FunctionVariables; // YYVAR
};
static_assert(sizeof(YYGMLFuncs) == 0x0c, "sizeof(YYGMLFuncs) != 0x0c");

struct CCode
{
	int (**_vptr$CCode)(void);
	CCode* m_Next;
	int m_Kind;
	int m_Compiled;
	const char* m_Str;
	RToken m_Token;
	RValue m_Value;
	void* m_VmInstance;
	void* m_VmDebugInfo;
	char* m_Code;
	const char* m_Name;
	int m_CodeIndex;
	YYGMLFuncs* m_Functions;
	bool m_Watch;
	int m_Offset;
	int m_LocalsCount;
	int m_ArgsCount;
	int m_Flags;
	YYObjectBase* m_Prototype;
};
static_assert(sizeof(CCode) == 0x80, "sizeof(YYGMLFuncs) != 0x80");

struct YYRoom
{
	// The name of the room
	uint32_t m_NameOffset;
	// The caption of the room, legacy variable, used pre-GMS
	uint32_t m_Caption;
	// The width of the room
	int32_t m_Width;
	// The height of the room
	int32_t m_Height;
	// Speed of the room
	int32_t m_Speed;
	// Whether the room is persistent (UMT marks it as a bool, but it seems to be int32_t)
	int32_t m_Persistent;
	// The background color
	int32_t m_Color;
	// Whether to show the background color
	int32_t m_ShowColor;
	// Creation code of the room
	uint32_t m_CreationCode;
	int32_t m_EnableViews;
	uint32_t pBackgrounds;
	uint32_t pViews;
	uint32_t pInstances;
	uint32_t pTiles;
	int32_t m_PhysicsWorld;
	int32_t m_PhysicsWorldTop;
	int32_t m_PhysicsWorldLeft;
	int32_t m_PhysicsWorldRight;
	int32_t m_PhysicsWorldBottom;
	float m_PhysicsGravityX;
	float m_PhysicsGravityY;
	float m_PhysicsPixelToMeters;
};
static_assert(sizeof(YYRoom) == 0x58, "sizeof(YYRoom) != 0x58");

// Note: this is not how RValue store arrays
template <typename T>
struct CArrayStructure
{
	int32_t Length;
	T* Array;
};
static_assert(sizeof(CArrayStructure<char>) == 0x08, "sizeof(CArrayStructure) != 0x08");

struct CLayerElementBase
{
	int32_t m_Type;
	int32_t m_ID;
	bool m_RuntimeDataInitialized;
	const char* m_Name;
	CLayer* m_Layer;
	union
	{
		CLayerInstanceElement* m_InstanceFlink;
		CLayerSpriteElement* m_SpriteFlink;
		CLayerElementBase* m_Flink;
	};
	union
	{
		CLayerInstanceElement* m_InstanceBlink;
		CLayerSpriteElement* m_SpriteBlink;
		CLayerElementBase* m_Blink;
	};
};
static_assert(sizeof(CLayerElementBase) == 0x1c, "sizeof(CLayerElementBase) != 0x1c");

struct CLayerSpriteElement : CLayerElementBase
{
	int32_t m_SpriteIndex;
	float m_SequencePosition;
	float m_SequenceDirection;
	float m_ImageIndex;
	float m_ImageSpeed;
	int32_t m_SpeedType;
	float m_ImageScaleX;
	float m_ImageScaleY;
	float m_ImageAngle;
	uint32_t m_ImageBlend;
	float m_ImageAlpha;
	float m_X;
	float m_Y;
};
static_assert(sizeof(CLayerSpriteElement) == 0x50, "sizeof(CLayerSpriteElement) != 0x50");

struct CLayerInstanceElement : CLayerElementBase
{
	int32_t m_InstanceID;
	CInstance* m_Instance;
};
static_assert(sizeof(CLayerInstanceElement) == 0x24, "sizeof(CLayerInstanceElement) != 0x24");

struct CLayer
{
	int32_t m_Id;
	int32_t m_Depth;
	float m_XOffset;
	float m_YOffset;
	float m_HorizontalSpeed;
	float m_VerticalSpeed;
	bool m_Visible;
	bool m_Deleting;
	bool m_Dynamic;
	const char* m_Name;
	RValue m_BeginScript;
	RValue m_EndScript;
	bool m_EffectEnabled;
	bool m_EffectPendingEnabled;
	RValue m_Effect;
	CLayerEffectInfo* m_InitialEffectInfo;
	int32_t m_ShaderID;
	LinkedList<CLayerElementBase> m_Elements;
	CLayer* m_Flink;
	CLayer* m_Blink;
	void* m_GCProxy;
};
static_assert(sizeof(CLayer) == 0x74, "sizeof(CLayer) != 0x74");

using CHashMapHash = uint32_t;

template<typename TKey, typename TValue>
struct CHashMapElement
{
	TValue m_Value;
	TKey m_Key;
	CHashMapHash m_Hash;
};

template <typename TKey, typename TValue, int TInitialMask>
struct CHashMap
{
private:
	// Typed functions for calculating hashes
	static CHashMapHash CHashMapCalculateHash(
		int Key
	)
	{
		return (Key * -0x61c8864f + 1) & INT_MAX;
	}

	static CHashMapHash CHashMapCalculateHash(
		YYObjectBase* Key
	)
	{
		return ((static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(Key)) >> 6) * 7 + 1) & INT_MAX;
	}

	static CHashMapHash CHashMapCalculateHash(
		void* Key
	)
	{
		return ((static_cast<unsigned long long>(reinterpret_cast<uintptr_t>(Key)) >> 8) + 1) & INT_MAX;
	};

	static CHashMapHash CHashMapCalculateHash(
		const char* Key
	)
	{
		// https://github.com/jwerle/murmurhash.c - Licensed under MIT
		size_t len = strlen(Key);
		uint32_t c1 = 0xcc9e2d51;
		uint32_t c2 = 0x1b873593;
		uint32_t r1 = 15;
		uint32_t r2 = 13;
		uint32_t m = 5;
		uint32_t n = 0xe6546b64;
		uint32_t h = 0;
		uint32_t k = 0;
		uint8_t* d = (uint8_t*)Key; // 32 bit extract from 'key'
		const uint32_t* chunks = NULL;
		const uint8_t* tail = NULL; // tail - last 8 bytes
		int i = 0;
		int l = len / 4; // chunk length

		chunks = (const uint32_t*)(d + l * 4); // body
		tail = (const uint8_t*)(d + l * 4); // last 8 byte chunk of `key'

		// for each 4 byte chunk of `key'
		for (i = -l; i != 0; ++i)
		{
			// next 4 byte chunk of `key'
			k = chunks[i];

			// encode next 4 byte chunk of `key'
			k *= c1;
			k = (k << r1) | (k >> (32 - r1));
			k *= c2;

			// append to hash
			h ^= k;
			h = (h << r2) | (h >> (32 - r2));
			h = h * m + n;
		}

		k = 0;

		// remainder
		switch (len & 3) { // `len % 4'
		case 3: k ^= (tail[2] << 16);
		case 2: k ^= (tail[1] << 8);

		case 1:
			k ^= tail[0];
			k *= c1;
			k = (k << r1) | (k >> (32 - r1));
			k *= c2;
			h ^= k;
		}

		h ^= len;

		h ^= (h >> 16);
		h *= 0x85ebca6b;
		h ^= (h >> 13);
		h *= 0xc2b2ae35;
		h ^= (h >> 16);

		return h;
	}

public:
	int32_t m_CurrentSize;
	int32_t m_UsedCount;
	int32_t m_CurrentMask;
	int32_t m_GrowThreshold;
	CHashMapElement<TKey, TValue>* m_Elements;
	void(*m_DeleteValue)(TKey* Key, TValue* Value);

	bool GetContainer(
		TKey Key,
		CHashMapElement<TKey, TValue>*& Value
	)
	{
		CHashMapHash value_hash = CHashMapCalculateHash(Key);
		int32_t ideal_position = static_cast<int>(value_hash & m_CurrentMask);

		for (
			// Start at the ideal element (the value is probably not here though)
			CHashMapElement<TKey, TValue>& current_element = this->m_Elements[ideal_position];
			// Continue looping while the hash isn't 0 (meaning we reached the end of the map)
			current_element.m_Hash != 0;
			// Go to the next position
			current_element = this->m_Elements[(++ideal_position) & this->m_CurrentMask]
			)
		{
			if (current_element.m_Key != Key)
				continue;

			Value = &current_element;
			return true;
		}

		return false;
	}

	bool GetValue(
		TKey Key,
		TValue& Value
	)
	{
		// Try to get the container
		CHashMapElement<TKey, TValue>* object_container = nullptr;
		if (!this->GetContainer(
			Key,
			object_container
		))
		{
			return false;
		}

		Value = object_container->m_Value;
		return true;
	}
};

struct CRoomInternal
{
	// CBackGM* m_Backgrounds[8];
	bool m_EnableViews;
	bool m_ClearScreen;
	bool m_ClearDisplayBuffer;
	CViewGM* m_Views[8];
	const char* m_LegacyCode;
	CCode* m_CodeObject;
	bool m_HasPhysicsWorld;
	int32_t m_PhysicsGravityX;
	int32_t m_PhysicsGravityY;
	float m_PhysicsPixelToMeters;
	LinkedList<CInstance> m_ActiveInstances;
	LinkedList<CInstance> m_InactiveInstances;
	CInstance* m_MarkedFirst;
	CInstance* m_MarkedLast;
	int32_t* m_CreationOrderList;
	int32_t m_CreationOrderListSize;
	YYRoom* m_WadRoom;
	void* m_WadBaseAddress;
	CPhysicsWorld* m_PhysicsWorld;
	int32_t m_TileCount;
	CArrayStructure<RTile> m_Tiles;
	YYRoomTiles* m_WadTiles;
	YYRoomInstances* m_WadInstances;
	const char* m_Name;
	bool m_IsDuplicate;
	LinkedList<CLayer> m_Layers;
	CHashMap<int32_t, CLayer*, 7> m_LayerLookup;
	CHashMap<int32_t, CLayerElementBase*, 7> m_LayerElementLookup;
	CLayerElementBase* m_LastElementLookedUp;
	CHashMap<int32_t, CLayerInstanceElement*, 7> m_InstanceElementLookup;
	int32_t* m_SequenceInstanceIDs;
	int32_t m_SequenceInstanceIdCount;
	int32_t m_SequenceInstanceIdMax;
	int32_t* m_EffectLayerIDs;
	int32_t m_EffectLayerIdCount;
	int32_t m_EffectLayerIdMax;
};
static_assert(sizeof(CRoomInternal) == 0xfc, "sizeof(CRoomInternal) != 0xfc");

struct CRoom
{
	int32_t m_LastTile;
	CRoom* m_InstanceHandle;
	const char* m_Caption;
	int32_t m_Speed;
	int32_t m_Width;
	int32_t m_Height;
	bool m_Persistent;
	uint32_t m_Color;
	bool m_ShowColor;

	// Last confirmed use in 2023.8, might be later even
	struct
	{
		CBackGM* Backgrounds[8];
		CRoomInternal Internals;
	} WithBackgrounds;
};
static_assert(sizeof(CRoom) == 0x140, "sizeof(CRoom) != 0x140");