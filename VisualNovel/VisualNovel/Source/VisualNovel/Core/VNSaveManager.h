#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Data/VNSaveData.h"
#include "VNSaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVNSaveEvent, int32, SlotIndex, bool, bSuccess);

UCLASS(BlueprintType)
class VISUALNOVEL_API UVNSaveManager : public UObject
{
	GENERATED_BODY()

public:
	UVNSaveManager();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	bool SaveGame(int32 SlotIndex, const FVNSaveData& SaveData);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	bool LoadGame(int32 SlotIndex, FVNSaveData& OutSaveData);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	bool DeleteSave(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	bool HasSave(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	TArray<FVNSaveSlotInfo> GetAllSaveSlots() const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	FVNSaveSlotInfo GetSaveSlotInfo(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	int32 GetMaxSaveSlots() const { return MaxSaveSlots; }

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	FString GetSaveFilePath(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	void SaveSettings(const FVNGameSettings& Settings);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Save")
	FVNGameSettings LoadSettings();

	UPROPERTY(BlueprintAssignable, Category = "VisualNovel|Save")
	FVNSaveEvent OnSaveComplete;

private:
	static constexpr int32 MaxSaveSlots = 10;

	FString GetSaveDirectory() const;
	FString GetSettingsFilePath() const;
};
