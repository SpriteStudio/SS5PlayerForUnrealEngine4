#pragma once

#include "SsTypes.h"

#include "SsCellMap.generated.h"


///�p�[�c�Ɏg�p������f�̋�`�͈͂��������\���ł��B
USTRUCT()
struct SPRITESTUDIO5_API FSsCell 
{
	GENERATED_USTRUCT_BODY()

public:
	//--------- �����^�C���p�f�[�^�Ƃ��ĕۑ����ׂ�����
	UPROPERTY(VisibleAnywhere, Category=FSsCell, BlueprintReadOnly)
	FName		CellName;		///< �Z������

	UPROPERTY(VisibleAnywhere, Category=FSsCell, BlueprintReadOnly)
	FVector2D	Pos;			///< ����̍��W

	UPROPERTY(VisibleAnywhere, Category=FSsCell, BlueprintReadOnly)
	FVector2D	Size;			///< WH�s�N�Z���T�C�Y

	UPROPERTY(VisibleAnywhere, Category=FSsCell, BlueprintReadOnly)
	FVector2D	Pivot;			///< ���_�Bsize /2 ������=0,0�ɂȂ�B

	UPROPERTY(VisibleAnywhere, Category=FSsCell, BlueprintReadOnly)
	bool		Rotated;		///< �������ɂX�O�x��]����Ă���Buvs �̊��蓖�Ă��ς��B
};



//!�Z���}�b�v�f�[�^��\�����邽�߂̃N���X�ł��B
/*!
@class SsCellMap
@breif �Z���}�b�v�͂P�̃e�N�X�`���t�@�C���Ƃ��̃e�N�X�`�����Ńp�[�c�Ƃ��Ďg�p�����`�͈͂��������Z�����R���e�i�Ƃ��ĕێ����邽�߂̍\���ł��B<BR>
<BR>
���̃f�[�^�R���e�i�̓G�f�B�b�g�p�Ƃ��ĉ��L��ǂݔ�΂��܂��B<BR>
imagePathAtImport;///< �C���|�[�g���̎Q�ƌ��摜�̃t���p�X<BR>
packInfoFilePath;	///< �p�b�N���t�@�C���BTexturePacker ���̃f�[�^���C���|�[�g�����ꍇ�̂ݗL��<BR>
texPackSettings;	///< �p�b�N���̎Q�Ə��<BR>
*/
USTRUCT()
struct SPRITESTUDIO5_API FSsCellMap
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	FString		Version;

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	FName		FileName;			///< �Z���}�b�v�̃t�@�C���l�[��

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	FName		CellMapName;		///< ���̃Z���}�b�v�̖��̂ł��B

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	FString		ImagePath;			///< �Q�Ɖ摜�t�@�C���p�X�B�v���W�F�N�g�̉摜�����

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	UTexture*	Texture;

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	FVector2D	PixelSize;			///< �摜�̃s�N�Z��WH�T�C�Y

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	bool		OverrideTexSettings;///< �e�N�X�`���ݒ���v���W�F�N�g�̐ݒ�ł͂Ȃ����L�ݒ���g��

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	TEnumAsByte<SsTexWrapMode::Type>	WrapMode;			///< �e�N�X�`���̃��b�v���[�h

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	TEnumAsByte<SsTexFilterMode::Type>	FilterMode;			///< �e�N�X�`���̃t�B���^���[�h

	UPROPERTY(VisibleAnywhere, Category=SsCellMap, BlueprintReadOnly)
	TArray<FSsCell>		Cells;
};


