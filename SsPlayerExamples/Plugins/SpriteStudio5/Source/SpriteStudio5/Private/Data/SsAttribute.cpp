#include "SpriteStudio5PrivatePCH.h"
#include "SsAttribute.h"
#include "SsString_uty.h"


namespace
{
	static const FName ConstName_Target("target");
	static const FName ConstName_BlendType("blendType");
	static const FName ConstName_LT("LT");
	static const FName ConstName_RT("RT");
	static const FName ConstName_LB("LB");
	static const FName ConstName_RB("RB");
	static const FName ConstName_RGBA("rgba");
	static const FName ConstName_Rate("rate");
	static const FName ConstName_Color("color");
	static const FName ConstName_MapId("mapId");
	static const FName ConstName_Name("name");
	static const FName ConstName_Integer("integer");
	static const FName ConstName_Point("point");
	static const FName ConstName_Rect("rect");
	static const FName ConstName_String("string");
	static const FName ConstName_StartTime("startTime");
	static const FName ConstName_Speed("speed");
	static const FName ConstName_Independent("independent");
	static const FName ConstName_StartLabel("startLabel");
	static const FName ConstName_StartOffset("startOffset");
	static const FName ConstName_EndLabel("endLabel");
	static const FName ConstName_EndOffset("endOffset");
	static const FName ConstName_LoopNum("loopNum");
	static const FName ConstName_Infinity("infinity");
	static const FName ConstName_Reverse("reverse");
	static const FName ConstName_PingPong("pingpong");

}

void FSsKeyframe::Serialize(FArchive& Ar)
{
	Value.Serialize(Ar);
}

void FSsAttribute::Serialize(FArchive& Ar)
{
	for(int32 i = 0; i < Key.Num(); ++i)
	{
		Key[i].Serialize(Ar);
	}
}

const FSsKeyframe*	FSsAttribute::FirstKey()
{
	if ( 0 == Key.Num() )
		return 0;

	return &Key[0];
}

///時間から左側のキーを取得
const FSsKeyframe*	FSsAttribute::FindLeftKey( int time )
{
	if ( 0 == Key.Num() )
		return 0;

	int32 KeyIndex = GetLowerBoundKeyIndex(time);
	if(KeyIndex < 0)
	{
		return &Key[Key.Num()-1];
	}
	FSsKeyframe* Keyframe = &(Key[KeyIndex]);

	if ( Keyframe->Time == time ) return Keyframe;
	if ( 0 == KeyIndex )
	{
		if ( Keyframe->Time > time ) return 0;
	}else{
		--KeyIndex;
		Keyframe = &(Key[KeyIndex]);
	}

	if ( Keyframe->Time > time ) return 0;

	return Keyframe;
}

//時間から右側のキーを取得する
const FSsKeyframe*	FSsAttribute::FindRightKey( int time )
{
	if ( 0 == Key.Num() )
		return 0;

	int32 KeyIndex = GetUpperBoundKeyIndex(time);
	if(KeyIndex < 0 )
	{
		return 0;
	}
	return &Key[KeyIndex];
}


int32 FSsAttribute::GetLowerBoundKeyIndex(int32 Time)
{
	for(int32 i = 0; i < Key.Num(); ++i)
	{
		if(Time <= Key[i].Time)
		{
			return i;
		}
	}
	return -1;
}
int32 FSsAttribute::GetUpperBoundKeyIndex(int32 Time)
{
	for(int32 i = 0; i < Key.Num(); ++i)
	{
		if(Time < Key[i].Time)
		{
			return i;
		}
	}
	return -1;
}


//頂点カラーアニメデータの取得
void	GetSsColorValue( const FSsKeyframe* key , FSsColorAnime& v )
{
	TEnumAsByte<SsColorBlendTarget::Type> target;
	__StringToEnum_( key->Value[ConstName_Target].get<FString>() , target );
	TEnumAsByte<SsBlendType::Type> blendtype;
	__StringToEnum_( key->Value[ConstName_BlendType].get<FString>() , blendtype);

	v.BlendType = blendtype;
	v.Target = target;

	if ( target == SsColorBlendTarget::Vertex )
	{
		SsHash lt = key->Value[ConstName_LT].get<SsHash>();
		SsHash rt = key->Value[ConstName_RT].get<SsHash>();
		SsHash lb = key->Value[ConstName_LB].get<SsHash>();
		SsHash rb = key->Value[ConstName_RB].get<SsHash>();

		ConvertStringToSsColor( lt[ConstName_RGBA].get<FString>() , v.Colors[0].Rgba);
		v.Colors[0].Rate = lt[ConstName_Rate].get<float>();

		ConvertStringToSsColor( rt[ConstName_RGBA].get<FString>() , v.Colors[1].Rgba);
		v.Colors[1].Rate = rt[ConstName_Rate].get<float>();

		ConvertStringToSsColor( lb[ConstName_RGBA].get<FString>() , v.Colors[2].Rgba);
		v.Colors[2].Rate = lb[ConstName_Rate].get<float>();

		ConvertStringToSsColor( rb[ConstName_RGBA].get<FString>() , v.Colors[3].Rgba);
		v.Colors[3].Rate = rb[ConstName_Rate].get<float>();

	}else{
		SsHash color = key->Value[ConstName_Color].get<SsHash>();
		ConvertStringToSsColor( color[ConstName_RGBA].get<FString>() , v.Color.Rgba);
		v.Color.Rate = color[ConstName_Rate].get<float>();
	}

}

