﻿#include "SpriteStudio5PrivatePCH.h"

//---------------------------------------------------------------
//相互変換 SsPartType
FString	__EnumToString_( TEnumAsByte<SsPartType::Type> n )
{
	if ( SsPartType::Invalid ) return "invalid";
	if ( SsPartType::Null ) return "null";
	if ( SsPartType::Normal ) return "normal";
	if ( SsPartType::Text ) return "text";
	if ( SsPartType::Instance ) return "instance";
	return "invalid";	
}

void 	__StringToEnum_( FString n , TEnumAsByte<SsPartType::Type>& out)
{
	out =  SsPartType::Invalid;
	if ( n == "invalid") out = SsPartType::Invalid;
	if ( n == "null") out = SsPartType::Null;
	if ( n == "normal") out = SsPartType::Normal;
	if ( n == "text") out = SsPartType::Text;
	if ( n == "instance") out = SsPartType::Instance;
}

//---------------------------------------------------------------
//相互変換 SsPartsSortMode
FString	__EnumToString_( TEnumAsByte<SsPartsSortMode::Type> n )
{
	if ( SsPartsSortMode::Invalid ) return "invalid";
	if ( SsPartsSortMode::Prio ) return "prio";
	if ( SsPartsSortMode::Z ) return "z";
	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsPartsSortMode::Type> &out )
{
	out = SsPartsSortMode::Invalid;
	if ( n == "invalid") out = SsPartsSortMode::Invalid;
	if ( n == "prio") out = SsPartsSortMode::Prio;
	if ( n == "z") out = SsPartsSortMode::Z;
}

//---------------------------------------------------------------
//相互変換 SsBoundsType
FString	__EnumToString_( TEnumAsByte<SsBoundsType::Type> n )
{
	if ( SsBoundsType::Invalid ) return "invalid";
	if ( SsBoundsType::None ) return "none";
	if ( SsBoundsType::Quad ) return "quad";
	if ( SsBoundsType::Aabb ) return "aabb";
	if ( SsBoundsType::Circle ) return "circle";
	if ( SsBoundsType::CircleSmin ) return "circle_smin";
	if ( SsBoundsType::CircleSmax ) return "circle_smax";
	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsBoundsType::Type> &out )
{
	out = SsBoundsType::Invalid;
	if ( n == "invalid") out = SsBoundsType::Invalid;
	if ( n == "none") out = SsBoundsType::None;
	if ( n == "quad") out = SsBoundsType::Quad;
	if ( n == "aabb") out = SsBoundsType::Aabb;
	if ( n == "circle") out = SsBoundsType::Circle;
	if ( n == "circle_smin") out = SsBoundsType::CircleSmin;
	if ( n == "circle_smax") out = SsBoundsType::CircleSmax;
}


//---------------------------------------------------------------
//相互変換 SsBoundsType
FString	__EnumToString_( TEnumAsByte<SsInheritType::Type> n )
{
	if ( SsInheritType::Invalid ) return "invalid";
	if ( SsInheritType::Parent ) return "parent";
	if ( SsInheritType::Self ) return "self";
	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsInheritType::Type> &out )
{
	out = SsInheritType::Invalid;
	if ( n == "invalid") out = SsInheritType::Invalid;
	if ( n == "parent") out = SsInheritType::Parent;
	if ( n == "self") out = SsInheritType::Self;
}


//---------------------------------------------------------------
//相互変換 SsBlendType
FString	__EnumToString_( TEnumAsByte<SsBlendType::Type> n )
{
	if ( SsBlendType::Invalid ) return "invalid";
	if ( SsBlendType::Mix ) return "mix";
	if ( SsBlendType::Mul ) return "mul";
	if ( SsBlendType::Add ) return "add";
	if ( SsBlendType::Sub ) return "sub";

	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsBlendType::Type> &out )
{
	out = SsBlendType::Invalid;
	if ( n == "invalid") out = SsBlendType::Invalid;
	if ( n == "mix") out = SsBlendType::Mix;
	if ( n == "mul") out = SsBlendType::Mul;
	if ( n == "add") out = SsBlendType::Add;
	if ( n == "sub") out = SsBlendType::Sub;
}




