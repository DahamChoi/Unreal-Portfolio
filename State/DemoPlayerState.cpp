// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerState.h"

const float ADemoPlayerState::Q_SKILL_MAX_PERCENT = 100.f;

ADemoPlayerState::ADemoPlayerState()
{
	bSprint = false;
	CurrentBulletCount = 30;
	MaxBulletCount = 30;
	CurrentHp = 100.f;
	MaxHp = 100.f;
	ESkillTimeRemaining = 0.f;
	FSkillTimeRemaining = 0.f;
	MaxESkillTimeRemaining = 10.f;
	MaxFSkillTimeRemaining = 10.f;
	QSkillPercent = 0.f;
	CurrentBattleForm = EBattleFormEnum::BFE_RIFLE;
	PlayerName = TEXT("SOLDIDER");
}

void ADemoPlayerState::ResetPlayerState()
{
	bSprint = false;
	CurrentBulletCount = MaxBulletCount;
	CurrentHp = MaxHp;
	ESkillTimeRemaining = 0.f;
	FSkillTimeRemaining = 0.f;
	QSkillPercent = 0.f;

	if (OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::SetDefaultState(
	float PMaxHp, int PMaxBulletCount,
	float PMaxESkillTimeRemaining, float PMaxFSkillTimeRemaining)
{
	this->MaxHp = PMaxHp;
	this->CurrentHp = this->MaxHp;
	this->MaxBulletCount = PMaxBulletCount;
	this->CurrentBulletCount = this->MaxBulletCount;
	this->MaxESkillTimeRemaining = PMaxESkillTimeRemaining;
	this->MaxFSkillTimeRemaining = PMaxFSkillTimeRemaining;

	if (OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::UpdateHpState(float NewHp)
{
	CurrentHp = FMath::Max(NewHp, 0.f);
	CurrentHp = FMath::Min(CurrentHp, MaxHp);

	if(OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::UpdateBulletCount(int NewBulletCount)
{
	CurrentBulletCount = NewBulletCount;
	
	if (OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::UpdateSkillTimeRemaining(float ESkill, float FSkill)
{
	ESkillTimeRemaining = FMath::Max(ESkill, 0.f);
	FSkillTimeRemaining = FMath::Max(FSkill, 0.f);

	if (OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::UpdateQSkillPercent(float Percent)
{
	QSkillPercent = FMath::Min(Percent, Q_SKILL_MAX_PERCENT);

	if (OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::UpdatebSprint(bool Sprint)
{
	bSprint = Sprint;

	if (OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::SetDemoPlayerName(FString Name)
{
	PlayerName = Name;

	if (OnPlayerStateChange.IsBound())
		OnPlayerStateChange.Broadcast();
}

void ADemoPlayerState::ChangeBattleForm()
{
	this->ESkillTimeRemaining = 0.f;
	this->FSkillTimeRemaining = 0.f;
	this->QSkillPercent = 0.f;

	CurrentBattleForm = (
		CurrentBattleForm == EBattleFormEnum::BFE_RIFLE ?
		EBattleFormEnum::BFE_HAND_GUN : EBattleFormEnum::BFE_RIFLE);

	if (OnPlayerFormChange.IsBound())
	{
		OnPlayerFormChange.Broadcast();

		if (OnPlayerStateChange.IsBound())
			OnPlayerStateChange.Broadcast();
	}
}
