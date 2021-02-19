// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_TeamRifle_BattleServ.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "../../Character/Player/PlayerCharacterRifleForm.h"
#include "../../Controller/BaseAIController.h"
#include "../../Actor/BioticFieldActor.h"

const FName UBTService_TeamRifle_BattleServ::IS_ULTIMATE_ON_KEY = TEXT("IsUltimateOn");
const FName UBTService_TeamRifle_BattleServ::BIOTIC_FIELD_LOCATION_KEY = TEXT("BioticFiledLocation");

UBTService_TeamRifle_BattleServ::UBTService_TeamRifle_BattleServ()
{

}

void UBTService_TeamRifle_BattleServ::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Is Ultimate On?
	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	APlayerCharacterRifleForm* CharacterRifleForm = 
		Cast<APlayerCharacterRifleForm>(ControllingPawn);
	if (CharacterRifleForm)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(
			IS_ULTIMATE_ON_KEY, CharacterRifleForm->PCActionIsUltimate);
	}

	// BioticField Location
	ABioticFieldActor* PendingBioticField = nullptr;
	for (TObjectIterator<ABioticFieldActor> Itr; Itr; ++Itr)
	{
		// Is Valid
		if (Itr)
		{
			if (PendingBioticField == nullptr) 
			{
				PendingBioticField = *Itr; continue;
			}
			if (Itr->GetDistanceTo(CharacterRifleForm) < 
				PendingBioticField->GetDistanceTo(CharacterRifleForm))
			{
				PendingBioticField = *Itr;
			}
		}
	}

	if (PendingBioticField)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(
			BIOTIC_FIELD_LOCATION_KEY, PendingBioticField->GetActorLocation());
	}
	else
	{
		OwnerComp.GetBlackboardComponent()->ClearValue(BIOTIC_FIELD_LOCATION_KEY);
	}
}