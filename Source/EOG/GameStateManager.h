// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameStateManager.generated.h"

UCLASS(BlueprintType, Blueprintable)
class EOG_API AGameStateManager : public AActor
{
	GENERATED_BODY()

public:
	AGameStateManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
