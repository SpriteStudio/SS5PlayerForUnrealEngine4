#pragma once

#include "SsAttribute.h"

#include "SsAnimePack.generated.h"


/// �A�j���[�V�����Đ��ݒ���ł��B
USTRUCT()
struct SPRITESTUDIO5_API FSsAnimationSettings
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	int32	Fps;			//!< �Đ�FPS

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	int32	FrameCount;		//!< �t���[����

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	TEnumAsByte<SsPartsSortMode::Type>	SortMode;		//!< �p�[�c�̃\�[�g���[�h

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	FVector2D	CanvasSize;				//!< �L�����o�X�T�C�Y(����g)�B�r���[�|�[�g�̃T�C�Y�ƃC�R�[���ł͂Ȃ��B

	UPROPERTY(VisibleAnywhere, Category=SsAnimationSettings, BlueprintReadOnly)
	FVector2D	Pivot;					//!< �L�����o�X�̌��_�B0,0 �������B-0.5, +0.5 ������
};



//�p�[�c�������̏���ێ�����f�[�^�ł��B
USTRUCT()
struct SPRITESTUDIO5_API FSsPart
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	FName	PartName;

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	ArrayIndex;		//!< �c���[��z��ɓW�J�������̃C���f�b�N�X

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	ParentIndex;	//!< �e�p�[�c�̃C���f�b�N�X

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsPartType::Type>		Type;			//!< ���

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsBoundsType::Type>		BoundsType;		//!< �����蔻��Ƃ��Ďg�����H�g���ꍇ�͂��̌`��B

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsInheritType::Type>	InheritType;	//!< �A�g���r���[�g�l�̌p�����@

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	TEnumAsByte<SsBlendType::Type>		AlphaBlendType;	//!< ���u�����h�̉��Z��

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	Show;			//!< [�ҏW�p�f�[�^] �p�[�c�̕\���E��펞

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	int32	Locked;			//!< [�ҏW�p�f�[�^] �p�[�c�̃��b�N���

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	float InheritRates[(int)SsAttributeKind::Num];	///< �e�̒l�̌p�����BSS4�Ƃ̌݊����̂��ߎc����Ă��邪0 or 1


	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	FName	RefAnimePack;   ///< �Q�ƃA�j����

	UPROPERTY(VisibleAnywhere, Category=SsPart, BlueprintReadOnly)
	FName	RefAnime;       ///< �Q�ƃA�j����

public:
	FSsPart()
		: PartName(TEXT("")), ArrayIndex(0), ParentIndex(0), Show(0), Locked(0)
	{
		for (int i = 0; i < (int)SsAttributeKind::Num; ++i)
		{
			InheritRates[i] = 1.f;
		}
		// �C���[�W���]�͌p�����Ȃ�
		InheritRates[(int)SsAttributeKind::Imgfliph] = 0.f;
		InheritRates[(int)SsAttributeKind::Imgflipv] = 0.f;
	}
};


//�A�j���[�V�������\������p�[�c�����X�g���������̂ł��B
USTRUCT()
struct SPRITESTUDIO5_API FSsModel
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsModel, BlueprintReadOnly)
	TArray<FSsPart>	PartList;	//!<�i�[����Ă���p�[�c�̃��X�g
};


//typedef std::vector<SsAttribute*>	SsAttributeList;

USTRUCT()
struct SPRITESTUDIO5_API FSsPartAnime
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsPartAnime, BlueprintReadOnly)
	FName PartName;

	UPROPERTY(VisibleAnywhere, Category=SsPartAnime, BlueprintReadOnly)
	TArray<FSsAttribute> Attributes;
};


/// ���x���B���[�v��ȂǂɎw�肷��
USTRUCT()
struct SPRITESTUDIO5_API FSsLabel
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsLabel, BlueprintReadOnly)
	FName		LabelName;	///< ���O [�ϐ����ύX�֎~]

	UPROPERTY(VisibleAnywhere, Category=SsLabel, BlueprintReadOnly)
	int32		Time;		///< �ݒu���ꂽ����(�t���[��) [�ϐ����ύX�֎~]
};


USTRUCT()
struct SPRITESTUDIO5_API FSsAnimation
{
	GENERATED_USTRUCT_BODY()
	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	FName	AnimationName;					/// �A�j���[�V�����̖���

	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	bool	OverrideSettings;				/// ���̃C���X�^���X�����ݒ���g��animePack �̐ݒ���Q�Ƃ��Ȃ��BFPS, frameCount �͏�Ɏ��g�̐ݒ���g���B

	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	FSsAnimationSettings	Settings;		/// �ݒ���

	UPROPERTY(Category=SsAnimation, BlueprintReadOnly)	// �v�f��������Details�E�B���h�E���ɒ[�ɏd���Ȃ��Ă��܂����߁AVisibleAnywhere��t���Ȃ�
	TArray<FSsPartAnime>	PartAnimes;		///	�p�[�c���̃A�j���[�V�����L�[�t���[�����i�[����郊�X�g

	UPROPERTY(VisibleAnywhere, Category=SsAnimation, BlueprintReadOnly)
	TArray<FSsLabel>		Labels;
};

/**
*@class SsAnimePack
*@brief �p�[�c��g�ݍ��킹���\���Ƃ��̍\�����g�p����A�j���[�V�������i�[����f�[�^�ł��B
�p�[�c�̑g�ݍ��킹�\����SsModel�AModel���g�p����A�j���f�[�^��SsAnimation�Œ�`���Ă��܂��B
*/
USTRUCT()
struct SPRITESTUDIO5_API FSsAnimePack
{
	GENERATED_USTRUCT_BODY()

	void Serialize(FArchive& Ar);

public:
	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FString					Version;

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FSsAnimationSettings	Settings;		//!< �ݒ��� 

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FName					AnimePackName;	//!< �A�j���[�V�����p�b�N����

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	FSsModel				Model;			//!< �p�[�c���̊i�[��

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	TArray<FName>			CellmapNames;	//!< �g�p����Ă���Z���}�b�v�̖���		

	UPROPERTY(VisibleAnywhere, Category=SsAnimePack, BlueprintReadOnly)
	TArray<FSsAnimation>	AnimeList;		//!< �i�[����Ă���q�A�j���[�V�����̃��X�g

	// �A�j���[�V����������C���f�b�N�X���擾����
	int32 FindAnimationIndex(const FName& Name);
};
