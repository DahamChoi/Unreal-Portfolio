// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../UI/MainUserWidget.h"
#include "CharacterBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnDeadDelegate);

UCLASS()
class DEMO_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Take Damage
	virtual float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	virtual void OnDeadFunction();

	UFUNCTION()
	void SetDefaultCharacterState(
		float MaxHp, FString PlayerName = TEXT("NONE"), int MaxBulletCount = -1,
		float MaxESkillTimeReamining = -1.f, float MaxFSkillTimeRemaining = -1.f);

	UFUNCTION(BlueprintCallable)
	void RagDollDead();

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HPBarWidget;

	UPROPERTY(VisibleAnywhere)
	class UHPBarWidget* HPBarWidgetObject;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	class ADemoPlayerState* CurrentPlayerState;

	// AiPerception Source
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAIPerceptionStimuliSourceComponent* AIStimuliSource;

	// Sound
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* TakeDownSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* DieSoundCue;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* TakeDownAudioComponent;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* DieAudioComponent;

	UPROPERTY(VisibleAnywhere, Blueprintreadwrite)
	bool IsDead;

	UPROPERTY()
	FTimerHandle DisableHpWidgetTimer;

	UPROPERTY()
	FTimerHandle DestoryActorTimer;

	FOnDeadDelegate OnDead;
};
