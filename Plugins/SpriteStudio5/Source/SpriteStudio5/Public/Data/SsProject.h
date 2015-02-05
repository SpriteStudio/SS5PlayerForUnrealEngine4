#pragma once

#include "SsTypes.h"
#include "SsAnimePack.h"
#include "SsCellMap.h"

#include "SsProject.generated.h"



/// �v���W�F�N�g�t�@�C���̐ݒ肪�L�ڂ���Ă���f�[�^�ł��B
/// �ȉ��̃^�O�̓G�f�B�^�ҏW�p�̃f�[�^�Ȃ̂œǂݔ�΂��܂��B
//	�ҏW���ݒ�̂��߂�܂Ȃ�
//	<ssaxImport>						//!< .ssax �C���|�[�g�ݒ�
//	<copyWhenImportImageIsOutside>		//!< �v���W�F�N�g�t�H���_�O�ɉ摜�t�@�C��������ꍇ�ɃC���|�[�g��ɃR�s�[����B
//	<exportAnimeFileFormat>				//!< �G�N�X�|�[�g���̃A�j���[�V�����t�@�C���̃t�H�[�}�b�g 
//	<exportCellMapFileFormat>			//!< �G�N�X�|�[�g���̃Z���}�b�v�t�@�C���̃t�H�[�}�b�g
//	<exportCellMap>						//!< �Z���}�b�v���G�N�X�|�[�g����
//	<copyImageWhenExportCellmap>		//!< �Z���}�b�v�̃G�N�X�|�[�g���ɉ摜�t�@�C�����G�N�X�|�[�g��ɃR�s�[����
//	<ssaxExport>						//!< .ssax �G�N�X�|�[�g�ݒ�
//	<player>							//!< �Đ��Ώۂ̃v���C���[�B����ɂ��g����@�\�ɐ�����������B
//	<strictVer4>						//!< Ver4�݊�
//	<availableAttributes>				//!< �g�p����A�g���r���[�g
//	<defaultSetAttributes>				//!< �V�K�L�[�쐬�Ńf�t�H���g�ŃL�[���ł����A�g���r���[�g
USTRUCT()
struct SPRITESTUDIO5_API FSsProjectSetting
{
	GENERATED_USTRUCT_BODY()

public:
	//���̃����o�[�����邪�A�K�v�Œ���̂��̂�ǂݍ���

	//�A�j���[�V�����t�@�C���̑��Ύw�蓙
	UPROPERTY(VisibleAnywhere, Category=SsProjectSetting, BlueprintReadOnly)
	FString		AnimeBaseDirectory;			//!< �A�j���[�V�����f�[�^�̓ǂݏ�����f�B���N�g���B

	UPROPERTY(VisibleAnywhere, Category=SsProjectSetting, BlueprintReadOnly)
	FString		CellMapBaseDirectory;		//!< �Z���}�b�v�f�[�^�̓ǂݏ�����f�B���N�g���B

	UPROPERTY(VisibleAnywhere, Category=SsProjectSetting, BlueprintReadOnly)
	FString		ImageBaseDirectory;			//!< �摜�f�[�^�̓ǂݍ��݌����f�B���N�g���B

	UPROPERTY(VisibleAnywhere, Category=SsProjectSetting, BlueprintReadOnly)
	FString		ExportBaseDirectory;		//!< �G�N�X�|�[�g��̊�f�B���N�g���B

	UPROPERTY(VisibleAnywhere, Category=SsProjectSetting, BlueprintReadOnly)
	bool		QueryExportBaseDirectory;	//!< �G�N�X�|�[�g��̊�f�B���N�g���w��������邩�H 

	UPROPERTY(VisibleAnywhere, Category=SsProjectSetting, BlueprintReadOnly)
	TEnumAsByte<SsTexWrapMode::Type>	WrapMode;			//!< �e�N�X�`���̃��b�v���[�h

	UPROPERTY(VisibleAnywhere, Category=SsProjectSetting, BlueprintReadOnly)
	TEnumAsByte<SsTexFilterMode::Type>	FilterMode;			//!< �e�N�X�`���̃t�B���^���[�h
};


