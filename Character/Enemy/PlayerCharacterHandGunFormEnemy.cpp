// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacterHandGunFormEnemy.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Controller/SciFiSoldierHGAIController.h"

APlayerCharacterHandGunFormEnemy::APlayerCharacterHandGunFormEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// AIController Set
	AIControllerClass = ASciFiSoldierHGAIController::StaticClass();

	// User Widget
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(
		TEXT("/Game/Demo/UI/HPBar_SciFiSoldier"));
	if (UI_HUD.Succeeded())
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 105.f));

	// Init Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM_PC_ENEMY(
		TEXT("/Game/SciFiSoldier/Meshes/SK_ScifiSoldierUE4_Enemy"));

	if (SM_PC_ENEMY.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SM_PC_ENEMY.Object);

		FString AnimClassStringTest =
			"Class'/Game/TPS_HandGun/TPS_HandGun/Animation/HandGun_AnimBlueprint_Enemy.HandGun_AnimBlueprint_Enemy_C'";
		UClass* AnimationClass = LoadObject<UClass>(NULL, *AnimClassStringTest);
		if (AnimationClass)
		{
			GetMesh()->SetAnimInstanceClass(AnimationClass);
		}
	}

	// Movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);

	// Player Name
	PlayerName = TEXT("ELITE SOLDIER TYPE-B");
}

void APlayerCharacterHandGunFormEnemy::BeginPlay()
{
	Super::BeginPlay();

}

void APlayerCharacterHandGunFormEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
