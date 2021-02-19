// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DemoGameStateBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnGameStateChangeDelegate);

DECLARE_MULTICAST_DELEGATE(FOnKillLogUpdateDelegate);

UENUM(BlueprintType)
enum class EPayloadStateEnum : uint8
{
	PSE_MOVE UMETA(DisplayName = "MOVE"),
	PSE_STOP UMETA(DisplayName = "STOP"),
	PSE_FIGHTING UMETA(DisplayName = "FIGHINT")
};

USTRUCT(BlueprintType)
struct DEMO_API FKillLog
{
	GENERATED_USTRUCT_BODY()

public:
	FString KillerName;
	FString KilledName;
};

/**
 * 
 */
UCLASS()
class DEMO_API ADemoGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ADemoGameStateBase();

	void UpdatePayloadState(EPayloadStateEnum State, float PayloadAlpha);

	void AddKillLog(FString KillerName, FString KilledName);

public:
	UPROPERTY(BlueprintReadwrite)
	EPayloadStateEnum PayloadState;

	UPROPERTY(BlueprintReadwrite)
	float PayloadAlphaDistance;

	UPROPERTY(BlueprintReadwrite)
	TArray<FKillLog> KillLogArray;

	FOnGameStateChangeDelegate OnGameStateChange;
	FOnKillLogUpdateDelegate OnKillLogUpdate;
};
