#include "SpriteStudio5EdPrivatePCH.h"

#include "SsArchiver.h"
#include "SsString_uty.h"
#include "SsTypes.h"
#include "SsAttribute.h"
#include "SsValue.h"
#include "SsAnimePack.h"
#include "SsCellMap.h"
#include "SsProject.h"


namespace
{
	// 日本語文字列が含まれるかチェックし、含まれた場合はユニークな文字列に置き換える 
	//    FNameに変換した際に、'?'とかに置き換えられて、別の文字列でも同じとみなされてしまい、参照が切れるのを防ぐため 
	void CheckReplaceJapaneseString(const char* Text, FString& String)
	{
		bool bNG_Char = false;

		for(int32 i = 0; 0 != Text[i]; ++i)
		{
			if((int32)Text[i] != (int32)(*String)[i])
			{
				bNG_Char = true;
				break;
			}
		}

		// 日本語文字列が含まれてしまう場合は、 
		if(bNG_Char)
		{
			String = FString("Replaced");
			for(int32 i = 0; 0 != Text[i]; ++i)
			{
				String = FString::Printf(TEXT("%s/%d"), *String, (int32)Text[i]);
			}
			UE_LOG(LogSpriteStudioEd, Warning, TEXT("Replace Japanese String to \"%s\""), *String);
		}
	}
}

bool SsXmlIArchiver::dc_attr(const char* name, FString& member)
{
	AR_SELF_CHECK();
	const char* v = 0;

	v = getxml()->Attribute(name);
	if(v != 0){ member= v; }

	return true;
}
bool SsXmlIArchiver::dc_attr(const char* name, int& member)
{
	AR_SELF_CHECK();

	const char* v = getxml()->Attribute(name);

	member = atoi(v);

	return true;
}

bool SsXmlIArchiver::dc(const char* name, int& member)
{
	AR_SELF_CHECK();
	FString str;
	dc(name , str);
	member = FCString::Atoi(*str);

	return true;
}

bool SsXmlIArchiver::dc(const char* name, float& member)
{
	AR_SELF_CHECK();
	FString str;
	dc(name , str);
	member = FCString::Atof(*str);

	return true;
}

bool SsXmlIArchiver::dc(const char* name, FString& member)
{
	AR_SELF_CHECK();
	XMLElement* e = getxml()->FirstChildElement(name);
	if(e)
	{
		if(e->GetText())
		{
			member = FString(babel::utf8_to_sjis( e->GetText() ).c_str());
			CheckReplaceJapaneseString(e->GetText(), member);
		}
		else
		{
			member ="";
		}
		return true;
	}
	return false;
}

bool SsXmlIArchiver::dc(const char* name, FName& member)
{
	AR_SELF_CHECK();

	FString dummy;
	if(dc(name, dummy))
	{
		member = FName(*dummy);
		return true;
	}
	return false;
}

bool SsXmlIArchiver::dc(const char* name, bool& member)
{
	AR_SELF_CHECK();
	member = false;
	XMLElement* e = getxml()->FirstChildElement(name);
	if(e)
	{
		int ret = GetTextToInt(e , 0);
		if(ret == 1)member = true;
		return true;
	}

	return false;
}

bool SsXmlIArchiver::dc(const char* name, TArray<FString>& list)
{
	AR_SELF_CHECK();
	list.Empty();
	XMLElement* e = getxml()->FirstChildElement(name);
	e = e->FirstChildElement("value");
	while(e)
	{
		const char* txt = e->GetText();
		FString sjis_str( babel::utf8_to_sjis( txt ).c_str() );
		CheckReplaceJapaneseString(txt, sjis_str);

		list.Add( sjis_str );
		e = e->NextSiblingElement();
	}

	return true;
}

bool SsXmlIArchiver::dc(const char* name, TArray<FName>& list)
{
	AR_SELF_CHECK();
	list.Empty();
	XMLElement* e = getxml()->FirstChildElement(name);
	e = e->FirstChildElement("value");
	while(e)
	{
		const char* txt = e->GetText();
		FString sjis_str( babel::utf8_to_sjis( txt ).c_str() );
		CheckReplaceJapaneseString(txt, sjis_str);

		list.Add( FName(*sjis_str) );
		e = e->NextSiblingElement();
	}

	return true;
}

bool SsXmlIArchiver::dc(const char* name, FVector2D& member)
{
	AR_SELF_CHECK();

	XMLElement* e = getxml()->FirstChildElement(name);

	if(e)
	{
		FString str( e->GetText() );
		return StringToPoint2( str , member );
	}
	return false;
}

