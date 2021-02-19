// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../CharacterBase.h"
#include "PrimeHelixCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnHelixEndAttackDelegate);

/**
 * 
 */
UCLASS()
class DEMO_API APrimeHelixCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	APrimeHelixCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void AttackHeadOn();

	UFUNCTION(BlueprintCallable)
	void OnEndAttackFunction();

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite)
	bool IsAttack;

	FOnHelixEndAttackDelegate OnEndAttack;

	static const float MAX_HP;
};
