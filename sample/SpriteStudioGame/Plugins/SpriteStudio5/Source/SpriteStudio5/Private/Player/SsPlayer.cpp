#include "SpriteStudio5PrivatePCH.h"
#include "SsPlayer.h"

#include "SsProject.h"
#include "SsAnimePack.h"
#include "SsPlayerAnimedecode.h"
#include "SsPlayerCellmap.h"
#include "SsString_uty.h"

// �R���X�g���N�^
FSsPlayer::FSsPlayer()
	: PlayRate(1.f)
	, LoopCount(-1)
	, bRoundTrip(false)
	, bFlipH(false)
	, bFlipV(false)
	, SsProject(NULL)
	, bPlaying(false)
	, bFirstTick(false)
	, AnimPivot(0.f,0.f)
{
}

// SsProject�A�Z�b�g���Z�b�g
void FSsPlayer::SetSsProject(TWeakObjectPtr<USsProject> InSsProject)
{
	SsProject = InSsProject;
	if(!SsProject.IsValid())
	{
		return;
	}

	Decoder = MakeShareable(new FSsAnimeDecoder());
	CellMapList = MakeShareable(new FSsCellMapList());
}

// �X�V
FSsPlayerTickResult FSsPlayer::Tick(float DeltaSeconds)
{
	FSsPlayerTickResult Result;

	if(bPlaying)
	{
		TickAnimation(DeltaSeconds, Result);
	}

	return Result;
}

// �A�j���[�V�����̍X�V
void FSsPlayer::TickAnimation(float DeltaSeconds, FSsPlayerTickResult& Result)
{
	if(!Decoder.IsValid())
	{
		return;
	}

	// �X�V��̃t���[��
	float BkAnimeFrame = Decoder->GetPlayFrame();
	float AnimeFrame = BkAnimeFrame + (PlayRate * DeltaSeconds * Decoder->GetAnimeFPS());

	// �Đ��J�n�t���[���Ɠ��t���[���ɐݒ肳�ꂽ���[�U�[�f�[�^���E�����߁A����X�V���̂�BkAnimeFrame���떂����
	if(bFirstTick)
	{
		BkAnimeFrame -= (.0f <= PlayRate) ? .1f : -.1f;
	}

	// �ŏI�t���[���ȍ~�ŏ������Đ�
	if((Decoder->GetAnimeEndFrame() <= AnimeFrame) && (0.f < PlayRate))
	{
		if(0 < LoopCount)
		{
			// ���[�v�񐔍X�V
			--LoopCount;

			// ���[�v�񐔂̏I��
			if(0 == LoopCount)
			{
				AnimeFrame = (float)Decoder->GetAnimeEndFrame();
				Pause();
				Result.bEndPlay = true;
			}
		}
		FindUserDataInInterval(Result, BkAnimeFrame, (float)Decoder->GetAnimeEndFrame());

		if(bPlaying)
		{
			float AnimeFrameSurplus = AnimeFrame - Decoder->GetAnimeEndFrame();		// �ɒ[�ɑ傫��DeltaSeconds�͍l�����Ȃ�
			// ����
			if(bRoundTrip)
			{
				AnimeFrame = Decoder->GetAnimeEndFrame() - AnimeFrameSurplus;
				PlayRate *= -1.f;
				FindUserDataInInterval(Result, (float)Decoder->GetAnimeEndFrame(), AnimeFrame);
			}
			// ���[�v
			else
			{
				AnimeFrame = AnimeFrameSurplus;
				FindUserDataInInterval(Result, -.1f, AnimeFrame);
			}
		}
	}
	// 0�t���[���ȑO�ŋt�����Đ�
	else if((AnimeFrame < 0.f) && (PlayRate < 0.f))
	{
		if(0 < LoopCount)
		{
			// ���[�v�񐔍X�V
			--LoopCount;

			// ���[�v�񐔂̏I��
			if(0 == LoopCount)
			{
				AnimeFrame = 0.f;
				Pause();
				Result.bEndPlay = true;
			}
		}
		FindUserDataInInterval(Result, BkAnimeFrame, 0.f);

		if(bPlaying)
		{
			// ����
			if(bRoundTrip)
			{
				AnimeFrame = -AnimeFrame;
				PlayRate *= -1.f;
				FindUserDataInInterval(Result, 0.f, AnimeFrame);
			}
			// ���[�v
			else
			{
				AnimeFrame = Decoder->GetAnimeEndFrame() + AnimeFrame;
				FindUserDataInInterval(Result, (float)Decoder->GetAnimeEndFrame()+.1f, AnimeFrame);
			}
		}
	}
	else
	{
		FindUserDataInInterval(Result, BkAnimeFrame, AnimeFrame);
	}

	Decoder->SetPlayFrame( AnimeFrame );
	Decoder->SetDeltaForIndependentInstance( DeltaSeconds );
	Decoder->Update();

	bFirstTick = false;
}

