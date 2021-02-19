// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoGameStateBase.h"

ADemoGameStateBase::ADemoGameStateBase()
{
	PayloadState = EPayloadStateEnum::PSE_STOP;
	PayloadAlphaDistance = 0.f;
}

void ADemoGameStateBase::UpdatePayloadState(EPayloadStateEnum State, float PayloadAlpha)
{
	PayloadState = State;
	PayloadAlphaDistance = PayloadAlpha;

	OnGameStateChange.Broadcast();
}

void ADemoGameStateBase::AddKillLog(FString KillerName, FString KilledName)
{
	FKillLog KillLog;
	KillLog.KillerName = KillerName;
	KillLog.KilledName = KilledName;
	KillLogArray.Add(KillLog);

	OnKillLogUpdate.Broadcast();
}