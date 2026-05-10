#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/VNStoryData.h"
#include "Data/VNSaveData.h"
#include "VNStoryEngine.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FVNStoryEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVNDialogueEvent, FString, Speaker, FString, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVNChoiceEvent, const TArray<FVNChoice>&, Choices);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVNBackgroundEvent, FString, BackgroundId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVNCharacterEvent, const TArray<FVNCharacterDisplay>&, Characters);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVNNodeEvent, FString, NodeId);

UCLASS(BlueprintType)
class VISUALNOVEL_API UVNStoryEngine : public UObject
{
	GENERATED_BODY()

public:
	UVNStoryEngine();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	bool LoadStory(const FString& StoryId);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	void ResumeFromSave(const FVNSaveData& SaveData);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	void AdvanceStory();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	void MakeChoice(const FString& ChoiceId);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	void JumpToNode(const FString& NodeId);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	FVNSaveData CreateSaveData(int32 SaveSlot) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	FString GetCurrentNodeId() const { return CurrentNodeId; }

	const FVNStoryNode* GetCurrentNode() const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	void GetCurrentNodeData(FVNStoryNode& OutNode) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	FString GetStoryId() const { return CurrentStoryId; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	bool IsStoryLoaded() const { return bStoryLoaded; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	bool IsAtChoice() const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	bool IsStoryEnd() const { return bStoryEnd; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	TArray<FString> GetVisitedNodeIds() const { return VisitedNodeIds; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	TArray<FVNChoiceRecord> GetChoiceHistory() const { return ChoiceHistory; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	FString GetLastChoiceNodeId() const { return LastChoiceNodeId; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	FString GetResumeNodeId() const { return ResumeNodeId; }

	const FVNStoryData& GetStoryData() const { return StoryData; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	FString GetStoryTextForNodes(const TArray<FString>& NodeIds) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Story")
	TArray<FVNChoiceRecord> GetAllChoiceRecords() const;

	UPROPERTY(BlueprintAssignable, Category = "VisualNovel|Story")
	FVNDialogueEvent OnDialogueStarted;

	UPROPERTY(BlueprintAssignable, Category = "VisualNovel|Story")
	FVNChoiceEvent OnChoicePresented;

	UPROPERTY(BlueprintAssignable, Category = "VisualNovel|Story")
	FVNBackgroundEvent OnBackgroundChanged;

	UPROPERTY(BlueprintAssignable, Category = "VisualNovel|Story")
	FVNCharacterEvent OnCharactersChanged;

	UPROPERTY(BlueprintAssignable, Category = "VisualNovel|Story")
	FVNStoryEvent OnStoryEnd;

	UPROPERTY(BlueprintAssignable, Category = "VisualNovel|Story")
	FVNNodeEvent OnNodeEntered;

private:
	UPROPERTY()
	FVNStoryData StoryData;

	FString CurrentNodeId;
	FString CurrentStoryId;
	FString LastChoiceNodeId;
	FString ResumeNodeId;

	TArray<FString> VisitedNodeIds;
	TArray<FVNChoiceRecord> ChoiceHistory;

	bool bStoryLoaded = false;
	bool bStoryEnd = false;

	void ProcessCurrentNode();
	void ProcessDialogueNode(const FVNStoryNode& Node);
	void ProcessChoiceNode(const FVNStoryNode& Node);
	void RecordVisit(const FString& NodeId);
};
