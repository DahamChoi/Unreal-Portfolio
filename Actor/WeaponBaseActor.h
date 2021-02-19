// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBaseActor.generated.h"

UCLASS()
class DEMO_API AWeaponBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* WeaponStaticMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
