#pragma once

#include "CoreMinimal.h"
#include "VNStoryData.generated.h"

UENUM(BlueprintType)
enum class EVNNodeType : uint8
{
	Dialogue UMETA(DisplayName = "Dialogue"),
	Choice UMETA(DisplayName = "Choice"),
	SceneTransition UMETA(DisplayName = "SceneTransition")
};

UENUM(BlueprintType)
enum class EVNCharacterPosition : uint8
{
	Left UMETA(DisplayName = "Left"),
	Center UMETA(DisplayName = "Center"),
	Right UMETA(DisplayName = "Right"),
	CenterLeft UMETA(DisplayName = "CenterLeft"),
	CenterRight UMETA(DisplayName = "CenterRight")
};

USTRUCT(BlueprintType)
struct FVNCharacterDisplay
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString CharacterId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	EVNCharacterPosition Position = EVNCharacterPosition::Center;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Expression = TEXT("normal");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool bVisible = true;
};

USTRUCT(BlueprintType)
struct FVNChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString ChoiceId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString NextNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString ConditionId;
};

USTRUCT(BlueprintType)
struct FVNStoryNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString NodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	EVNNodeType Type = EVNNodeType::Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Speaker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Text;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString BackgroundId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TArray<FVNCharacterDisplay> Characters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString NextNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TArray<FVNChoice> Choices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString MusicId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString SoundEffectId;

	bool IsDialogue() const { return Type == EVNNodeType::Dialogue; }
	bool IsChoice() const { return Type == EVNNodeType::Choice; }
	bool HasNext() const { return !NextNodeId.IsEmpty(); }
	bool HasChoices() const { return Choices.Num() > 0; }
};

USTRUCT(BlueprintType)
struct FVNStoryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString StoryId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString FirstNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TMap<FString, FVNStoryNode> Nodes;

	const FVNStoryNode* GetNode(const FString& NodeId) const
	{
		const FVNStoryNode* Node = Nodes.Find(NodeId);
		return Node;
	}

	bool IsValid() const
	{
		return !StoryId.IsEmpty() && !FirstNodeId.IsEmpty() && Nodes.Num() > 0;
	}

	TArray<FString> GetAllChoiceNodeIds() const
	{
		TArray<FString> ChoiceNodeIds;
		for (const auto& Pair : Nodes)
		{
			if (Pair.Value.IsChoice())
			{
				ChoiceNodeIds.Add(Pair.Key);
			}
		}
		return ChoiceNodeIds;
	}
};

USTRUCT(BlueprintType)
struct FVNCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString CharacterId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	TMap<FString, FString> ExpressionImages;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString DefaultExpression = TEXT("normal");
};

USTRUCT(BlueprintType)
struct FVNBackgroundInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString BackgroundId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString ImagePath;
};

USTRUCT(BlueprintType)
struct FVNMusicInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString MusicId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	FString FilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VisualNovel")
	bool bLoop = true;
};
