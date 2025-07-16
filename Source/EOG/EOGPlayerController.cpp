// Copyright Epic Games, Inc. All Rights Reserved.


#include "EOGPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "EOGCameraManager.h"

AEOGPlayerController::AEOGPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AEOGCameraManager::StaticClass();
}

void AEOGPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
}
