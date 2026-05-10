#pragma once

#include "CoreMinimal.h"
#include "VNSaveData.generated.h"

USTRUCT(BlueprintType)
struct FVNChoiceRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString ChoiceNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString SelectedChoiceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString SelectedChoiceText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Timestamp;
};

USTRUCT(BlueprintType)
struct FVNSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 SaveSlot = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString StoryId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString CurrentNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString LastChoiceNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString ResumeNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TArray<FVNChoiceRecord> ChoiceHistory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TArray<FString> VisitedNodeIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool bIsFromSave = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 OriginalSaveSlot = -1;

	bool IsValid() const
	{
		return SaveSlot >= 0 && !StoryId.IsEmpty() && !CurrentNodeId.IsEmpty();
	}

	bool HasChoices() const
	{
		return ChoiceHistory.Num() > 0;
	}

	FString GetDisplayText() const
	{
		if (!IsValid()) return TEXT("空存档");
		FString Display = FString::Printf(TEXT("存档 %d - %s"), SaveSlot, *Timestamp);
		return Display;
	}

	TArray<FString> GetVisitedChoiceNodeIds() const
	{
		TArray<FString> ChoiceNodeIds;
		for (const auto& Record : ChoiceHistory)
		{
			ChoiceNodeIds.Add(Record.ChoiceNodeId);
		}
		return ChoiceNodeIds;
	}

	FString GetStoryText() const
	{
		FString Result;
		for (const auto& NodeId : VisitedNodeIds)
		{
			Result += NodeId + TEXT("\n");
		}
		return Result;
	}
};

USTRUCT(BlueprintType)
struct FVNSaveSlotInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 SlotIndex = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool bHasSave = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString StoryTitle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString LastChoiceText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	int32 ChoiceCount = 0;
};

USTRUCT(BlueprintType)
struct FVNGameSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	float TextSpeed = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	float AutoAdvanceDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	float BGMVolume = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	float VoiceVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool bFullscreen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Language = TEXT("zh-Hans");
};
