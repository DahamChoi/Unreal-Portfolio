// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API UHPBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	void InitHPBarWidget(int HPBarNumber);
	void UpdateHPWidget(float UpdateHP);
	void SetNameText(FString Name);
	void DisableHPBarWidget();

	UWidgetAnimation* GetAnimationByName(FName AnimationName) const;
	bool PlayAnimationByName(FName AnimationName,
		float StartAtTime,
		int32 NumLoopsToPlay,
		EUMGSequencePlayMode::Type PlayMode,
		float PlaybackSpeed);
	void FillAnimationsMap();

private:
	UPROPERTY()
	TArray<class UBorder*> BorderArray;

	UPROPERTY()
	class UTextBlock* NameText;

	UPROPERTY()
	TMap<FName, UWidgetAnimation*> AnimationsMap;

	UPROPERTY()
	FLinearColor OriginalColor;
};
