// Fill out your copyright notice in the Description page of Project Settings.


#include "HPBarWidget.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"

void UHPBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FillAnimationsMap();
}

void UHPBarWidget::InitHPBarWidget(int HPBarNumber)
{
	for (int i = 0; i < HPBarNumber; i++)
	{
		FName WidgetName =
			FName(*(FString("HP_") + FString::FromInt(i + 1)));
		UBorder* Border = Cast<UBorder>(GetWidgetFromName(WidgetName));
		if (Border)
		{
			BorderArray.Add(Border);
			Border->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (BorderArray.Num() > 0)
	{
		OriginalColor = (*BorderArray.begin())->BrushColor;
	}
}

void UHPBarWidget::UpdateHPWidget(float UpdateHP)
{
	int Size = BorderArray.Num();
	int MAX_HP = Size * 25;
	for (int i = 0; i < Size; i++)
	{
		float HpAlpha = UpdateHP - (i * (MAX_HP / Size));
		HpAlpha = (HpAlpha < 0) ? 0.2f : (0.2f + (HpAlpha * 0.032f));
		BorderArray[i]->SetBrushColor(FLinearColor(OriginalColor.R, OriginalColor.G, OriginalColor.B, HpAlpha));
		BorderArray[i]->SetVisibility(ESlateVisibility::Visible);
	}
}

void UHPBarWidget::SetNameText(FString Name)
{
	NameText = Cast<UTextBlock>(GetWidgetFromName("NAME_TEXT"));
	NameText->SetText(FText::FromString(Name));
}

void UHPBarWidget::DisableHPBarWidget()
{
	PlayAnimationByName("DisableAnimation", 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
}

UWidgetAnimation* UHPBarWidget::GetAnimationByName(FName AnimationName) const
{
	UWidgetAnimation* const* WidgetAnim = AnimationsMap.Find(AnimationName);
	if (WidgetAnim)
	{
		return *WidgetAnim;
	}
	return nullptr;
}

bool UHPBarWidget::PlayAnimationByName(FName AnimationName, float StartAtTime, int32 NumLoopsToPlay, EUMGSequencePlayMode::Type PlayMode, float PlaybackSpeed)
{
	UWidgetAnimation* WidgetAnim = GetAnimationByName(AnimationName);
	if (WidgetAnim)
	{
		PlayAnimation(WidgetAnim, StartAtTime, NumLoopsToPlay, PlayMode, PlaybackSpeed);
		return true;
	}
	return false;
}

void UHPBarWidget::FillAnimationsMap()
{
	AnimationsMap.Empty();

	UProperty* Prop = GetClass()->PropertyLink;

	// Run through all properties of this class to find any widget animations
	while (Prop != nullptr)
	{
		// Only interested in object properties
		if (Prop->GetClass() == UObjectProperty::StaticClass())
		{
			UObjectProperty* ObjProp = Cast<UObjectProperty>(Prop);

			// Only want the properties that are widget animations
			if (ObjProp->PropertyClass == UWidgetAnimation::StaticClass())
			{
				UObject* Obj = ObjProp->GetObjectPropertyValue_InContainer(this);

				UWidgetAnimation* WidgetAnim = Cast<UWidgetAnimation>(Obj);

				if (WidgetAnim != nullptr && WidgetAnim->MovieScene != nullptr)
				{
					FName AnimName = WidgetAnim->MovieScene->GetFName();
					AnimationsMap.Add(AnimName, WidgetAnim);
				}
			}
		}

		Prop = Prop->PropertyLinkNext;
	}
}
