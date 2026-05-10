#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Data/VNSaveData.h"
#include "VNStoryTextViewWidget.generated.h"

class UVNStoryEngine;

UCLASS()
class VISUALNOVEL_API UVNStoryTextViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|UI")
	void ShowStoryText(UVNStoryEngine* InStoryEngine, const FVNSaveData& SaveData);

private:
	void BuildWidgetTree();

	UPROPERTY()
	UTextBlock* StoryTextBlock;

	UPROPERTY()
	UButton* CloseButton;

	bool bWidgetTreeBuilt = false;

	UFUNCTION()
	void OnCloseClicked();
};
