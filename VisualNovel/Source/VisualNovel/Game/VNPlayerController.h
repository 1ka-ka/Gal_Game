#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VNPlayerController.generated.h"

UCLASS()
class VISUALNOVEL_API AVNPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVNPlayerController();

	virtual void SetupInputComponent() override;

	UFUNCTION()
	void OnAdvanceDialogue();

	UFUNCTION()
	void OnTogglePause();

	UFUNCTION()
	void OnQuickSave();

	UFUNCTION()
	void OnToggleAuto();
};