// �w���ԓ���UserData�L�[��Result�Ɋi�[����
//   Start < Key <= End
void FSsPlayer::FindUserDataInInterval(FSsPlayerTickResult& Result, float Start, float End)
{
	float IntervalMin = FMath::Min(Start, End);
	float IntervalMax = FMath::Max(Start, End);

	const TArray<FSsPartAndAnime>& PartAnime = Decoder->GetPartAnime();
	for(int32 i = 0; i < PartAnime.Num(); ++i)
	{
		FSsPart* Part = PartAnime[i].Key;
		FSsPartAnime* Anime = PartAnime[i].Value;
		if((NULL == Part) || (NULL == Anime))
		{
			continue;
		}

		for(int32 j = 0; j < Anime->Attributes.Num(); ++j)
		{
			FSsAttribute* Attr = &(Anime->Attributes[j]);
			if(Attr->Tag != SsAttributeKind::User)
			{
				continue;
			}

			for(int32 k = 0; k < Attr->Key.Num(); ++k)
			{
				float FloatTime = (float)Attr->Key[k].Time;
				if(	   ((IntervalMin < FloatTime) && (FloatTime < IntervalMax))
					|| (FloatTime == End)
					)
				{
					FSsUserData NewUserData;
					NewUserData.PartName  = Part->PartName;
					NewUserData.PartIndex = i;
					NewUserData.KeyFrame  = Attr->Key[k].Time;

					FSsValue& Value = Attr->Key[k].Value;
					if((Value.Type == SsValueType::HashType) && (Value._Hash))
					{
						for(int32 l = 0; l < Value._Hash->Num(); ++l)
						{
							if(Value._Hash->Contains(TEXT("integer")))
							{
								NewUserData.Value.bUseInteger = true;
								NewUserData.Value.Integer = (*Value._Hash)[TEXT("integer")].get<int>();
							}
							if(Value._Hash->Contains(TEXT("rect")))
							{
								NewUserData.Value.bUseRect = true;
								FString temp = (*Value._Hash)[TEXT("rect")].get<FString>();
								TArray<FString> tempArr;
								tempArr.Empty(4);
								split_string(temp, ' ', tempArr);
								if(4 == tempArr.Num())
								{
									NewUserData.Value.Rect.Left   = (float)FCString::Atof(*tempArr[0]);
									NewUserData.Value.Rect.Top    = (float)FCString::Atof(*tempArr[1]);
									NewUserData.Value.Rect.Right  = (float)FCString::Atof(*tempArr[2]);
									NewUserData.Value.Rect.Bottom = (float)FCString::Atof(*tempArr[3]);
								}
							}
							if(Value._Hash->Contains(TEXT("point")))
							{
								NewUserData.Value.bUsePoint = true;
								FString temp = (*Value._Hash)[TEXT("point")].get<FString>();
								TArray<FString> tempArr;
								tempArr.Empty(2);
								split_string(temp, ' ', tempArr);
								if(2 == tempArr.Num())
								{
									NewUserData.Value.Point.X = (float)FCString::Atof(*tempArr[0]);
									NewUserData.Value.Point.Y = (float)FCString::Atof(*tempArr[1]);
								}
							}
							if(Value._Hash->Contains(TEXT("string")))
							{
								NewUserData.Value.bUseString = true;
								NewUserData.Value.String = (*Value._Hash)[TEXT("string")].get<FString>();
							}
						}
					}

					Result.UserData.Add(NewUserData);
				}
			}
		}
	}
}

// �`��
void FSsPlayer::Draw(FCanvas* Canvas, FVector2D CenterLocation, float Rotation, FVector2D Scale, bool bMask)
{
	if(Decoder.IsValid() && Canvas)
	{
		FSsAnimeDecoder::DrawOption Option;
		Option.CenterLocation = CenterLocation;
		Option.Rotation = Rotation;
		Option.Scale = Scale;
		Option.bFlipH = bFlipH;
		Option.bFlipV = bFlipV;
		Option.TextureReplacements = &TextureReplacements;
		Option.bDrawMask = bMask;

		Decoder->Draw(Canvas, Option);
	}
}

