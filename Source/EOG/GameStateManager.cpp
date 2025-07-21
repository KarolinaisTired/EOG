// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateManager.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Kismet/GameplayStatics.h"

AGameStateManager::AGameStateManager()
{
}

void AGameStateManager::BeginPlay()
{
	Super::BeginPlay();
	PreviousSpawnLocation = FVector(400.f,0.f,0.f);
	CurrentState = EGameState::HouseOne;
	FString EnumName = UEnum::GetValueAsString(CurrentState);

	if (HouseLevelInstances.Num() > 0)
	{
		int32 HouseIndex = static_cast<int32>(CurrentState);
		TSoftObjectPtr<UWorld> SoftLevelToLoad = HouseLevelInstances[HouseIndex];
		UWorld* HouseToLoad = SoftLevelToLoad.LoadSynchronous();
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		FVector SpawnLocation = PlayerLocation + FVector(0.f, 0.f, -200.f);

		if (HouseToLoad)
		{
			bool bLevelLoaded = false;

			ULevelStreamingDynamic* LoadedHouse = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
				GetWorld(), HouseToLoad, SpawnLocation, FRotator::ZeroRotator, bLevelLoaded);
			PreviousSpawnLocation = SpawnLocation;
			NextHouseToUnload = LoadedHouse;
		}
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, EnumName);

	OnGameStateChanged.AddUniqueDynamic(this, &AGameStateManager::HandleGameStateChange);
	FOnPlayerInHouse.AddUniqueDynamic(this, &AGameStateManager::OnPlayerInHouse);
}

void AGameStateManager::HandleGameStateChange(EGameState CurState, bool CorrectBunnyChosen)
{
	int32 NewStateIndex = static_cast<int32>(CurrentState); 
	if (CorrectBunnyChosen)
	{
		if (NewStateIndex < MaxStates)
		{
			NewStateIndex = static_cast<int32>(CurrentState) + 1; // Move onto the next house
			CurrentState = static_cast<EGameState>(NewStateIndex);
			FString EnumName = UEnum::GetValueAsString(CurrentState);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player had Right bunny"));
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, EnumName);
			LoadHouse(NewStateIndex);
			OpenDoorToNextHouse();
		}
		else // Player has completed the final house 
		{
			// TODO Trigger maybe BP instance to show boxes burning and bunny's head coming off, 
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Omg you won queen"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Player did not have bunny"));
		LoadHouse(NewStateIndex);
		OpenDoorToNextHouse(); 
	}
}

void AGameStateManager::OnPlayerInHouse() // Unload previous level once player has entered a new house
{
	if (NextHouseToUnload != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Previous house unloading"));
		NextHouseToUnload->SetIsRequestingUnloadAndRemoval(true);
		FLatentActionInfo LatentInfo;
		UGameplayStatics::UnloadStreamLevelBySoftObjectPtr(this,NextHouseToUnload,LatentInfo, false); 
	}
	NextHouseToUnload = CurrentHouse;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Entered new house"));
}

void AGameStateManager::LoadHouse(int StateIndex)
{
	if (HouseLevelInstances.IsValidIndex(StateIndex))
	{
		TSoftObjectPtr<UWorld> SoftLevelToLoad = HouseLevelInstances[StateIndex];
		UWorld* HouseToLoad = SoftLevelToLoad.LoadSynchronous();
		FVector SpawnLocation = PreviousSpawnLocation + LevelSize;
		if (HouseToLoad)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Next house loading"));
			bool bLevelLoaded = false;
			ULevelStreamingDynamic* LoadedHouse = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
				GetWorld(), HouseToLoad, SpawnLocation, FRotator::ZeroRotator, bLevelLoaded);
			PreviousSpawnLocation = SpawnLocation;
			CurrentHouse = LoadedHouse; 
		}
	}
}

void AGameStateManager::BroadcastStateChange(EGameState NewState, bool BunnyChosen)
{
	OnGameStateChanged.Broadcast(NewState, BunnyChosen);
}

void AGameStateManager::BroadcastPlayerInHouse()
{
	FOnPlayerInHouse.Broadcast(); 
}

EGameState AGameStateManager::GetCurrentState() const
{
	return CurrentState;
}