bool SsXmlIArchiver::dc(const char* name, FSsCurve& member)
{
	AR_SELF_CHECK();

	XMLElement* e = getxml()->FirstChildElement( name );
	if(e)
	{
		TArray<FString>	str_list;
		split_string( e->GetText() , ' ' , str_list );
		if(str_list.Num() < 4)
		{
			return false;
		}else{
			member.StartTime = FCString::Atof(*(str_list[0]));
			member.StartValue = FCString::Atof(*(str_list[1]));
			member.EndTime = FCString::Atof(*(str_list[2]));
			member.EndValue = FCString::Atof(*(str_list[3]));
			return true;
		}
	}
	return false;
}

void SsArchiverInit()
{
	babel::init_babel();
}

///補間でカーブパラメータが必要なタイプか判定する
bool SsNeedsCurveParams(SsInterpolationType::Type type)
{
	switch (type)
	{
	case SsInterpolationType::Bezier:
	case SsInterpolationType::Hermite:
		return true;
	}
	return false;
}


FSsValue SsValueSeriarizer__MakeValue(const char* v)
{
	FString temp(v);
	if(temp.IsNumeric())
	{
		return FSsValue((float)atof(v));
	}
	else
	{
		FString ValueStr(babel::utf8_to_sjis(v).c_str());
		CheckReplaceJapaneseString(v, ValueStr);
		return FSsValue(ValueStr);
	}
}
void SsValueSeriarizer(ISsXmlArchiver* ar , FSsValue& v , const FString key = "value")
{
	char tmp[32];

	//インプット
	XMLElement* e = ar->getxml();
	if ( key != "" )
	{
#if PLATFORM_WINDOWS
		sprintf_s(tmp, 32, "%S", *key);
#else
		sprintf(tmp, "%S", *key);
#endif
		e = e->FirstChildElement( tmp );
	}
	if ( e )
	{
		XMLElement* child = e->FirstChildElement();

		if ( child == 0 )
		{
			const char* str = e->GetText();

			if (str==0)
			{
				const char* err_log1 = e->Parent()->Value();
				const char* err_log2 = e->Parent()->ToElement()->GetText();
				const char* err_log3 = e->Parent()->Parent()->ToElement()->FirstAttribute()->Value();
				return ;
			}
			
			v = SsValueSeriarizer__MakeValue( str );
		}else{
			XMLElement* ce = child;

			SsHash hash;
			while(ce)
			{
				TCHAR ceName[32];
#if PLATFORM_WINDOWS
				FUTF8ToTCHAR_Convert::Convert(ceName, 32, ce->Name(), strnlen_s(ce->Name(), 32) + 1);
#else
				FUTF8ToTCHAR_Convert::Convert(ceName, 32, ce->Name(), strnlen(ce->Name(), 32) + 1);
#endif
				FName fceName(ceName);
				if(!hash.Contains(fceName))
				{
					hash.Add(fceName);
				}

				const char* str = ce->GetText();
				if ( str != 0 )
				{
					hash[fceName] = SsValueSeriarizer__MakeValue( str );
					ce = (XMLElement*)ce->NextSibling();
				}else{
					//さらに子構造があるようだ
					//さらに子構造があるようだ
					//ce = 0 ;
					SsXmlIArchiver ar2(ce);

					FSsValue tempv;
					SsValueSeriarizer( &ar2 , tempv , "");
					hash[fceName] = tempv;					
					ce = (XMLElement*)ce->NextSibling();
				}
			}

			v = FSsValue(hash);

		}

		return ;
	}
}
void SerializeStruct(FSsKeyframe& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE_ATTRIBUTE("time", Value.Time);
	SSAR_DECLARE_ATTRIBUTE_ENUM("ipType", Value.IpType);

	if(SsNeedsCurveParams(Value.IpType))
	{
		SSAR_DECLARE("curve", Value.Curve);
	}
	SsValueSeriarizer(ar , Value.Value);
}
void SerializeStruct(FSsAttribute& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE_ATTRIBUTE_ENUM("tag", Value.Tag);
	SSAR_DECLARE_LISTEX("key", Value.Key,  "");
}
void SerializeStruct(FSsCell& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE("name", Value.CellName);
	SSAR_DECLARE("pos", Value.Pos);
	SSAR_DECLARE("size", Value.Size);
	SSAR_DECLARE("pivot", Value.Pivot);
	SSAR_DECLARE("rotated", Value.Rotated);
}
void SerializeStruct(FSsAnimationSettings& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE("fps", Value.Fps);
	SSAR_DECLARE("frameCount", Value.FrameCount);
	SSAR_DECLARE("canvasSize", Value.CanvasSize);
	SSAR_DECLARE("pivot", Value.Pivot);
}
void SerializeStruct(FSsPart& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE("name", Value.PartName);
	SSAR_DECLARE("arrayIndex", Value.ArrayIndex);
	SSAR_DECLARE("parentIndex", Value.ParentIndex);

	SSAR_DECLARE_ENUM("type", Value.Type);
	SSAR_DECLARE_ENUM("boundsType", Value.BoundsType);
	SSAR_DECLARE_ENUM("inheritType", Value.InheritType);
	SSAR_DECLARE_ENUM("alphaBlendType", Value.AlphaBlendType);
	SSAR_DECLARE("show", Value.Show);
	SSAR_DECLARE("locked", Value.Locked);
	SSAR_DECLARE("refAnimePack", Value.RefAnimePack);
	SSAR_DECLARE("refAnime", Value.RefAnime);

	//継承率後に改良を実施
	if (ar->getType() == EnumSsArchiver::in)
	{
		XMLElement* e = ar->getxml()->FirstChildElement("ineheritRates");
		if (e)
		{
			XMLElement* ec = e->FirstChildElement();
			while(ec)
			{
				const char* tag = ec->Value();
				TEnumAsByte<SsAttributeKind::Type> enumattr;

				__StringToEnum_( tag , enumattr );
				Value.InheritRates[(int)enumattr] = (float)atof(ec->GetText());
				ec = ec->NextSiblingElement();
			}
		}
	}
}
void SerializeStruct(FSsModel& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE_LIST("partList", Value.PartList);
}
void SerializeStruct(FSsPartAnime& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE("partName", Value.PartName);
	SSAR_DECLARE_LISTEX("attributes", Value.Attributes, "attribute");
}
void SerializeStruct(FSsLabel& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE("name", Value.LabelName);
	SSAR_DECLARE("time", Value.Time);
}
void SerializeStruct(FSsAnimation& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE("name", Value.AnimationName);
	SSAR_STRUCT_DECLARE("settings", Value.Settings);
	SSAR_DECLARE_LISTEX("labels", Value.Labels, "value");
	SSAR_DECLARE_LISTEX("partAnimes", Value.PartAnimes, "partAnime");
}
void SerializeStruct(FSsProjectSetting& Value, SsXmlIArchiver* ar)
{
	SSAR_DECLARE("animeBaseDirectory", Value.AnimeBaseDirectory);
	SSAR_DECLARE("cellMapBaseDirectory", Value.CellMapBaseDirectory);
	SSAR_DECLARE("imageBaseDirectory", Value.ImageBaseDirectory);
	SSAR_DECLARE("exportBaseDirectory", Value.ExportBaseDirectory);
	SSAR_DECLARE("queryExportBaseDirectory", Value.QueryExportBaseDirectory);
	SSAR_DECLARE_ENUM("wrapMode", Value.WrapMode);
	SSAR_DECLARE_ENUM("filterMode", Value.FilterMode);
}