// �Đ�
bool FSsPlayer::Play(int32 InAnimPackIndex, int32 InAnimationIndex, int32 StartFrame, float InPlayRate, int32 InLoopCount, bool bInRoundTrip)
{
	if(NULL == SsProject){ return false; }

	FSsAnimePack* AnimPack = (InAnimPackIndex < SsProject->AnimeList.Num()) ? &(SsProject->AnimeList[InAnimPackIndex]) : NULL;
	if(NULL == AnimPack){ return false; }

	FSsAnimation* Animation = (InAnimationIndex < AnimPack->AnimeList.Num()) ? &(AnimPack->AnimeList[InAnimationIndex]) : NULL;
	if(NULL == Animation){ return false; }

	CellMapList->Set(SsProject.Get(), AnimPack);
	Decoder->SetAnimation(&AnimPack->Model, Animation, CellMapList.Get(), SsProject.Get());
	Decoder->SetPlayFrame( (float)StartFrame );

	bPlaying = true;
	bFirstTick = true;
	PlayRate = InPlayRate;
	LoopCount = InLoopCount;
	bRoundTrip = bInRoundTrip;
	AnimPivot = Animation->Settings.Pivot;

	return true;
}

// �ĊJ
bool FSsPlayer::Resume()
{
	if(Decoder.IsValid() && Decoder->IsAnimationValid())
	{
		bPlaying = true;
		return true;
	}
	return false;
}

// �A�j���[�V����������C���f�b�N�X���擾
bool FSsPlayer::GetAnimationIndex(const FName& InAnimPackName, const FName& InAnimationName, int32& OutAnimPackIndex, int32& OutAnimationIndex)
{
	if(SsProject.IsValid())
	{
		return SsProject->FindAnimationIndex(InAnimPackName, InAnimationName, OutAnimPackIndex, OutAnimationIndex);
	}
	return false;
}

// �w��t���[������
void FSsPlayer::SetPlayFrame(float Frame)
{
	if(Decoder.IsValid())
	{
		Decoder->SetPlayFrame( Frame );
	}
}

// ���݃t���[���擾
float FSsPlayer::GetPlayFrame() const
{
	if(Decoder.IsValid())
	{
		return Decoder->GetPlayFrame();
	}
	return 0.f;
}

// �ŏI�t���[���擾
float FSsPlayer::GetAnimeEndFrame() const
{
	if(Decoder.IsValid())
	{
		return Decoder->GetAnimeEndFrame();
	}
	return 0.f;
}

// �R�}����i�O�t���[���ցj
void FSsPlayer::ToPrevFrame(bool bLoop)
{
	if( Decoder.IsValid() )
	{
		float AnimeFrame = FMath::FloorToFloat(Decoder->GetPlayFrame()) - 1.f;

		if(AnimeFrame < 0.f)
		{
			if(bLoop)
			{
				AnimeFrame += Decoder->GetAnimeEndFrame();
			}
			else
			{
				AnimeFrame = 0.f;
			}
		}

		Decoder->SetPlayFrame( AnimeFrame );
		Decoder->SetDeltaForIndependentInstance( -1.f / Decoder->GetAnimeFPS() );
		Decoder->Update();
	}
}

// �R�}����i���t���[���ցj
void FSsPlayer::ToNextFrame(bool bLoop)
{
	if( Decoder.IsValid() )
	{
		float AnimeFrame = FMath::FloorToFloat(Decoder->GetPlayFrame()) + 1.f;

		if(Decoder->GetAnimeEndFrame() <= AnimeFrame)
		{
			if(bLoop)
			{
				AnimeFrame -= Decoder->GetAnimeEndFrame();
			}
			else
			{
				AnimeFrame = Decoder->GetAnimeEndFrame();
			}
		}

		Decoder->SetPlayFrame( AnimeFrame );
		Decoder->SetDeltaForIndependentInstance( 1.f / Decoder->GetAnimeFPS() );
		Decoder->Update();
	}
}

// �p�[�c������C���f�b�N�X���擾
int32 FSsPlayer::GetPartIndexFromName(FName PartName) const
{
	if(Decoder.IsValid())
	{
		return Decoder->GetPartIndexFromName(PartName);
	}
	return -1;
}

// �p�[�c��Transform���擾(Canvas��2D���W�n)
FTransform FSsPlayer::GetPartTransformInCanvas(int32 PartIndex, int32 CanvasWidth, int32 CanvasHeight, FVector2D CenterLocation, float Rotation, FVector2D Scale) const
{
	if( Decoder.IsValid() )
	{
		FSsAnimeDecoder::DrawOption Option;
		Option.CenterLocation = CenterLocation;
		Option.Rotation = Rotation;
		Option.Scale = Scale;
		Option.bFlipH = bFlipH;
		Option.bFlipV = bFlipV;

		return Decoder->GetPartTransformInCanvas(PartIndex, CanvasWidth, CanvasHeight, Option);
	}
	return FTransform();
}
