#include "Core/VNStoryEngine.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonReader.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "HAL/PlatformFileManager.h"

UVNStoryEngine::UVNStoryEngine()
{
}

bool UVNStoryEngine::LoadStory(const FString& StoryId)
{
	TArray<FString> CandidatePaths;
	
	FString ProjectDir = FPaths::ProjectDir();
	CandidatePaths.Add(FPaths::Combine(ProjectDir, TEXT("StoryData"), StoryId + TEXT(".json")));
	CandidatePaths.Add(FPaths::Combine(TEXT("E:/VNBuild/VisualNovel/StoryData"), StoryId + TEXT(".json")));
	CandidatePaths.Add(FPaths::Combine(TEXT("E:/trea/视觉系小说/VisualNovel/StoryData"), StoryId + TEXT(".json")));

	FString StoryPath;
	for (const FString& Path : CandidatePaths)
	{
		UE_LOG(LogTemp, Log, TEXT("VNStoryEngine: Checking story path: %s"), *Path);
		if (IFileManager::Get().FileExists(*Path))
		{
			StoryPath = Path;
			UE_LOG(LogTemp, Log, TEXT("VNStoryEngine: Found story file at: %s"), *Path);
			break;
		}
	}

	if (StoryPath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("VNStoryEngine: Story file not found in any candidate path for: %s"), *StoryId);
		return false;
	}

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *StoryPath))
	{
		UE_LOG(LogTemp, Error, TEXT("VNStoryEngine: Failed to read story file: %s"), *StoryPath);
		return false;
	}

	if (JsonString.Len() > 0 && JsonString[0] == 0xFEFF)
	{
		JsonString.RemoveAt(0);
		UE_LOG(LogTemp, Warning, TEXT("VNStoryEngine: Stripped BOM from story file"));
	}

	JsonString.TrimStartAndEnd();

	UE_LOG(LogTemp, Log, TEXT("VNStoryEngine: Story file loaded, size=%d chars"), JsonString.Len());

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		FString ErrorMsg = Reader->GetErrorMessage();
		UE_LOG(LogTemp, Error, TEXT("VNStoryEngine: Failed to parse story JSON: %s - Error: %s"), *StoryId, *ErrorMsg);
		return false;
	}

	StoryData.StoryId = JsonObject->GetStringField(TEXT("StoryId"));
	StoryData.Title = JsonObject->GetStringField(TEXT("Title"));
	StoryData.FirstNodeId = JsonObject->GetStringField(TEXT("FirstNodeId"));

	const TSharedPtr<FJsonObject>& NodesObj = JsonObject->GetObjectField(TEXT("Nodes"));
	for (const auto& Pair : NodesObj->Values)
	{
		FVNStoryNode Node;
		Node.NodeId = Pair.Key;

		const TSharedPtr<FJsonObject>& NodeObj = Pair.Value->AsObject();

		FString TypeStr = NodeObj->GetStringField(TEXT("Type"));
		if (TypeStr == TEXT("Dialogue"))
			Node.Type = EVNNodeType::Dialogue;
		else if (TypeStr == TEXT("Choice"))
			Node.Type = EVNNodeType::Choice;
		else if (TypeStr == TEXT("SceneTransition"))
			Node.Type = EVNNodeType::SceneTransition;

		if (NodeObj->HasField(TEXT("Speaker")))
			Node.Speaker = NodeObj->GetStringField(TEXT("Speaker"));

		if (NodeObj->HasField(TEXT("Text")))
			Node.Text = NodeObj->GetStringField(TEXT("Text"));

		if (NodeObj->HasField(TEXT("Background")))
			Node.BackgroundId = NodeObj->GetStringField(TEXT("Background"));

		if (NodeObj->HasField(TEXT("Next")))
			Node.NextNodeId = NodeObj->GetStringField(TEXT("Next"));

		if (NodeObj->HasField(TEXT("Music")))
			Node.MusicId = NodeObj->GetStringField(TEXT("Music"));

		if (NodeObj->HasField(TEXT("Characters")))
		{
			const TArray<TSharedPtr<FJsonValue>>& CharsArr = NodeObj->GetArrayField(TEXT("Characters"));
			for (const auto& CharVal : CharsArr)
			{
				const TSharedPtr<FJsonObject>& CharObj = CharVal->AsObject();
				FVNCharacterDisplay CharDisplay;
				CharDisplay.CharacterId = CharObj->GetStringField(TEXT("Id"));
				CharDisplay.bVisible = true;

				if (CharObj->HasField(TEXT("Position")))
				{
					FString PosStr = CharObj->GetStringField(TEXT("Position"));
					if (PosStr == TEXT("Left")) CharDisplay.Position = EVNCharacterPosition::Left;
					else if (PosStr == TEXT("Center")) CharDisplay.Position = EVNCharacterPosition::Center;
					else if (PosStr == TEXT("Right")) CharDisplay.Position = EVNCharacterPosition::Right;
					else if (PosStr == TEXT("CenterLeft")) CharDisplay.Position = EVNCharacterPosition::CenterLeft;
					else if (PosStr == TEXT("CenterRight")) CharDisplay.Position = EVNCharacterPosition::CenterRight;
				}

				if (CharObj->HasField(TEXT("Expression")))
					CharDisplay.Expression = CharObj->GetStringField(TEXT("Expression"));

				Node.Characters.Add(CharDisplay);
			}
		}

		if (NodeObj->HasField(TEXT("Choices")))
		{
			const TArray<TSharedPtr<FJsonValue>>& ChoicesArr = NodeObj->GetArrayField(TEXT("Choices"));
			for (const auto& ChoiceVal : ChoicesArr)
			{
				const TSharedPtr<FJsonObject>& ChoiceObj = ChoiceVal->AsObject();
				FVNChoice Choice;
				Choice.ChoiceId = ChoiceObj->GetStringField(TEXT("Id"));
				Choice.Text = ChoiceObj->GetStringField(TEXT("Text"));
				Choice.NextNodeId = ChoiceObj->GetStringField(TEXT("Next"));
				Node.Choices.Add(Choice);
			}
		}

		StoryData.Nodes.Add(Pair.Key, Node);
	}

	CurrentStoryId = StoryId;
	bStoryLoaded = true;
	bStoryEnd = false;

	UE_LOG(LogTemp, Log, TEXT("VNStoryEngine: Story loaded: %s (%d nodes)"), *StoryData.Title, StoryData.Nodes.Num());
	return true;
}

