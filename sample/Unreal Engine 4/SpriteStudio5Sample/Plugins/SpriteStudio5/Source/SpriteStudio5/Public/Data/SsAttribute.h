#pragma once

#include "SsTypes.h"
#include "SsValue.h"

#include "SsAttribute.generated.h"


//�A�j���[�V�������̃L�[�t���[���̓��e��\������N���X
USTRUCT()
struct SPRITESTUDIO5_API FSsKeyframe
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	int32	Time;	///< ����

	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	TEnumAsByte<SsInterpolationType::Type>	IpType;	///< ��ԃ^�C�v

	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	FSsCurve	Curve;	///< �Ȑ���Ԍv�Z�p�p�����[�^

	UPROPERTY(VisibleAnywhere, Category=SsKeyframe, BlueprintReadOnly)
	FSsValue	Value;	///< �l
public:
	FSsKeyframe()
		: IpType(SsInterpolationType::Invalid)
		, Time(0)
	{}
};


//�^�O���ɑ��݂���L�[�t���[�����i�[����N���X
USTRUCT()
struct SPRITESTUDIO5_API FSsAttribute	//Tag���ɑ��݂���
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	//�L�[�t���[���f�[�^ : Value�i�^�O�ɂ���ĈقȂ�̑g��)
	UPROPERTY(VisibleAnywhere, Category=SsAttribute, BlueprintReadOnly)
	TEnumAsByte<SsAttributeKind::Type> Tag;

	UPROPERTY(VisibleAnywhere, Category=SsAttribute, BlueprintReadOnly)
	TArray<FSsKeyframe> Key;

public:
	bool isEmpty()
	{
		return 0 == Key.Num();
	}

	const FSsKeyframe* FirstKey();

	///���Ԃ��獶���̃L�[���擾
	const FSsKeyframe* FindLeftKey(int time);

	//���Ԃ���E���̃L�[���擾����
	const FSsKeyframe* FindRightKey(int time);

private:
	int32 GetLowerBoundKeyIndex(int32 Time);	// std::map::lower_bound()���
	int32 GetUpperBoundKeyIndex(int32 Time);	// std::map::upper_bound()���

};

void GetSsColorValue(const FSsKeyframe* key , FSsColorAnime& v);
void GetFSsVertexAnime(const FSsKeyframe* key , FSsVertexAnime& v);
void GetFSsRefCell(const FSsKeyframe* key , FSsRefCell& v);
void GetSsUserDataAnime(const FSsKeyframe* key , FSsUserDataAnime& v);
void GetSsInstparamAnime(const FSsKeyframe* key , FSsInstanceAttr& v);

bool SPRITESTUDIO5_API StringToPoint2(const FString& str , FVector2D& point);
bool SPRITESTUDIO5_API StringToIRect(const FString& str , SsIRect& rect);
