#include "Core/VNSaveManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "HAL/PlatformFileManager.h"

UVNSaveManager::UVNSaveManager()
{
}

FString UVNSaveManager::GetSaveDirectory() const
{
	TArray<FString> CandidateDirs;
	CandidateDirs.Add(FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames")));
	CandidateDirs.Add(TEXT("E:/VNBuild/VisualNovel/Saved/SaveGames"));

	for (const FString& Dir : CandidateDirs)
	{
		FString Normalized = Dir;
		FPaths::NormalizeDirectoryName(Normalized);
		
		if (IFileManager::Get().DirectoryExists(*Normalized))
		{
			return Normalized;
		}
	}

	FString SaveDir = CandidateDirs[0];
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectoryTree(*SaveDir);
	
	FString AltDir = CandidateDirs[1];
	PlatformFile.CreateDirectoryTree(*AltDir);
	
	return SaveDir;
}

FString UVNSaveManager::GetSaveFilePath(int32 SlotIndex) const
{
	return FPaths::Combine(GetSaveDirectory(), FString::Printf(TEXT("save_slot_%d.json"), SlotIndex));
}

FString UVNSaveManager::GetSettingsFilePath() const
{
	return FPaths::Combine(GetSaveDirectory(), TEXT("settings.json"));
}

bool UVNSaveManager::SaveGame(int32 SlotIndex, const FVNSaveData& SaveData)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots)
	{
		UE_LOG(LogTemp, Error, TEXT("VNSaveManager: Invalid slot index: %d"), SlotIndex);
		OnSaveComplete.Broadcast(SlotIndex, false);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetStringField(TEXT("SaveSlot"), FString::FromInt(SaveData.SaveSlot));
	JsonObject->SetStringField(TEXT("Timestamp"), SaveData.Timestamp);
	JsonObject->SetStringField(TEXT("StoryId"), SaveData.StoryId);
	JsonObject->SetStringField(TEXT("CurrentNodeId"), SaveData.CurrentNodeId);
	JsonObject->SetStringField(TEXT("LastChoiceNodeId"), SaveData.LastChoiceNodeId);
	JsonObject->SetStringField(TEXT("ResumeNodeId"), SaveData.ResumeNodeId);
	JsonObject->SetBoolField(TEXT("bIsFromSave"), SaveData.bIsFromSave);
	JsonObject->SetNumberField(TEXT("OriginalSaveSlot"), SaveData.OriginalSaveSlot);

	TArray<TSharedPtr<FJsonValue>> ChoiceHistoryArray;
	for (const FVNChoiceRecord& Record : SaveData.ChoiceHistory)
	{
		TSharedPtr<FJsonObject> RecordObj = MakeShareable(new FJsonObject);
		RecordObj->SetStringField(TEXT("ChoiceNodeId"), Record.ChoiceNodeId);
		RecordObj->SetStringField(TEXT("SelectedChoiceId"), Record.SelectedChoiceId);
		RecordObj->SetStringField(TEXT("SelectedChoiceText"), Record.SelectedChoiceText);
		RecordObj->SetStringField(TEXT("Timestamp"), Record.Timestamp);
		ChoiceHistoryArray.Add(MakeShareable(new FJsonValueObject(RecordObj)));
	}
	JsonObject->SetArrayField(TEXT("ChoiceHistory"), ChoiceHistoryArray);

	TArray<TSharedPtr<FJsonValue>> VisitedNodesArray;
	for (const FString& NodeId : SaveData.VisitedNodeIds)
	{
		VisitedNodesArray.Add(MakeShareable(new FJsonValueString(NodeId)));
	}
	JsonObject->SetArrayField(TEXT("VisitedNodeIds"), VisitedNodesArray);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	FString FilePath = GetSaveFilePath(SlotIndex);
	bool bSuccess = FFileHelper::SaveStringToFile(JsonString, *FilePath);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("VNSaveManager: Game saved to slot %d"), SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("VNSaveManager: Failed to save game to slot %d"), SlotIndex);
	}

	OnSaveComplete.Broadcast(SlotIndex, bSuccess);
	return bSuccess;
}

