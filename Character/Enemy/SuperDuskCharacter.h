// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CharacterBase.h"
#include "SuperDuskCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnEndAttackDelegate);

/**
 * 
 */
UCLASS()
class DEMO_API ASuperDuskCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASuperDuskCharacter();

	UFUNCTION(BlueprintCallable)
	void AttackHeadOn();

	UFUNCTION(BlueprintCallable)
	void OnEndAttackFunction();

	UFUNCTION()
	void AttackTakeDamage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite)
	bool IsAttack;

	FOnEndAttackDelegate OnEndAttack;

	static const float MAX_HP;
};
