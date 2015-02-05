#pragma once

#include "SsPlayerTickResult.h"

class USsProject;
class FSsAnimeDecoder;
class FSsCellMapList;


class SPRITESTUDIO5_API FSsPlayer
{
public:
	FSsPlayer();

	void SetSsProject(TWeakObjectPtr<USsProject> InSsProject);
	TWeakObjectPtr<USsProject> GetSsProject() { return SsProject; }

	FSsAnimeDecoder* GetDecoder() { return &*Decoder; }

	FSsPlayerTickResult Tick(float DeltaSeconds);
	void Draw(FCanvas* Canvas, FVector2D CenterLocation, float Rotation=0.f, FVector2D Scale=FVector2D(1.f,1.f), bool bMask=false);


	// �Đ�
	bool Play(int32 InAnimPackIndex, int32 InAnimationIndex, int32 StartFrame=0, float PlayRate=1.f, int32 LoopCount=0, bool bRoundTrip=false);
	// �ꎞ��~
	void Pause(){ bPlaying = false; }
	// �ĊJ
	bool Resume();
	// �Đ������擾
	bool IsPlaying() const { return bPlaying; }
	// �A�j���[�V����������C���f�b�N�X���擾
	bool GetAnimationIndex(const FName& InAnimPackName, const FName& InAnimationName, int32& OutAnimPackIndex, int32& OutAnimationIndex);

	// �w��t���[������
	void SetPlayFrame(float Frame);
	// ���݃t���[���擾
	float GetPlayFrame() const;
	// �ŏI�t���[���擾
	float GetAnimeEndFrame() const;

	// �R�}����i�O�t���[���ցj
	void ToPrevFrame(bool bLoop=true);
	// �R�}����i���t���[���ցj
	void ToNextFrame(bool bLoop=true);


	// �p�[�c������C���f�b�N�X���擾
	int32 GetPartIndexFromName(FName PartName) const;

	// �p�[�c��Transform���擾(Canvas��2D���W�n)
	FTransform GetPartTransformInCanvas(int32 PartIndex, int32 CanvasWidth, int32 CanvasHeight, FVector2D CenterLocation, float Rotation, FVector2D Scale) const;

private:
	void TickAnimation(float DeltaSeconds, FSsPlayerTickResult& Result);
	void FindUserDataInInterval(FSsPlayerTickResult& Result, float Start, float End);

public:
	float PlayRate;		// �Đ����x
	int32 LoopCount;	// ���[�v��. 0�ȉ��̏ꍇ�͖������[�v. �����Đ��̏ꍇ�͕Г��łP��Ƃ݂Ȃ�. 
	bool bRoundTrip;	// �����Đ�
	bool bFlipH;		// ���E���]
	bool bFlipV;		// �㉺���]
	TMap<int32, TWeakObjectPtr<UTexture>> TextureReplacements;	// �p�[�c���̃e�N�X�`�������ւ�

	const FVector2D& GetAnimPivot() const { return AnimPivot; }	// �A�j���[�V�����ɐݒ肳�ꂽPivot���擾

private:
	TWeakObjectPtr<USsProject> SsProject;
	TSharedPtr<FSsAnimeDecoder> Decoder;
	TSharedPtr<FSsCellMapList> CellMapList;
	bool bPlaying;
	bool bFirstTick;
	FVector2D AnimPivot;
};