//---------------------------------------------------------------
//相互変換 SsInterpolationType
FString	__EnumToString_( TEnumAsByte<SsInterpolationType::Type> n )
{
	if ( SsInterpolationType::Invalid )		return "invalid";
	if ( SsInterpolationType::None )		return "none";
	if ( SsInterpolationType::Linear )		return "linear";
	if ( SsInterpolationType::Hermite )		return "hermite";
	if ( SsInterpolationType::Bezier )		return "bezier";
	if ( SsInterpolationType::Acceleration ) return "acceleration";
	if ( SsInterpolationType::Deceleration ) return "deceleration";

	return "none";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsInterpolationType::Type> &out )
{
	out = SsInterpolationType::None;
	if ( n == "invalid") out = SsInterpolationType::Invalid;
	if ( n == "none") out = SsInterpolationType::None;
	if ( n == "linear") out = SsInterpolationType::Linear;
	if ( n == "hermite") out = SsInterpolationType::Hermite;
	if ( n == "bezier") out = SsInterpolationType::Bezier;
	if ( n == "acceleration") out = SsInterpolationType::Acceleration;
	if ( n == "deceleration") out = SsInterpolationType::Deceleration;
}

//---------------------------------------------------------------
//相互変換 SsTexWrapMode
FString	__EnumToString_( TEnumAsByte<SsTexWrapMode::Type> n )
{
	if ( SsTexWrapMode::Invalid )		return "invalid";
	if ( SsTexWrapMode::Clamp )		return "clamp";
	if ( SsTexWrapMode::Repeat )		return "repeat";
	if ( SsTexWrapMode::Mirror )		return "mirror";

	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsTexWrapMode::Type> &out )
{
	out = SsTexWrapMode::Invalid;
	if ( n == "invalid") out = SsTexWrapMode::Invalid;
	if ( n == "clamp") out = SsTexWrapMode::Clamp;
	if ( n == "repeat") out = SsTexWrapMode::Repeat;
	if ( n == "mirror") out = SsTexWrapMode::Mirror;

}

//---------------------------------------------------------------
//相互変換 SsTexFilterMode
FString	__EnumToString_( TEnumAsByte<SsTexFilterMode::Type> n )
{
	if ( SsTexFilterMode::Invalid )		return "invalid";
	if ( SsTexFilterMode::Nearest )		return "nearest";
	if ( SsTexFilterMode::Linear )		return "linear";

	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsTexFilterMode::Type> &out )
{
	out = SsTexFilterMode::Invalid;
	if ( n == "invalid") out = SsTexFilterMode::Invalid;
	if ( n == "nearest") out = SsTexFilterMode::Nearest;
	if ( n == "linear") out = SsTexFilterMode::Linear;

}

//---------------------------------------------------------------
//相互変換 SsTexFilterMode
FString	__EnumToString_( TEnumAsByte<SsColorBlendTarget::Type> n )
{
	if ( SsColorBlendTarget::Invalid )		return "invalid";
	if ( SsColorBlendTarget::Whole )		return "whole";
	if ( SsColorBlendTarget::Vertex )		return "vertex";

	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsColorBlendTarget::Type> &out )
{
	out = SsColorBlendTarget::Invalid;
	if ( n == "invalid") out = SsColorBlendTarget::Invalid;
	if ( n == "whole") out = SsColorBlendTarget::Whole;
	if ( n == "vertex") out = SsColorBlendTarget::Vertex;

}