void UVNStoryEngine::StartNewGame()
{
	if (!bStoryLoaded) return;

	CurrentNodeId = StoryData.FirstNodeId;
	LastChoiceNodeId = TEXT("");
	ResumeNodeId = TEXT("");
	VisitedNodeIds.Empty();
	ChoiceHistory.Empty();
	bStoryEnd = false;

	ProcessCurrentNode();
}

void UVNStoryEngine::ResumeFromSave(const FVNSaveData& SaveData)
{
	if (!bStoryLoaded) return;

	CurrentNodeId = SaveData.CurrentNodeId;
	LastChoiceNodeId = SaveData.LastChoiceNodeId;
	ResumeNodeId = SaveData.ResumeNodeId;
	VisitedNodeIds = SaveData.VisitedNodeIds;
	ChoiceHistory = SaveData.ChoiceHistory;
	bStoryEnd = false;

	ProcessCurrentNode();
}

void UVNStoryEngine::Reset()
{
	CurrentNodeId = TEXT("");
	LastChoiceNodeId = TEXT("");
	ResumeNodeId = TEXT("");
	VisitedNodeIds.Empty();
	ChoiceHistory.Empty();
	bStoryLoaded = false;
	bStoryEnd = false;
	StoryData = FVNStoryData();
	CurrentStoryId = TEXT("");

	UE_LOG(LogTemp, Log, TEXT("VNStoryEngine: Reset to initial state"));
}

void UVNStoryEngine::AdvanceStory()
{
	if (!bStoryLoaded || bStoryEnd) return;

	const FVNStoryNode* CurrentNode = StoryData.GetNode(CurrentNodeId);
	if (!CurrentNode) return;

	if (CurrentNode->IsDialogue() && CurrentNode->HasNext())
	{
		CurrentNodeId = CurrentNode->NextNodeId;
		ProcessCurrentNode();
	}
}

