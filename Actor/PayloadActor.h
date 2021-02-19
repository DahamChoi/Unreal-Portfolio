// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PayloadActor.generated.h"

UCLASS()
class DEMO_API APayloadActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APayloadActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	/** Dummy root component */
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* PayloadSkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class ATrackActor* Track;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ActivePayload;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Alpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanMove;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	float Duration;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite)
	float Forward;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite)
	int OurPlayerInCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite)
	int EnemyInCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadwrite)
	TArray<class ACharacterBase*> InEnemyArray;

	// Sound
	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* AnnounceCue1;			// Victory

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class USoundCue* AnnounceCue2;			// The Payload Is Approacing It's Destination

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AnnounceAudioComponent1;

	UPROPERTY(VisibleAnywhere, Category = "Audio")
	class UAudioComponent* AnnounceAudioComponent2;

	UPROPERTY()
	bool AnnounceChecker1;

	UPROPERTY()
	bool AnnounceChecker2;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void SetCanMovePayload();
};