//---------------------------------------------------------------
//相互変換 SsAttributeKind
FString	__EnumToString_( TEnumAsByte<SsAttributeKind::Type> n )
{
	if ( SsAttributeKind::Invalid )		return "invalid";
	if ( SsAttributeKind::Cell )		return "CELL";
	if ( SsAttributeKind::Posx )		return "POSX";
	if ( SsAttributeKind::Posy )		return "POSY";
	if ( SsAttributeKind::Posz )		return "POSZ";
	if ( SsAttributeKind::Rotx )		return "ROTX";
	if ( SsAttributeKind::Roty )		return "ROTY";
	if ( SsAttributeKind::Rotz )		return "ROTZ";
	if ( SsAttributeKind::Sclx )		return "SCLX";
	if ( SsAttributeKind::Scly )		return "SCLY";
	if ( SsAttributeKind::Alpha )		return "ALPH";
	if ( SsAttributeKind::Prio )		return "PRIO";
	if ( SsAttributeKind::Fliph )		return "FLPH";
	if ( SsAttributeKind::Flipv )		return "FLPV";
	if ( SsAttributeKind::Hide )		return "HIDE";
	if ( SsAttributeKind::Color )		return "VCOL";
	if ( SsAttributeKind::Vertex )		return "VERT";
	if ( SsAttributeKind::Pivotx )		return "PVTX";
	if ( SsAttributeKind::Pivoty )		return "PVTY";
	if ( SsAttributeKind::Anchorx )		return "ANCX";
	if ( SsAttributeKind::Anchory )		return "ANCY";
	if ( SsAttributeKind::Sizex )		return "SIZX";
	if ( SsAttributeKind::Sizey )		return "SIZY";
	if ( SsAttributeKind::Imgfliph )	return "IFLH";
	if ( SsAttributeKind::Imgflipv )	return "IFLV";
	if ( SsAttributeKind::Uvtx )		return "UVTX";
	if ( SsAttributeKind::Uvty )		return "UVTY";
	if ( SsAttributeKind::Uvrz )		return "UVRZ";
	if ( SsAttributeKind::Uvsx )		return "UVSX";
	if ( SsAttributeKind::Uvsy )		return "UVSY";
	if ( SsAttributeKind::Boundr )		return "BNDR";
	if ( SsAttributeKind::User )		return "USER";
	if ( SsAttributeKind::Instance )	return "IPRM";

	return "invalid";	
}

void	__StringToEnum_( FString n , TEnumAsByte<SsAttributeKind::Type> &out )
{

	out = SsAttributeKind::Invalid;
	if ( n == "invalid") out = SsAttributeKind::Invalid;
	if ( n == "CELL") out = SsAttributeKind::Cell;
	if ( n == "POSX") out = SsAttributeKind::Posx;
	if ( n == "POSY") out = SsAttributeKind::Posy;
	if ( n == "POSZ") out = SsAttributeKind::Posz;
	if ( n == "ROTX") out = SsAttributeKind::Rotx;
	if ( n == "ROTY") out = SsAttributeKind::Roty;
	if ( n == "ROTZ") out = SsAttributeKind::Rotz;
	if ( n == "SCLX") out = SsAttributeKind::Sclx;
	if ( n == "SCLY") out = SsAttributeKind::Scly;
	if ( n == "ALPH") out = SsAttributeKind::Alpha;
	if ( n == "PRIO") out = SsAttributeKind::Prio;
	if ( n == "FLPH") out = SsAttributeKind::Fliph;
	if ( n == "FLPV") out = SsAttributeKind::Flipv;
	if ( n == "HIDE") out = SsAttributeKind::Hide;
	if ( n == "VCOL") out = SsAttributeKind::Color;
	if ( n == "VERT") out = SsAttributeKind::Vertex;
	if ( n == "PVTX") out = SsAttributeKind::Pivotx;
	if ( n == "PVTY") out = SsAttributeKind::Pivoty;
	if ( n == "ANCX") out = SsAttributeKind::Anchorx;
	if ( n == "ANCY") out = SsAttributeKind::Anchory;
	if ( n == "SIZX") out = SsAttributeKind::Sizex;
	if ( n == "SIZY") out = SsAttributeKind::Sizey;
	if ( n == "IFLH") out = SsAttributeKind::Imgfliph;
	if ( n == "IFLV") out = SsAttributeKind::Imgflipv;
	if ( n == "UVTX") out = SsAttributeKind::Uvtx;
	if ( n == "UVTY") out = SsAttributeKind::Uvty;
	if ( n == "UVRZ") out = SsAttributeKind::Uvrz;
	if ( n == "UVSX") out = SsAttributeKind::Uvsx;
	if ( n == "UVSY") out = SsAttributeKind::Uvsy;
	if ( n == "BNDR") out = SsAttributeKind::Boundr;
	if ( n == "USER") out = SsAttributeKind::User;
	if ( n == "IPRM") out = SsAttributeKind::Instance;

}