void UVNStoryEngine::MakeChoice(const FString& ChoiceId)
{
	if (!bStoryLoaded || bStoryEnd) return;

	const FVNStoryNode* CurrentNode = StoryData.GetNode(CurrentNodeId);
	if (!CurrentNode || !CurrentNode->IsChoice()) return;

	for (const FVNChoice& Choice : CurrentNode->Choices)
	{
		if (Choice.ChoiceId == ChoiceId)
		{
			FVNChoiceRecord Record;
			Record.ChoiceNodeId = CurrentNodeId;
			Record.SelectedChoiceId = ChoiceId;
			Record.SelectedChoiceText = Choice.Text;
			Record.Timestamp = FDateTime::Now().ToString();
			ChoiceHistory.Add(Record);

			LastChoiceNodeId = CurrentNodeId;
			ResumeNodeId = Choice.NextNodeId;

			CurrentNodeId = Choice.NextNodeId;
			ProcessCurrentNode();
			return;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("VNStoryEngine: Choice not found: %s"), *ChoiceId);
}

void UVNStoryEngine::JumpToNode(const FString& NodeId)
{
	if (!bStoryLoaded) return;

	if (!StoryData.Nodes.Contains(NodeId))
	{
		UE_LOG(LogTemp, Warning, TEXT("VNStoryEngine: Node not found: %s"), *NodeId);
		return;
	}

	CurrentNodeId = NodeId;
	ProcessCurrentNode();
}

FVNSaveData UVNStoryEngine::CreateSaveData(int32 SaveSlot) const
{
	FVNSaveData SaveData;
	SaveData.SaveSlot = SaveSlot;
	SaveData.Timestamp = FDateTime::Now().ToString();
	SaveData.StoryId = CurrentStoryId;
	SaveData.CurrentNodeId = CurrentNodeId;
	SaveData.LastChoiceNodeId = LastChoiceNodeId;
	SaveData.ResumeNodeId = ResumeNodeId;
	SaveData.ChoiceHistory = ChoiceHistory;
	SaveData.VisitedNodeIds = VisitedNodeIds;
	return SaveData;
}

const FVNStoryNode* UVNStoryEngine::GetCurrentNode() const
{
	if (!bStoryLoaded) return nullptr;
	return StoryData.GetNode(CurrentNodeId);
}

void UVNStoryEngine::GetCurrentNodeData(FVNStoryNode& OutNode) const
{
	const FVNStoryNode* Node = GetCurrentNode();
	if (Node)
	{
		OutNode = *Node;
	}
}

bool UVNStoryEngine::IsAtChoice() const
{
	const FVNStoryNode* Node = GetCurrentNode();
	return Node && Node->IsChoice();
}

void UVNStoryEngine::ProcessCurrentNode()
{
	UE_LOG(LogTemp, Log, TEXT("VNStoryEngine: ProcessCurrentNode - CurrentNodeId=%s"), *CurrentNodeId);
	
	const FVNStoryNode* Node = StoryData.GetNode(CurrentNodeId);
	if (!Node)
	{
		UE_LOG(LogTemp, Error, TEXT("VNStoryEngine: Node not found: %s"), *CurrentNodeId);
		bStoryEnd = true;
		OnStoryEnd.Broadcast();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("VNStoryEngine: Processing node %s, Type=%d, Speaker=%s, Text=%s"), 
		*CurrentNodeId, (int32)Node->Type, *Node->Speaker, *Node->Text.Left(50));

	RecordVisit(CurrentNodeId);
	OnNodeEntered.Broadcast(CurrentNodeId);

	if (!Node->BackgroundId.IsEmpty())
	{
		OnBackgroundChanged.Broadcast(Node->BackgroundId);
	}

	if (Node->Characters.Num() > 0)
	{
		OnCharactersChanged.Broadcast(Node->Characters);
	}

	if (Node->IsDialogue())
	{
		ProcessDialogueNode(*Node);
	}
	else if (Node->IsChoice())
	{
		ProcessChoiceNode(*Node);
	}

	if (!Node->HasNext() && !Node->HasChoices())
	{
		bStoryEnd = true;
		OnStoryEnd.Broadcast();
	}
}

void UVNStoryEngine::ProcessDialogueNode(const FVNStoryNode& Node)
{
	OnDialogueStarted.Broadcast(Node.Speaker, Node.Text);
}

void UVNStoryEngine::ProcessChoiceNode(const FVNStoryNode& Node)
{
	if (!Node.Text.IsEmpty())
	{
		OnDialogueStarted.Broadcast(Node.Speaker, Node.Text);
	}
	OnChoicePresented.Broadcast(Node.Choices);
}

void UVNStoryEngine::RecordVisit(const FString& NodeId)
{
	if (!VisitedNodeIds.Contains(NodeId))
	{
		VisitedNodeIds.Add(NodeId);
	}
}

FString UVNStoryEngine::GetStoryTextForNodes(const TArray<FString>& NodeIds) const
{
	FString Result;
	for (const FString& NodeId : NodeIds)
	{
		const FVNStoryNode* Node = StoryData.GetNode(NodeId);
		if (Node)
		{
			if (!Node->Speaker.IsEmpty())
			{
				Result += TEXT("【") + Node->Speaker + TEXT("】");
			}
			if (!Node->Text.IsEmpty())
			{
				Result += Node->Text;
			}
			Result += TEXT("\n\n");
		}
	}
	return Result;
}

TArray<FVNChoiceRecord> UVNStoryEngine::GetAllChoiceRecords() const
{
	return ChoiceHistory;
}