void	GetFSsVertexAnime( const FSsKeyframe* key , FSsVertexAnime& v )
{
	const FString& sLT = key->Value[ConstName_LT].get<FString>();
	const FString& sRT = key->Value[ConstName_RT].get<FString>();
	const FString& sLB = key->Value[ConstName_LB].get<FString>();
	const FString& sRB = key->Value[ConstName_RB].get<FString>();
	
	StringToPoint2( sLT , v.Offsets[0] );
	StringToPoint2( sRT , v.Offsets[1] );
	StringToPoint2( sLB , v.Offsets[2] );
	StringToPoint2( sRB , v.Offsets[3] );

}


void GetFSsRefCell( const FSsKeyframe* key , FSsRefCell& v )
{
	int id = key->Value[ConstName_MapId].get<int>();
	FName name = FName( *(key->Value[ConstName_Name].get<FString>()) );

	v.Mapid = id;
	v.Name = name;
}


void	GetSsUserDataAnime( const FSsKeyframe* key , FSsUserDataAnime& v )
{
	v.Integer = 0;
	v.Point.X = v.Point.Y = 0;
	v.Rect.X = v.Rect.Y = v.Rect.W = v.Rect.H = 0; 
	v.String = FString(TEXT(""));
	v.UseInteger = key->Value.IsExistHashkey(ConstName_Integer);
	v.UsePoint = key->Value.IsExistHashkey(ConstName_Point);
	v.UseRect = key->Value.IsExistHashkey(ConstName_Rect);
	v.UseString = key->Value.IsExistHashkey(ConstName_String);

	if ( v.UseInteger )
	{
		v.Integer = key->Value[ConstName_Integer].get<int>();
	}

	if ( v.UsePoint )
	{
		const FString& str = key->Value[ConstName_Point].get<FString>();
		StringToPoint2( str , v.Point );
	}
	
	if ( v.UseRect )
	{
		const FString& str = key->Value[ConstName_Rect].get<FString>();
		StringToIRect( str , v.Rect );
	}

	if ( v.UseString )
	{
		const FString& str = key->Value[ConstName_String].get<FString>();
		v.String = str;
	}

}

void	GetSsEffectParamAnime( const FSsKeyframe* key, FSsEffectAttr& v )
{
	v.StartTime = key->Value[ConstName_StartTime].get<int>();
	v.Speed = key->Value[ConstName_Speed].get<float>();
	v.Independent = key->Value[ConstName_Independent].get<bool>();
	v.CurKeyframe = key->Time;

	int iflags = 0;
	if (v.Independent)
	{
		iflags = iflags | SsEffectLoopFlag::EFFECT_LOOP_FLAG_INFINITY;
	}
	v.LoopFlag = iflags;
}

void	GetSsInstparamAnime( const FSsKeyframe* key , FSsInstanceAttr& v )
{
	const FString& sstartLabel = key->Value[ConstName_StartLabel].get<FString>();
	const int& sstartOffset = key->Value[ConstName_StartOffset].get<int>();
	const FString& sendLabel = key->Value[ConstName_EndLabel].get<FString>();
	const int& sendOffset = key->Value[ConstName_EndOffset].get<int>();

	const float& sspeed = key->Value[ConstName_Speed].get<float>();

	const int& sloopNum = key->Value[ConstName_LoopNum].get<int>();
	const bool& sinfinity = key->Value[ConstName_Infinity].get<bool>();
	const bool& sreverse = key->Value[ConstName_Reverse].get<bool>();
	const bool& spingpong = key->Value[ConstName_PingPong].get<bool>();
	const bool& sindependent = key->Value[ConstName_Independent].get<bool>();


	v.StartLabel = FName(*sstartLabel);
	v.StartOffset = sstartOffset;
	v.EndLabel = FName(*sendLabel);
	v.EndOffset = sendOffset;
	v.Speed = sspeed;

	v.LoopNum = sloopNum;
	v.Infinity = sinfinity;
	v.Reverse = sreverse;
	v.Pingpong = spingpong;
	v.Independent = sindependent;
	v.CurKeyframe = key->Time;


	int iflags = 0;
	if (sinfinity)
	{
		iflags = iflags | SsInstanceLoopFlag::INSTANCE_LOOP_FLAG_INFINITY;
	}
	if (sreverse)
	{
		iflags = iflags | SsInstanceLoopFlag::INSTANCE_LOOP_FLAG_REVERSE;
	}
	if (spingpong)
	{
		iflags = iflags | SsInstanceLoopFlag::INSTANCE_LOOP_FLAG_PINGPONG;
	}
	if (sindependent)
	{
		iflags = iflags | SsInstanceLoopFlag::INSTANCE_LOOP_FLAG_INDEPENDENT;
	}
	v.LoopFlag = iflags;
}

bool StringToPoint2(const FString& str, FVector2D& point)
{
	FString LeftS, RightS;
	if(str.Split(TEXT(" "), &LeftS, &RightS))
	{
		point.X = FCString::Atof(*LeftS);
		point.Y = FCString::Atof(*RightS);
		return true;
	}
	else
	{
		point.X = point.Y = 0.f;
		return false;
	}
}

bool StringToIRect( const FString& str, SsIRect& rect )
{
	TArray<FString>	str_list;
	split_string( str , ' ' , str_list );
	if ( str_list.Num() < 4 )
	{
		rect.X = 0;
		rect.Y = 0;
		rect.W = 0;
		rect.H = 0;
		return false;
	}else{
		rect.X = FCString::Atof(*(str_list[0]));
		rect.Y = FCString::Atof(*(str_list[1]));
		rect.W = FCString::Atof(*(str_list[2]));
		rect.H = FCString::Atof(*(str_list[3]));
	}

	return true;
}
