// Fill out your copyright notice in the Description page of Project Settings.


#include "GameStateManager.h"
#include "Engine/LevelStreamingDynamic.h"

// Sets default values
AGameStateManager::AGameStateManager()
{
	// PrimaryActorTick.bCanEverTick = true;
}

void AGameStateManager::BeginPlay()
{
	Super::BeginPlay();
	CurrentState = EGameState::HouseOne;
	FString EnumName = UEnum::GetValueAsString(CurrentState);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, EnumName);

	OnGameStateChanged.AddUniqueDynamic(this, &AGameStateManager::HandleGameStateChange);
	FOnPlayerInHouse.AddUniqueDynamic(this, &AGameStateManager::OnPlayerInHouse);
}

void AGameStateManager::HandleGameStateChange(EGameState CurState, bool CorrectBunnyChosen)
{
	int32 PreviousHouseIndex = static_cast<int32>(CurrentState);
	StreamedHouseLevel = nullptr; 
	if (HouseLevelInstances.IsValidIndex(PreviousHouseIndex))
	{
		TSoftObjectPtr<UWorld> SoftLevelToUnload = HouseLevelInstances[PreviousHouseIndex];
		UWorld* HouseToUnload = SoftLevelToUnload.LoadSynchronous();
		if (HouseToUnload)
		{
			for (ULevelStreaming* Streamed : GetWorld()->GetStreamingLevels())
			{
				if (ULevelStreamingDynamic* Dyn = Cast<ULevelStreamingDynamic>(Streamed))
				{
					if (Dyn->GetWorldAsset() == HouseToUnload)
					{
						StreamedHouseLevel = Dyn; // Cache to unload 
					}
				}
			}
		}
	}

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
	// TODO Unload previous instance
	if (StreamedHouseLevel != nullptr)
	{
		StreamedHouseLevel->SetShouldBeLoaded(false);
		StreamedHouseLevel->SetShouldBeVisible(false);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Entered new house"));
	// TODO Here we pull the associated house with the game state and load it
}

void AGameStateManager::LoadHouse(int StateIndex)
{
	if (HouseLevelInstances.IsValidIndex(StateIndex))
	{
		TSoftObjectPtr<UWorld> SoftLevelToLoad = HouseLevelInstances[StateIndex];
		UWorld* HouseToLoad = SoftLevelToLoad.LoadSynchronous();
		bool bLevelLoaded = false;

		// TODO here we need to plug in the location of the previous level plus the same dimensions. 
		FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
		if (HouseToLoad)
		{
			ULevelStreamingDynamic* LoadedHouse = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
				GetWorld(), HouseToLoad, PlayerLocation, FRotator::ZeroRotator, bLevelLoaded);
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
