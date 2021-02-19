// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "LevelStreamingActor.generated.h"

USTRUCT(BlueprintType)
struct DEMO_API FFunctionStruct
{
	GENERATED_USTRUCT_BODY()

public:
	void(ALevelStreamingActor::*pFunc)();
};

UCLASS()
class DEMO_API ALevelStreamingActor : public ATriggerVolume
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelStreamingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// overlap begin function
	UFUNCTION()
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION()
	const FTransform& GetSpawnTransfrom();

	void SpawnSphereRoBotFunction();
	void SpawnSciFiSoldierTypeAFunction();
	void SpawnSciFiSoldierTypeBFunction();
	void SpawnSuperDuskFunction();

public:
	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	TArray<class ATargetPoint*> TargetArray;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	int LevelSpawnEnemyNubmer;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	float SpawnDealy;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	bool SpawnSphereRobot;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	bool SpawnSciFiSoldierTypeA;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	bool SpawnSciFiSoldierTypeB;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	bool SpawnSuperDusk;

	UPROPERTY()
	TArray<FFunctionStruct> SpawnFunctionArray;

	UPROPERTY()
	float DelayTimer;

	UPROPERTY()
	int SpawnCounter;

	UPROPERTY()
	bool LevelTriggerOn;

	// Sound
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* BattleMusicSoundCue;

	// Attack Comminacing
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* AnnounceSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* BattleMusicAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AnnounceAudioComponent;

	FActorSpawnParameters SpawnParameters;
};