void SerializeSsCellMap(FSsCellMap& CellMap, SsXmlIArchiver* ar)
{
	SSAR_DECLARE_ATTRIBUTE("version", CellMap.Version);
	SSAR_DECLARE("name", CellMap.CellMapName);
	SSAR_DECLARE("imagePath", CellMap.ImagePath);
	SSAR_DECLARE("pixelSize", CellMap.PixelSize);
	SSAR_DECLARE("overrideTexSettings", CellMap.OverrideTexSettings);
	SSAR_DECLARE_ENUM("wrapMode", CellMap.WrapMode);
	SSAR_DECLARE_ENUM("filterMode", CellMap.FilterMode);

	SSAR_DECLARE_LISTEX("cells", CellMap.Cells, "cell");
}
void SerializeSsAnimePack(FSsAnimePack& AnimePack, SsXmlIArchiver* ar)
{
	SSAR_DECLARE_ATTRIBUTE("version", AnimePack.Version);
	SSAR_STRUCT_DECLARE("settings", AnimePack.Settings);
	SSAR_DECLARE("name", AnimePack.AnimePackName);
	SSAR_STRUCT_DECLARE("Model", AnimePack.Model);
	SSAR_DECLARE("cellmapNames", AnimePack.CellmapNames);
	SSAR_DECLARE_LISTEX("animeList", AnimePack.AnimeList, "anime");
}
void SerializeSsProject(USsProject& Proj, SsXmlIArchiver* ar)
{
	SSAR_DECLARE_ATTRIBUTE("version", Proj.Version);
	SSAR_STRUCT_DECLARE("settings", Proj.Settings);
	SSAR_DECLARE("cellmapNames", Proj.CellmapNames);
	SSAR_DECLARE("animepackNames", Proj.AnimepackNames);
}

