#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Data/VNStoryData.h"
#include "Data/VNSaveData.h"
#include "VNBranchViewWidget.generated.h"

class UVNStoryEngine;

UCLASS()
class VISUALNOVEL_API UVNBranchViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void ShowBranchView(UVNStoryEngine* InStoryEngine, const FVNSaveData& SaveData);

private:
	void BuildWidgetTree();

	UPROPERTY()
	UVerticalBox* BranchContainer;

	UPROPERTY()
	UButton* CloseButton;

	bool bWidgetTreeBuilt = false;

	UFUNCTION()
	void OnCloseClicked();
};
