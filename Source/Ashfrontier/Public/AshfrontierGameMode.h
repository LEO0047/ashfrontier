#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AshfrontierGameMode.generated.h"

class AAshfrontierCharacter;
class ACameraActor;
class AAshfrontierWorldBlockoutDirector;

USTRUCT()
struct FAshfrontierArtScreenshotView
{
    GENERATED_BODY()

    UPROPERTY()
    FString FileStem;

    UPROPERTY()
    FString LabelZh;

    UPROPERTY()
    FVector Location = FVector::ZeroVector;

    UPROPERTY()
    FVector Target = FVector::ZeroVector;

    UPROPERTY()
    bool bShowHud = false;
};

UCLASS()
class ASHFRONTIER_API AAshfrontierGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAshfrontierGameMode();

protected:
    virtual void BeginPlay() override;

private:
    void SpawnPrototypeWorld();
    void SpawnGate05Hostile();
    void SpawnGate06Characters();
    void SpawnGate08Guards();
    void StartArtScreenshotCaptureIfRequested();
    void CaptureNextArtScreenshot();
    void FinishArtScreenshotCapture();

    UPROPERTY(Transient)
    TObjectPtr<AAshfrontierWorldBlockoutDirector> WorldBlockoutDirector;

    UPROPERTY(Transient)
    TObjectPtr<AAshfrontierCharacter> Gate05Hostile;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AAshfrontierCharacter>> Gate06Characters;

    UPROPERTY(Transient)
    TArray<TObjectPtr<AAshfrontierCharacter>> Gate08Guards;

    UPROPERTY(Transient)
    TObjectPtr<ACameraActor> ArtCaptureCamera;

    UPROPERTY(Transient)
    TArray<FAshfrontierArtScreenshotView> ArtScreenshotViews;

    int32 ArtScreenshotIndex = 0;
};