/// XML�h�L�������g�ƂȂ��Ă���sspj�t�@�C���̃f�[�^�ێ���񋟂���N���X�ł��B
///�ȉ��̓G�f�B�^���̂��ߓǂݔ�΂��܂��B
/// animeSettings   �f�t�H���g�̃A�j���[�V�����ݒ� 
/// texPackSettings �f�t�H���g�̃e�N�X�`���p�b�L���O�ݒ�
UCLASS()
class SPRITESTUDIO5_API USsProject : public UObject
{
	GENERATED_UCLASS_BODY()

	virtual void Serialize(FArchive& Ar) override;

public:
	UPROPERTY(VisibleAnywhere, Category=SsProject, BlueprintReadOnly)
	FString				Version;

	UPROPERTY(VisibleAnywhere, Category=SsProject, BlueprintReadOnly)
	FSsProjectSetting	Settings;			//!< �v���W�F�N�g�ݒ�

	UPROPERTY(VisibleAnywhere, Category=SsProject, BlueprintReadOnly)
	TArray<FName>		CellmapNames;		//!< �Z���}�b�v�t�@�C���̃��X�g

	UPROPERTY(VisibleAnywhere, Category=SsProject, BlueprintReadOnly)
	TArray<FName>		AnimepackNames;		//!< �A�j���t�@�C���̃��X�g

	UPROPERTY(VisibleAnywhere, Category=SsProject, BlueprintReadOnly)
	TArray<FSsCellMap>		CellmapList;	//!< �Z���}�b�v���X�g

	UPROPERTY(VisibleAnywhere, Category=SsProject, BlueprintReadOnly)
	TArray<FSsAnimePack>	AnimeList;		//!< �A�j���[�V�����̃��X�g	

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Instanced, Category = Reimport)
	class UAssetImportData* AssetImportData;
#endif

	//���[�h���ɍ쐬����郏�[�N
	FString		ProjFilepath;	///�v���W�F�N�g�t�@�C���̃p�X


public:	
	///�v���W�F�N�g�̐ݒ���̎擾
	const FSsProjectSetting& GetProjectSetting(){ return Settings; }

	//�A�j���p�b�N������A�j���[�V�����C���f�b�N�X���擾����
	int32 FindAnimePackIndex(const FName& AnimePackName);

	//�Z���}�b�v������Z���}�b�v�C���f�b�N�X���擾����
	int32 FindCellMapIndex(const FName& CellMapName);

	// �A�j���[�V����������C���f�b�N�X���擾
	bool FindAnimationIndex(const FName& InAnimPackName, const FName& InAnimationName, int32& OutAnimPackIndex, int32& OutAnimationIndex);

	///�C���f�b�N�X����A�j���[�V�������擾����
	FSsAnimation* FindAnimation(int32 AnimPackIndex, int32 AnimationIndex);

	
	///���g�̃t�@�C���p�X��ݒ肷��
	void SetFilepath(const FString& path){ ProjFilepath = path; }

	///���g�̓ǂݍ��݌��̃t�@�C���p�X���擾����
	const FString& GetFilepath() const { return ProjFilepath; }

	///ssce�f�[�^�̓ǂݍ��݌��̊�p�X���擾����B 
	FString GetSsceBasepath() const;

	///ssae�f�[�^�̓ǂݍ��݌��̊�p�X���擾����B 
	FString GetSsaeBasepath() const;

	///�e�N�X�`���f�[�^�̓ǂݍ��݌��̊�p�X���擾����B 
	FString GetImageBasepath() const;


	///AnimePack(ssae)�̃t�@�C�������p�X�t���Ŏ擾����
	FString GetAnimePackFilePath(int32 index)
	{
		if (AnimepackNames.Num() <= index) return TEXT("");
		return GetSsaeBasepath() + AnimepackNames[index].ToString();
	}

	///CellMap(ssce)�̃t�@�C�������p�X�t���Ŏ擾����
	FString GetCellMapFilePath(int32 index)
	{
		if (CellmapNames.Num() <= index) return TEXT("");
		FString str = GetSsceBasepath();
		str = str + CellmapNames[index].ToString();
		return str;
	}
};
