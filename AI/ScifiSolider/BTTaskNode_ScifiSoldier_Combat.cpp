// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_ScifiSoldier_Combat.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "../../Actor/ScifiWeapon.h"
#include "../../Controller/BaseAIController.h"
#include "../../Character/Enemy/PlayerCharacterRifleFormEnemy.h"
#include "../Common/BTService_Common_TargetDetecter.h"
#include "../../State/DemoPlayerState.h"

const FName UBTTaskNode_ScifiSoldier_Combat::FIRE_TIMER_KEY = TEXT("FireTimer");

UBTTaskNode_ScifiSoldier_Combat::UBTTaskNode_ScifiSoldier_Combat()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_ScifiSoldier_Combat::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	// Set Fire Timer Default
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, 0.f);

	// Ethernal Progress Task
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_ScifiSoldier_Combat::TickTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Pawn As PlayerCharacterRifleFormEnemy
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterRifleFormEnemy* RifleEnemy =
		Cast<APlayerCharacterRifleFormEnemy>(ControllingPawn);

	// Get Current FireTimer Value
	float FireTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(FIRE_TIMER_KEY);
	
	// Turn To Target
	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(
		UBTService_Common_TargetDetecter::TARGET_KEY);
	AActor* Target = Cast<AActor>(TargetObject);
	if (RifleEnemy)
	{
		if (Target)
		{
			FVector TargetLocation = Target->GetActorLocation();
			// FireHole Location
			FVector OriginLocation = RifleEnemy->GetMesh()->GetSocketLocation("WeaponSocket") + FTransform(
				RifleEnemy->Weapon->GetActorRotation()).TransformPosition(FVector(100.f, 0.f, 8.f));

			FVector LookVector = TargetLocation - OriginLocation;
			LookVector.Z = 0.f;
			FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

			// Rotate To Target
			ControllingPawn->SetActorRotation(TargetRot);

			// Line Trace
			FVector Start = OriginLocation;
			FVector End = Start + (RifleEnemy->GetCapsuleComponent()->GetForwardVector() * 3000.f);

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;

			bool bCanTrace = GetWorld()->LineTraceSingleByChannel(
				HitResult, Start, End, ECollisionChannel::ECC_WorldDynamic, CollisionParams);

			// Fire
			if (RifleEnemy->CurrentPlayerState->CurrentBulletCount > 0 &&
				!RifleEnemy->PCActionIsFire && bCanTrace && FireTimer > 1.f)
			{
				RifleEnemy->PCActionFire();
				OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, 0.f);
			}
			// Stop Fire
			else if (FireTimer > 0.3f && RifleEnemy->PCActionIsFire)
			{
				RifleEnemy->PCActionStopFire();
			}

			// Reload
			if (RifleEnemy->CurrentPlayerState->CurrentBulletCount <= 0)
			{
				RifleEnemy->PCActionStopFire();
				RifleEnemy->PCActionReload();
			}

			// Skill
			if (RifleEnemy->CurrentPlayerState->CurrentHp < 100.f &&
				FMath::IsNearlyEqual(RifleEnemy->CurrentPlayerState->ESkillTimeRemaining, 0.f))
			{
				RifleEnemy->PCActionBioticFiled();
			}

			if (RifleEnemy->CurrentPlayerState->CurrentHp < 50.f &&
				FMath::IsNearlyEqual(RifleEnemy->CurrentPlayerState->FSkillTimeRemaining, 0.f) && bCanTrace)
			{
				RifleEnemy->PCActionRocketLauncher();
			}
		}
	}

	// Update Fire Timer
	FireTimer = OwnerComp.GetBlackboardComponent()->GetValueAsFloat(FIRE_TIMER_KEY);
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(FIRE_TIMER_KEY, FireTimer + DeltaSeconds);

	//	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}