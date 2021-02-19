// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DemoPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnPlayerStateChangeDelegate);
DECLARE_MULTICAST_DELEGATE(FOnPlayerFormChangeDelegate);

UENUM(BlueprintType)
enum class EBattleFormEnum : uint8
{
	BFE_RIFLE UMETA(DisplayName = "RIFLE"),
	BFE_HAND_GUN UMETA(DisplayName = "HAND_GUN")
};

/**
 * 
 */
UCLASS()
class DEMO_API ADemoPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ADemoPlayerState();

	void ResetPlayerState();

	void SetDefaultState(
		float PMaxHp, int PMaxBulletCount = -1,
		float PMaxESkillTimeRemaining = -1.f, float PMaxFSkillTimeRemaining = -1.f);

	void UpdateHpState(float NewHp);

	void UpdateBulletCount(int NewBulletCount);

	void UpdateSkillTimeRemaining(float ESkill, float FSkill);

	void UpdateQSkillPercent(float Percent);

	void UpdatebSprint(bool Sprint);

	void SetDemoPlayerName(FString Name);

	void ChangeBattleForm();

public:
	UPROPERTY()
	bool bSprint;

	UPROPERTY()
	int CurrentBulletCount;

	UPROPERTY()
	int MaxBulletCount;

	UPROPERTY()
	float MaxHp;

	UPROPERTY()
	float CurrentHp;

	UPROPERTY()
	float ESkillTimeRemaining;

	UPROPERTY()
	float FSkillTimeRemaining;

	UPROPERTY()
	float MaxESkillTimeRemaining;

	UPROPERTY()
	float MaxFSkillTimeRemaining;

	UPROPERTY()
	float QSkillPercent;

	UPROPERTY()
	EBattleFormEnum CurrentBattleForm;

	UPROPERTY()
	FString PlayerName;

	FOnPlayerStateChangeDelegate OnPlayerStateChange;
	FOnPlayerFormChangeDelegate OnPlayerFormChange;

public:
	static const float Q_SKILL_MAX_PERCENT;
};
