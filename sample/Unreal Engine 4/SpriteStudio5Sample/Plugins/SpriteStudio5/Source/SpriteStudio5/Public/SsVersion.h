#pragma once

//
// �Ή�UE�o�[�W�����؂�ւ��p
//
#define SS_UE4_4
//#define SS_UE4_5
//#define SS_UE4_6


//
//	UE4.5/UE4.6 �Ŏg�p����ꍇ�A�G���W���R�[�h���C�����ā��̖�����������K�v������܂��B 
//	���̖����������Ȃ��ƁASsPlayer�R���|�[�l���g�쐬��A�����Ƀv���O�������N���b�V�����܂��B 
//		https://answers.unrealengine.com/questions/121155/45-urgent-solus-making-an-fcanvas-for-render-textu.html
//	�ŒZ�̏C�����@�Ƃ��ẮACanvasRenderTarget2D.cpp UCanvasRenderTarget2D::UpdateResource() ���̉��L�̉ӏ����C�����ĉ������B 
//		// Create the FCanvas which does the actual rendering.
//		�~�FFCanvas RenderCanvas(GameThread_GetRenderTargetResource(), nullptr, FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime, GetWorld()->FeatureLevel);
//		���FFCanvas RenderCanvas(GameThread_GetRenderTargetResource(), nullptr, FApp::GetCurrentTime() - GStartTime, FApp::GetDeltaTime(), FApp::GetCurrentTime() - GStartTime, GMaxRHIFeatureLevel);
//
//	3D���b�V���ւ̕`����s�킸�AHUD�p�r�݂̂ł���΂��̂܂܂ł��g�p�\�ł��B 
//	��L�̖����������AUE4.5/4.6��3D���b�V���ւ̕`����g�p����ꍇ�́ASS_DISABLE_DRAW_MESH�𖳌������ĉ������B 
//
#if defined(SS_UE4_5) || defined(SS_UE4_6)
#define SS_DISABLE_DRAW_MESH
#endif


//
//	UE4.5�ȍ~�Ńr���h����ꍇ�́A���#define�����łȂ��A SpriteStudio5Ed.Build.cs 44�s�ڂ̃R�����g�A�E�g���O���ĉ����� 
//


//
//	�`�悪�㉺���]����s��ɂ��� 
//		UE4.4�̃r���h��Android�̎��@��œ��������ۂɁA�`�悪�㉺���]����s����m�F����Ă��܂��B 
//		���̖���UE4.5�ȍ~�ł͏C������Ă��܂��B 
//			�i�Q�ƁFUE4.5�ȍ~�̃G���W���R�[�h�ŁubNeedsToSwitchVerticalAxis�v�������j 
//		���L��#define��L���ɂ���ƁA���̃v���O�C������̕`����㉺���]���܂��B 
//		�A���A���̏C���͑��Ȃ̂��̂ŁAWindows�ł̕`������]���Ă��܂��܂��̂ŁA�������������B 
//
//#define SS_SWITCH_VERTICAL_AXIS

