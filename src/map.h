/// @file map.h
/// @author namazso
/// @date 2017-12-04
/// @brief Data structures describing a map

#pragma once

enum bond_type
{
	Bond_Top			= 1 << 0,
	Bond_TopRight		= 1 << 1,
	Bond_Right			= 1 << 2,
	Bond_BottomRight	= 1 << 3,
	Bond_Bottom			= 1 << 4,
	Bond_BottomLeft		= 1 << 5,
	Bond_Left			= 1 << 6,
	Bond_TopLeft		= 1 << 7,
	Bond_TopDouble		= 1 << 8,
	Bond_RightDouble	= 1 << 9,
	Bond_BottomDouble	= 1 << 10,
	Bond_LeftDouble		= 1 << 11,
	Bond_TopTriple		= 1 << 12,
	Bond_RightTriple	= 1 << 13,
	Bond_BottomTriple	= 1 << 14,
	Bond_LeftTriple		= 1 << 15
};

inline uint16_t bond_char_to_flag(const char c)
{
	switch(c)
	{
	case 'a':  return Bond_Top;
	case 'b':  return Bond_TopRight;
	case 'c':  return Bond_Right;
	case 'd':  return Bond_BottomRight;
	case 'e':  return Bond_Bottom;
	case 'f':  return Bond_BottomLeft;
	case 'g':  return Bond_Left;
	case 'h':  return Bond_TopLeft;
	case 'A':  return Bond_TopDouble;
	case 'B':  return Bond_RightDouble;
	case 'C':  return Bond_BottomDouble;
	case 'D':  return Bond_LeftDouble;
	case 'E':  return Bond_TopTriple;
	case 'F':  return Bond_RightTriple;
	case 'G':  return Bond_BottomTriple;
	case 'H':  return Bond_LeftTriple;
	default: return 0;
	}
}

enum item_kind
{
	Item_AtomHydrogen			= '1',
	Item_AtomCarbon				= '2',
	Item_AtomOxygen				= '3',
	Item_AtomNitrogen			= '4',
	Item_AtomSulfur				= '5',
	Item_AtomFluorine			= '6',
	Item_AtomChlorine			= '7',
	Item_AtomBromine			= '8',
	Item_AtomPhosphorus			= '9',
	Item_AtomCrystal			= 'o',
	Item_ConnectorHorizontal	= 'A',
	Item_ConnectorSlash			= 'B',
	Item_ConnectorVertical		= 'C',
	Item_ConnectorBackslash		= 'D',

	Item_CrystalE				= 'E',
	Item_CrystalF				= 'F',
	Item_CrystalG				= 'G',
	Item_CrystalH				= 'H',
	Item_CrystalI				= 'I',
	Item_CrystalJ				= 'J',
	Item_CrystalK				= 'K',
	Item_CrystalL				= 'L',

	Item_Wall					= '#',
};

struct atom
{
	char item_kind;
	uint16_t bond_flags;
};

struct map
{
	char id[32];
	char name[64];
	struct atom atoms[128];
	char arena[32][32];
	char molecule[16][16];
};
