#include "Core/VNAssetLoader.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Engine/Texture2D.h"
#include "RHI.h"
#include "RenderingThread.h"
#include "HAL/PlatformFileManager.h"

UVNAssetLoader::UVNAssetLoader()
{
	SupportedImageExtensions = { TEXT(".png"), TEXT(".jpg"), TEXT(".jpeg"), TEXT(".bmp"), TEXT(".hdr") };
}

FString UVNAssetLoader::GetResourcesPath() const
{
	TArray<FString> CandidatePaths;
	
	FString ProjectDir = FPaths::ProjectDir();
	CandidatePaths.Add(FPaths::Combine(ProjectDir, TEXT("Resources")));
	
	CandidatePaths.Add(TEXT("E:/VNBuild/VisualNovel/Resources"));
	
	CandidatePaths.Add(TEXT("E:/trea/视觉系小说/VisualNovel/Resources"));

	for (const FString& Path : CandidatePaths)
	{
		FString Normalized = Path;
		FPaths::NormalizeDirectoryName(Normalized);
		
		if (IFileManager::Get().DirectoryExists(*Normalized))
		{
			UE_LOG(LogTemp, Log, TEXT("VNAssetLoader: Using Resources at: %s"), *Normalized);
			return Normalized;
		}
	}

	UE_LOG(LogTemp, Error, TEXT("VNAssetLoader: No Resources directory found in any candidate path!"));
	return FPaths::Combine(ProjectDir, TEXT("Resources"));
}

FString UVNAssetLoader::GetBackgroundsPath() const
{
	return FPaths::Combine(GetResourcesPath(), TEXT("Backgrounds"));
}

FString UVNAssetLoader::GetCharactersPath() const
{
	return FPaths::Combine(GetResourcesPath(), TEXT("Characters"));
}

FString UVNAssetLoader::GetAudioPath() const
{
	return FPaths::Combine(GetResourcesPath(), TEXT("Audio"));
}

FString UVNAssetLoader::FindImageFile(const FString& Directory, const FString& BaseName)
{
	for (const FString& Ext : SupportedImageExtensions)
	{
		FString FullPath = FPaths::Combine(Directory, BaseName + Ext);
		if (IFileManager::Get().FileExists(*FullPath))
		{
			return FullPath;
		}
	}
	return FString();
}

UTexture2D* UVNAssetLoader::LoadTextureFromFile(const FString& FilePath)
{
	if (FilePath.IsEmpty()) return nullptr;

	if (UTexture2D* const* Cached = TextureCache.Find(FilePath))
	{
		return *Cached;
	}

	UTexture2D* Texture = CreateTextureFromRawData(FilePath);
	if (Texture)
	{
		TextureCache.Add(FilePath, Texture);
	}
	return Texture;
}

UTexture2D* UVNAssetLoader::LoadBackground(const FString& BackgroundId)
{
	if (BackgroundId.IsEmpty()) return nullptr;

	FString CacheKey = TEXT("BG_") + BackgroundId;
	if (UTexture2D* const* Cached = TextureCache.Find(CacheKey))
	{
		return *Cached;
	}

	FString FilePath = FindImageFile(GetBackgroundsPath(), BackgroundId);
	if (FilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNAssetLoader: Background not found: %s (searched in %s)"), *BackgroundId, *GetBackgroundsPath());
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("VNAssetLoader: Loading background: %s from %s"), *BackgroundId, *FilePath);
	UTexture2D* Texture = LoadTextureFromFile(FilePath);
	if (Texture)
	{
		TextureCache.Add(CacheKey, Texture);
	}
	return Texture;
}

UTexture2D* UVNAssetLoader::LoadCharacterImage(const FString& CharacterId, const FString& Expression)
{
	if (CharacterId.IsEmpty()) return nullptr;

	FString CacheKey = TEXT("CHAR_") + CharacterId + TEXT("_") + Expression;
	if (UTexture2D* const* Cached = TextureCache.Find(CacheKey))
	{
		return *Cached;
	}

	FString FileName = CharacterId + TEXT("_") + Expression;
	FString FilePath = FindImageFile(GetCharactersPath(), FileName);
	if (FilePath.IsEmpty())
	{
		FilePath = FindImageFile(GetCharactersPath(), CharacterId);
	}

	if (FilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("VNAssetLoader: Character image not found: %s (%s) (searched in %s)"), *CharacterId, *Expression, *GetCharactersPath());
		return nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("VNAssetLoader: Loading character: %s_%s from %s"), *CharacterId, *Expression, *FilePath);
	UTexture2D* Texture = LoadTextureFromFile(FilePath);
	if (Texture)
	{
		TextureCache.Add(CacheKey, Texture);
	}
	return Texture;
}

void UVNAssetLoader::PreloadBackgrounds()
{
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *GetBackgroundsPath(), TEXT("*.png"), true, false);
	IFileManager::Get().FindFilesRecursive(Files, *GetBackgroundsPath(), TEXT("*.jpg"), true, false);

	for (const FString& File : Files)
	{
		FString BaseName = FPaths::GetBaseFilename(File);
		LoadBackground(BaseName);
	}
}

void UVNAssetLoader::PreloadCharacters()
{
	TArray<FString> Files;
	IFileManager::Get().FindFilesRecursive(Files, *GetCharactersPath(), TEXT("*.png"), true, false);

	for (const FString& File : Files)
	{
		FString BaseName = FPaths::GetBaseFilename(File);
		LoadTextureFromFile(File);
	}
}

void UVNAssetLoader::ClearCache()
{
	TextureCache.Empty();
}

UTexture2D* UVNAssetLoader::CreateTextureFromRawData(const FString& FilePath)
{
	TArray<uint8> FileData;
	if (!FFileHelper::LoadFileToArray(FileData, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("VNAssetLoader: Failed to load file: %s"), *FilePath);
		return nullptr;
	}

	if (FileData.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("VNAssetLoader: File is empty: %s"), *FilePath);
		return nullptr;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));

	EImageFormat Format = ImageWrapperModule.DetectImageFormat(FileData.GetData(), FileData.Num());
	if (Format == EImageFormat::Invalid)
	{
		UE_LOG(LogTemp, Error, TEXT("VNAssetLoader: Unrecognized image format: %s"), *FilePath);
		return nullptr;
	}

	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(Format);
	if (!ImageWrapper.IsValid() || !ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
	{
		UE_LOG(LogTemp, Error, TEXT("VNAssetLoader: Failed to decompress image: %s"), *FilePath);
		return nullptr;
	}

	TArray<uint8> RawData;
	if (!ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
	{
		UE_LOG(LogTemp, Error, TEXT("VNAssetLoader: Failed to get raw image data: %s"), *FilePath);
		return nullptr;
	}

	const int32 Width = ImageWrapper->GetWidth();
	const int32 Height = ImageWrapper->GetHeight();

	if (Width <= 0 || Height <= 0 || RawData.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("VNAssetLoader: Invalid image dimensions: %s"), *FilePath);
		return nullptr;
	}

	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, EPixelFormat::PF_B8G8R8A8);
	if (!Texture) return nullptr;

	Texture->SRGB = true;
	Texture->UpdateResource();

	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, RawData.GetData(), RawData.Num());
	Mip.BulkData.Unlock();

	Texture->UpdateResource();

#if WITH_EDITORONLY_DATA
	Texture->Source.Init(Width, Height, 1, 1, ETextureSourceFormat::TSF_BGRA8, RawData.GetData());
#endif

	return Texture;
}
