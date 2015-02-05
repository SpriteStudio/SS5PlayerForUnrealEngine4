#include "SpriteStudio5PrivatePCH.h"
#include "SsAttribute.h"
#include "SsString_uty.h"


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

///���Ԃ��獶���̃L�[���擾
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

//���Ԃ���E���̃L�[���擾����
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


//���_�J���[�A�j���f�[�^�̎擾
void	GetSsColorValue( const FSsKeyframe* key , FSsColorAnime& v )
{
	TEnumAsByte<SsColorBlendTarget::Type> target;
	__StringToEnum_( key->Value["target"].get<FString>() , target );
	TEnumAsByte<SsBlendType::Type> blendtype;
	__StringToEnum_( key->Value["blendType"].get<FString>() , blendtype);

	v.BlendType = blendtype;
	v.Target = target;

	if ( target == SsColorBlendTarget::Vertex )
	{
		SsHash lt = key->Value["LT"].get<SsHash>();
		SsHash rt = key->Value["RT"].get<SsHash>();
		SsHash lb = key->Value["LB"].get<SsHash>();
		SsHash rb = key->Value["RB"].get<SsHash>();

		ConvertStringToSsColor( lt["rgba"].get<FString>() , v.Colors[0].Rgba);
		v.Colors[0].Rate = lt["rate"].get<float>();

		ConvertStringToSsColor( rt["rgba"].get<FString>() , v.Colors[1].Rgba);
		v.Colors[1].Rate = rt["rate"].get<float>();

		ConvertStringToSsColor( lb["rgba"].get<FString>() , v.Colors[2].Rgba);
		v.Colors[2].Rate = lb["rate"].get<float>();

		ConvertStringToSsColor( rb["rgba"].get<FString>() , v.Colors[3].Rgba);
		v.Colors[3].Rate = rb["rate"].get<float>();

	}else{
		SsHash color = key->Value["color"].get<SsHash>();
		ConvertStringToSsColor( color["rgba"].get<FString>() , v.Color.Rgba);
		v.Color.Rate = color["rate"].get<float>();
	}

}

void	GetFSsVertexAnime( const FSsKeyframe* key , FSsVertexAnime& v )
{
	const FString& sLT = key->Value["LT"].get<FString>();
	const FString& sRT = key->Value["RT"].get<FString>();
	const FString& sLB = key->Value["LB"].get<FString>();
	const FString& sRB = key->Value["RB"].get<FString>();
	
	StringToPoint2( sLT , v.Offsets[0] );
	StringToPoint2( sRT , v.Offsets[1] );
	StringToPoint2( sLB , v.Offsets[2] );
	StringToPoint2( sRB , v.Offsets[3] );

}


void GetFSsRefCell( const FSsKeyframe* key , FSsRefCell& v )
{
	int id = key->Value["mapId"].get<int>();
	FName name = FName( *(key->Value["name"].get<FString>()) );

	v.Mapid = id;
	v.Name = name;
}


void	GetSsUserDataAnime( const FSsKeyframe* key , FSsUserDataAnime& v )
{
	v.Integer = 0;
	v.Point.X = v.Point.Y = 0;
	v.Rect.X = v.Rect.Y = v.Rect.W = v.Rect.H = 0; 
	v.String = FString(TEXT(""));
	v.UseInteger = key->Value.IsExistHashkey("integer");
	v.UsePoint = key->Value.IsExistHashkey("point");
	v.UseRect = key->Value.IsExistHashkey("rect");
	v.UseString = key->Value.IsExistHashkey("string");

	if ( v.UseInteger )
	{
		v.Integer = key->Value["integer"].get<int>();
	}

	if ( v.UsePoint )
	{
		const FString& str = key->Value["point"].get<FString>();
		StringToPoint2( str , v.Point );
	}
	
	if ( v.UseRect )
	{
		const FString& str = key->Value["rect"].get<FString>();
		StringToIRect( str , v.Rect );
	}

	if ( v.UseString )
	{
		const FString& str = key->Value["string"].get<FString>();
		v.String = str;
	}

}

void	GetSsInstparamAnime( const FSsKeyframe* key , FSsInstanceAttr& v )
{
	const FString& sstartLabel = key->Value["startLabel"].get<FString>();
	const int& sstartOffset = key->Value["startOffset"].get<int>();
	const FString& sendLabel = key->Value["endLabel"].get<FString>();
	const int& sendOffset = key->Value["endOffset"].get<int>();

	const float& sspeed = key->Value["speed"].get<float>();

	const int& sloopNum = key->Value["loopNum"].get<int>();
	const bool& sinfinity = key->Value["infinity"].get<bool>();
	const bool& sreverse = key->Value["reverse"].get<bool>();
	const bool& spingpong = key->Value["pingpong"].get<bool>();
	const bool& sindependent = key->Value["independent"].get<bool>();


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
