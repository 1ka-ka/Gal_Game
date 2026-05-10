#include "Game/VNPlayerController.h"
#include "Game/VNGameMode.h"
#include "Core/VNGameInstance.h"
#include "Core/VNSaveManager.h"
#include "Components/InputComponent.h"

AVNPlayerController::AVNPlayerController()
{
	bShowMouseCursor = true;
}

void AVNPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction("AdvanceDialogue", IE_Pressed, this, &AVNPlayerController::OnAdvanceDialogue);
		InputComponent->BindAction("TogglePause", IE_Pressed, this, &AVNPlayerController::OnTogglePause);
		InputComponent->BindAction("QuickSave", IE_Pressed, this, &AVNPlayerController::OnQuickSave);
		InputComponent->BindAction("ToggleAuto", IE_Pressed, this, &AVNPlayerController::OnToggleAuto);
	}
}

void AVNPlayerController::OnAdvanceDialogue()
{
	AVNGameMode* GM = GetWorld()->GetAuthGameMode<AVNGameMode>();
	if (GM)
	{
		GM->OnAdvanceDialogue();
	}
}

void AVNPlayerController::OnTogglePause()
{
	AVNGameMode* GM = GetWorld()->GetAuthGameMode<AVNGameMode>();
	if (GM)
	{
		GM->OnTogglePause();
	}
}

void AVNPlayerController::OnQuickSave()
{
	UVNGameInstance* GI = GetGameInstance<UVNGameInstance>();
	if (GI && GI->IsGameInProgress())
	{
		AVNGameMode* GM = GetWorld()->GetAuthGameMode<AVNGameMode>();
		if (GM)
		{
			int32 Slot = GI->GetActiveSaveSlot();
			if (Slot < 0)
			{
				UVNSaveManager* SM = GI->GetSaveManager();
				for (int32 i = 0; i < SM->GetMaxSaveSlots(); i++)
				{
					if (!SM->HasSave(i))
					{
						Slot = i;
						break;
					}
				}
			}
			if (Slot >= 0)
			{
				GM->OnSaveGame(Slot);
			}
		}
	}
}

void AVNPlayerController::OnToggleAuto()
{
	AVNGameMode* GM = GetWorld()->GetAuthGameMode<AVNGameMode>();
	if (GM)
	{
		GM->OnToggleAuto();
	}
}
