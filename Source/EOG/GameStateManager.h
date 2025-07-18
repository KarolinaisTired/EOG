// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/Actor.h"
#include "LevelInstance/LevelInstanceActor.h"
#include "GameStateManager.generated.h"

UENUM(BlueprintType)
enum class EGameState : uint8
{
	HouseOne      UMETA(DisplayName = "House One"),
	HouseTwo       UMETA(DisplayName = "House Two"),
	HouseThree       UMETA(DisplayName = "HouseThree"),
	HouseFour       UMETA(DisplayName = "House Four"),
	HouseFive       UMETA(DisplayName = "House Five"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameStateChanged, EGameState, NewState, bool, CorrectBunnyChose);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerInHouse);

UCLASS(BlueprintType, Blueprintable)
class EOG_API AGameStateManager : public AActor
{
	GENERATED_BODY()

public:
	AGameStateManager();
	UFUNCTION() void HandleGameStateChange(EGameState OldState, bool CorrectBunnyChosen);
	UFUNCTION() void OnPlayerInHouse();
	UFUNCTION() void LoadHouse(int StateIndex); 
	
	UFUNCTION(BlueprintCallable) void BroadcastStateChange(EGameState CurStateToBroadcast, bool BunnyChosen);
	UFUNCTION(BlueprintCallable) void BroadcastPlayerInHouse();
	UFUNCTION(BlueprintCallable) EGameState GetCurrentState() const;
	UFUNCTION(BlueprintImplementableEvent) void OpenDoorToNextHouse();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) TArray<TSoftObjectPtr<UWorld>> HouseLevelInstances;
	UPROPERTY(BlueprintReadWrite) FOnGameStateChanged OnGameStateChanged;
	UPROPERTY(BlueprintReadWrite) FOnPlayerInHouse FOnPlayerInHouse;

	UPROPERTY() EGameState CurrentState;

protected:
	virtual void BeginPlay() override;

private:
	int MaxStates = 4;
	UPROPERTY() ULevelStreamingDynamic* StreamedHouseLevel; 
};

// TODO if we choose wrong bunny, broadcast delegate with boolean of true or false, the BP of this will listen to that, if true, we get the current index of gamestate,
// then set the CurrentState + 1 to whatever that is, then match that index to the ones in the level instance array, we then load that level instance in front of the current instance.