bool UVNSaveManager::LoadGame(int32 SlotIndex, FVNSaveData& OutSaveData)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots) return false;

	FString FilePath = GetSaveFilePath(SlotIndex);
	if (!FPaths::FileExists(FilePath)) return false;

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) return false;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid()) return false;

	OutSaveData.SaveSlot = SlotIndex;
	OutSaveData.Timestamp = JsonObject->GetStringField(TEXT("Timestamp"));
	OutSaveData.StoryId = JsonObject->GetStringField(TEXT("StoryId"));
	OutSaveData.CurrentNodeId = JsonObject->GetStringField(TEXT("CurrentNodeId"));
	OutSaveData.LastChoiceNodeId = JsonObject->GetStringField(TEXT("LastChoiceNodeId"));

	if (JsonObject->HasField(TEXT("ResumeNodeId")))
		OutSaveData.ResumeNodeId = JsonObject->GetStringField(TEXT("ResumeNodeId"));

	if (JsonObject->HasField(TEXT("bIsFromSave")))
		OutSaveData.bIsFromSave = JsonObject->GetBoolField(TEXT("bIsFromSave"));

	if (JsonObject->HasField(TEXT("OriginalSaveSlot")))
		OutSaveData.OriginalSaveSlot = JsonObject->GetIntegerField(TEXT("OriginalSaveSlot"));

	const TArray<TSharedPtr<FJsonValue>>& ChoiceHistoryArray = JsonObject->GetArrayField(TEXT("ChoiceHistory"));
	for (const auto& ChoiceVal : ChoiceHistoryArray)
	{
		const TSharedPtr<FJsonObject>& RecordObj = ChoiceVal->AsObject();
		FVNChoiceRecord Record;
		Record.ChoiceNodeId = RecordObj->GetStringField(TEXT("ChoiceNodeId"));
		Record.SelectedChoiceId = RecordObj->GetStringField(TEXT("SelectedChoiceId"));
		Record.SelectedChoiceText = RecordObj->GetStringField(TEXT("SelectedChoiceText"));
		Record.Timestamp = RecordObj->GetStringField(TEXT("Timestamp"));
		OutSaveData.ChoiceHistory.Add(Record);
	}

	const TArray<TSharedPtr<FJsonValue>>& VisitedNodesArray = JsonObject->GetArrayField(TEXT("VisitedNodeIds"));
	for (const auto& NodeVal : VisitedNodesArray)
	{
		OutSaveData.VisitedNodeIds.Add(NodeVal->AsString());
	}

	UE_LOG(LogTemp, Log, TEXT("VNSaveManager: Game loaded from slot %d"), SlotIndex);
	return true;
}

bool UVNSaveManager::DeleteSave(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots) return false;

	FString FilePath = GetSaveFilePath(SlotIndex);
	if (!FPaths::FileExists(FilePath)) return false;

	bool bSuccess = IFileManager::Get().Delete(*FilePath);
	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("VNSaveManager: Save deleted from slot %d"), SlotIndex);
	}
	return bSuccess;
}

bool UVNSaveManager::HasSave(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= MaxSaveSlots) return false;
	return FPaths::FileExists(GetSaveFilePath(SlotIndex));
}

TArray<FVNSaveSlotInfo> UVNSaveManager::GetAllSaveSlots() const
{
	TArray<FVNSaveSlotInfo> Slots;
	for (int32 i = 0; i < MaxSaveSlots; i++)
	{
		Slots.Add(GetSaveSlotInfo(i));
	}
	return Slots;
}

FVNSaveSlotInfo UVNSaveManager::GetSaveSlotInfo(int32 SlotIndex) const
{
	FVNSaveSlotInfo Info;
	Info.SlotIndex = SlotIndex;
	Info.bHasSave = HasSave(SlotIndex);

	if (Info.bHasSave)
	{
		FVNSaveData SaveData;
		const_cast<UVNSaveManager*>(this)->LoadGame(SlotIndex, SaveData);
		Info.Timestamp = SaveData.Timestamp;
		Info.ChoiceCount = SaveData.ChoiceHistory.Num();
		if (SaveData.ChoiceHistory.Num() > 0)
		{
			Info.LastChoiceText = SaveData.ChoiceHistory.Last().SelectedChoiceText;
		}
	}

	return Info;
}

void UVNSaveManager::SaveSettings(const FVNGameSettings& Settings)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	JsonObject->SetNumberField(TEXT("TextSpeed"), Settings.TextSpeed);
	JsonObject->SetNumberField(TEXT("AutoAdvanceDelay"), Settings.AutoAdvanceDelay);
	JsonObject->SetNumberField(TEXT("BGMVolume"), Settings.BGMVolume);
	JsonObject->SetNumberField(TEXT("SFXVolume"), Settings.SFXVolume);
	JsonObject->SetNumberField(TEXT("VoiceVolume"), Settings.VoiceVolume);
	JsonObject->SetBoolField(TEXT("bFullscreen"), Settings.bFullscreen);
	JsonObject->SetStringField(TEXT("Language"), Settings.Language);

	FString JsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

	FFileHelper::SaveStringToFile(JsonString, *GetSettingsFilePath());
}

FVNGameSettings UVNSaveManager::LoadSettings()
{
	FVNGameSettings Settings;

	FString FilePath = GetSettingsFilePath();
	if (!FPaths::FileExists(FilePath)) return Settings;

	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath)) return Settings;

	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid()) return Settings;

	if (JsonObject->HasField(TEXT("TextSpeed")))
		Settings.TextSpeed = JsonObject->GetNumberField(TEXT("TextSpeed"));
	if (JsonObject->HasField(TEXT("AutoAdvanceDelay")))
		Settings.AutoAdvanceDelay = JsonObject->GetNumberField(TEXT("AutoAdvanceDelay"));
	if (JsonObject->HasField(TEXT("BGMVolume")))
		Settings.BGMVolume = JsonObject->GetNumberField(TEXT("BGMVolume"));
	if (JsonObject->HasField(TEXT("SFXVolume")))
		Settings.SFXVolume = JsonObject->GetNumberField(TEXT("SFXVolume"));
	if (JsonObject->HasField(TEXT("VoiceVolume")))
		Settings.VoiceVolume = JsonObject->GetNumberField(TEXT("VoiceVolume"));
	if (JsonObject->HasField(TEXT("bFullscreen")))
		Settings.bFullscreen = JsonObject->GetBoolField(TEXT("bFullscreen"));
	if (JsonObject->HasField(TEXT("Language")))
		Settings.Language = JsonObject->GetStringField(TEXT("Language"));

	return Settings;
}
