// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_TeamRifle_Combat.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "../../Character/Player/PlayerCharacterRifleForm.h"
#include "../../Controller/BaseAIController.h"
#include "../Common/BTService_Common_TargetDetecter.h"
#include "../../Actor/ScifiWeapon.h"
#include "../../State/DemoPlayerState.h"

UBTTaskNode_TeamRifle_Combat::UBTTaskNode_TeamRifle_Combat()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_TeamRifle_Combat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (ControllingPawn)
	{
		ControllingPawn->bUseControllerRotationYaw = false;
	}

	// Ethernal Progress Task
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_TeamRifle_Combat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	// Get Pawn As PlayerCharacterRifleFormEnemy
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterRifleForm* CharacterRifleForm =
		Cast<APlayerCharacterRifleForm>(ControllingPawn);

	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(
		UBTService_Common_TargetDetecter::TARGET_KEY);
	AActor* Target = Cast<AActor>(TargetObject);
	if (CharacterRifleForm)
	{
		if (Target)
		{
			ACharacterBase* TargetCharacter = Cast<ACharacterBase>(Target);
			if (TargetCharacter)
			{
				if (TargetCharacter->IsDead)
				{
					OwnerComp.GetBlackboardComponent()->ClearValue(UBTService_Common_TargetDetecter::TARGET_KEY);
				}
			}

			FVector TargetLocation = Target->GetActorLocation();
			// FireHole Location
			FVector OriginLocation = CharacterRifleForm->GetMesh()->GetSocketLocation("WeaponSocket") + FTransform(
				CharacterRifleForm->Weapon->GetActorRotation()).TransformPosition(FVector(100.f, 0.f, 8.f));

			FVector LookVector = TargetLocation - OriginLocation;
			LookVector.Z = 0.f;
			FRotator TargetRot = FRotationMatrix::MakeFromX(LookVector).Rotator();

			// Rotate To Target
			ControllingPawn->SetActorRotation(TargetRot);

			// Line Trace
			FVector Start = OriginLocation;
			FVector End = Start + (CharacterRifleForm->GetCapsuleComponent()->GetForwardVector() * 3000.f);

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;

			bool bCanTrace = GetWorld()->LineTraceSingleByChannel(
				HitResult, Start, End, ECollisionChannel::ECC_WorldDynamic, CollisionParams);
			bCanTrace = (HitResult.Actor == Target) && bCanTrace;

			// Fire
			if (bCanTrace && !CharacterRifleForm->PCActionIsFire 
				&& CharacterRifleForm->CurrentPlayerState->CurrentBulletCount > 0 ||
				(CharacterRifleForm->PCActionIsUltimate && !CharacterRifleForm->PCActionIsFire))
			{
				CharacterRifleForm->PCActionFire();
			}
			// Stop Fire
			else if (!bCanTrace && CharacterRifleForm->PCActionIsFire)
			{
				CharacterRifleForm->PCActionStopFire();
			}

			// Reload
			if(CharacterRifleForm->CurrentPlayerState->CurrentBulletCount <= 0)
			{
				CharacterRifleForm->PCActionStopFire();
				CharacterRifleForm->PCActionReload();
			}

			// Skill
			if (CharacterRifleForm->CurrentPlayerState->CurrentHp < 100.f &&
				FMath::IsNearlyEqual(CharacterRifleForm->CurrentPlayerState->ESkillTimeRemaining, 0.f))
			{
				CharacterRifleForm->PCActionBioticFiled();
			}

			if (FMath::IsNearlyEqual(
				CharacterRifleForm->CurrentPlayerState->FSkillTimeRemaining, 0.f) && bCanTrace)
			{
				CharacterRifleForm->PCActionRocketLauncher();
			}

			// Ultimate
			if (FMath::IsNearlyEqual(
				CharacterRifleForm->CurrentPlayerState->QSkillPercent, 100.f))
			{
				CharacterRifleForm->PCActionUltimate();
			}
		}
	}
}
