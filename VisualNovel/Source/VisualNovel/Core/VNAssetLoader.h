#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include "VNAssetLoader.generated.h"

UCLASS(BlueprintType)
class VISUALNOVEL_API UVNAssetLoader : public UObject
{
	GENERATED_BODY()

public:
	UVNAssetLoader();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	UTexture2D* LoadTextureFromFile(const FString& FilePath);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	UTexture2D* LoadBackground(const FString& BackgroundId);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	UTexture2D* LoadCharacterImage(const FString& CharacterId, const FString& Expression);

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	void PreloadBackgrounds();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	void PreloadCharacters();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	void ClearCache();

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	FString GetResourcesPath() const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	FString GetBackgroundsPath() const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	FString GetCharactersPath() const;

	UFUNCTION(BlueprintCallable, Category = "VisualNovel|Asset")
	FString GetAudioPath() const;

private:
	UPROPERTY()
	TMap<FString, UTexture2D*> TextureCache;

	UTexture2D* CreateTextureFromRawData(const FString& FilePath);
	FString FindImageFile(const FString& Directory, const FString& BaseName);
	TArray<FString> SupportedImageExtensions;
};
