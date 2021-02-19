// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "DemoPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API ADemoPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
	
public:
	ADemoPlayerController();

	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* aPawn) override;

	void MainUserWidgetBindTeamPlayerState(class ADemoPlayerState* State);
	void InitializeMainWidget();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = UI)
	TSubclassOf<class UMainUserWidget> MainUserWidgetClass;

private:
	UPROPERTY()
	class UMainUserWidget* MainUserWidget;

	// Implement The Generic Team Interface 
	FGenericTeamId TeamId;
	FGenericTeamId GetGenericTeamId() const;
};
