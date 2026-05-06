#include "AshfrontierGameMode.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierHUD.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierWorldBlockoutDirector.h"
#include "Camera/CameraActor.h"
#include "Engine/Engine.h"
#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Paths.h"
#include "Misc/Parse.h"
#include "TimerManager.h"

AAshfrontierGameMode::AAshfrontierGameMode()
{
    DefaultPawnClass = AAshfrontierCharacter::StaticClass();
    PlayerControllerClass = AAshfrontierPlayerController::StaticClass();
    HUDClass = AAshfrontierHUD::StaticClass();
}

void AAshfrontierGameMode::BeginPlay()
{
    Super::BeginPlay();

    SpawnPrototypeWorld();
}

void AAshfrontierGameMode::SpawnPrototypeWorld()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = TEXT("AF_WorldBlockoutDirector");
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    WorldBlockoutDirector = World->SpawnActor<AAshfrontierWorldBlockoutDirector>(AAshfrontierWorldBlockoutDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    if (WorldBlockoutDirector)
    {
        WorldBlockoutDirector->BuildPrototypeWorld();
    }

    SpawnGate05Hostile();
    SpawnGate06Characters();
    SpawnGate08Guards();
    StartArtScreenshotCaptureIfRequested();
}

void AAshfrontierGameMode::SpawnGate05Hostile()
{
    UWorld* World = GetWorld();
    if (!World || IsValid(Gate05Hostile))
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    Gate05Hostile = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(420.0f, -260.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    if (Gate05Hostile)
    {
        Gate05Hostile->SetSquadDisplayName(TEXT("塵路劫手"));
        Gate05Hostile->SetCharacterTeam(EAshfrontierCharacterTeam::Hostile);
        Gate05Hostile->SetFactionId(TEXT("faction_dustrunners"));
        Gate05Hostile->SetPrototypeBodyMaterialPath(TEXT("/Game/GeneratedArt/Materials/MI_AfV02_Surface_SunBleachedCloth"));
        Gate05Hostile->SpawnDefaultController();
    }
}

void AAshfrontierGameMode::SpawnGate06Characters()
{
    UWorld* World = GetWorld();
    if (!World || Gate06Characters.Num() > 0)
    {
        return;
    }

    struct FRecruitSeed
    {
        const TCHAR* Name;
        const TCHAR* ArtCharacterId;
        const TCHAR* PortraitTexturePath;
        const TCHAR* FactionId;
        const TCHAR* BodyMaterialPath;
        FVector Location;
        int32 Cost;
    };

    const FRecruitSeed Recruits[] = {
        {TEXT("苦根醫士 露禾"), TEXT("recruit_mender_luhe"), TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitMender"), TEXT("faction_dustrunners"), TEXT("/Game/GeneratedArt/Materials/MI_AfV02_Surface_BoneCeramic"), FVector(-260.0f, 340.0f, 120.0f), 60},
        {TEXT("塵路斥候 迦南"), TEXT("recruit_scout_jianan"), TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitScout"), TEXT("faction_dustrunners"), TEXT("/Game/GeneratedArt/Materials/MI_AfV02_Surface_SunBleachedCloth"), FVector(-360.0f, 410.0f, 120.0f), 70},
        {TEXT("鹽棚工匠 莫澤"), TEXT("recruit_trader_moze"), TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitTrader"), TEXT("faction_glasshouse"), TEXT("/Game/GeneratedArt/Materials/MI_AfV02_Surface_IndustrialFloor"), FVector(-460.0f, 360.0f, 120.0f), 80},
        {TEXT("舊牆守手 洛槌"), TEXT("recruit_guard_luochui"), TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitGuard"), TEXT("faction_saltwardens"), TEXT("/Game/GeneratedArt/Materials/MI_AfV02_Surface_RustedMetal"), FVector(-540.0f, 440.0f, 120.0f), 90}
    };

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    for (const FRecruitSeed& Seed : Recruits)
    {
        AAshfrontierCharacter* Recruit = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), Seed.Location, FRotator::ZeroRotator, SpawnParams);
        if (!Recruit)
        {
            continue;
        }

        Recruit->SetSquadDisplayName(Seed.Name);
        Recruit->SetArtCharacterId(Seed.ArtCharacterId);
        Recruit->SetPortraitTexturePath(Seed.PortraitTexturePath);
        Recruit->SetPrototypeBodyMaterialPath(Seed.BodyMaterialPath);
        Recruit->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
        Recruit->SetFactionId(Seed.FactionId);
        Recruit->SetRecruitable(true, Seed.Cost);
        Recruit->SpawnDefaultController();
        Gate06Characters.Add(Recruit);
    }

    AAshfrontierCharacter* Merchant = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(180.0f, 360.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    if (Merchant)
    {
        Merchant->SetSquadDisplayName(TEXT("玻璃屋販商 澄瓶"));
        Merchant->SetArtCharacterId(TEXT("merchant_glasshouse_chengping"));
        Merchant->SetPortraitTexturePath(TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitTrader"));
        Merchant->SetPrototypeBodyMaterialPath(TEXT("/Game/GeneratedArt/Materials/MI_AfV02_Surface_IndustrialFloor"));
        Merchant->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
        Merchant->SetFactionId(TEXT("faction_glasshouse"));
        Merchant->SetShopkeeper(true);
        if (Merchant->GetInventory())
        {
            Merchant->GetInventory()->AddItem(TEXT("item_field_bandage"), 8);
            Merchant->GetInventory()->AddItem(TEXT("item_cooked_ration"), 8);
            Merchant->GetInventory()->AddItem(TEXT("item_medic_wrap"), 3);
            Merchant->GetInventory()->AddItem(TEXT("item_ash_credit"), 240);
        }
        Merchant->SpawnDefaultController();
        Gate06Characters.Add(Merchant);
    }
}

void AAshfrontierGameMode::SpawnGate08Guards()
{
    UWorld* World = GetWorld();
    if (!World || Gate08Guards.Num() > 0)
    {
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AAshfrontierCharacter* CityGuard = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(-120.0f, -500.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    if (CityGuard)
    {
        CityGuard->SetSquadDisplayName(TEXT("鹽脊守衛 磐哨"));
        CityGuard->SetArtCharacterId(TEXT("guard_saltwardens_panshao"));
        CityGuard->SetPortraitTexturePath(TEXT("/Game/GeneratedArt/Characters/T_AfV02_Portrait_RecruitGuard"));
        CityGuard->SetPrototypeBodyMaterialPath(TEXT("/Game/GeneratedArt/Materials/MI_AfV02_Surface_RustedMetal"));
        CityGuard->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
        CityGuard->SetFactionId(TEXT("faction_saltwardens"));
        CityGuard->SpawnDefaultController();
        Gate08Guards.Add(CityGuard);
    }
}

void AAshfrontierGameMode::StartArtScreenshotCaptureIfRequested()
{
    if (!FParse::Param(FCommandLine::Get(), TEXT("AshfrontierCaptureArtScreenshots")))
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    UE_LOG(LogTemp, Display, TEXT("Ashfrontier art screenshot capture requested."));
    if (GEngine)
    {
        GEngine->Exec(World, TEXT("DisableAllScreenMessages"));
    }
    IFileManager::Get().MakeDirectory(*FPaths::Combine(FPaths::ProjectDir(), TEXT("Reports/Art/Screenshots")), true);

    ArtScreenshotViews = {
        {TEXT("01_main_city_entrance"), TEXT("主城入口"), FVector(250.0f, -980.0f, 360.0f), FVector(880.0f, -180.0f, 190.0f), false},
        {TEXT("02_main_city_market"), TEXT("主城商店區"), FVector(-120.0f, 700.0f, 330.0f), FVector(300.0f, 240.0f, 155.0f), false},
        {TEXT("03_guard_patrol"), TEXT("守衛巡邏區"), FVector(-760.0f, -740.0f, 300.0f), FVector(-420.0f, -520.0f, 135.0f), false},
        {TEXT("04_outpost_exterior"), TEXT("前哨外觀"), FVector(2500.0f, -830.0f, 330.0f), FVector(2860.0f, -470.0f, 175.0f), false},
        {TEXT("05_wilderness_build_zone"), TEXT("野外建造區"), FVector(1060.0f, 1260.0f, 380.0f), FVector(1460.0f, 1640.0f, 150.0f), false},
        {TEXT("06_recruit_ui"), TEXT("角色招募 UI"), FVector(-760.0f, 130.0f, 400.0f), FVector(-410.0f, 380.0f, 120.0f), true},
        {TEXT("07_trade_ui"), TEXT("交易 UI"), FVector(-160.0f, -80.0f, 400.0f), FVector(180.0f, 360.0f, 120.0f), true},
        {TEXT("08_inventory_ui"), TEXT("庫存 UI"), FVector(120.0f, -520.0f, 400.0f), FVector(120.0f, -40.0f, 110.0f), true},
    };

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = this;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ArtCaptureCamera = World->SpawnActor<ACameraActor>(ACameraActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
    ArtScreenshotIndex = 0;

    UE_LOG(LogTemp, Display, TEXT("Ashfrontier art screenshot capture prepared %d views."), ArtScreenshotViews.Num());
    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle, this, &AAshfrontierGameMode::CaptureNextArtScreenshot, 3.0f, false);
}

void AAshfrontierGameMode::CaptureNextArtScreenshot()
{
    UWorld* World = GetWorld();
    if (!World || ArtScreenshotIndex >= ArtScreenshotViews.Num())
    {
        FinishArtScreenshotCapture();
        return;
    }

    if (!ArtCaptureCamera)
    {
        FinishArtScreenshotCapture();
        return;
    }

    const FAshfrontierArtScreenshotView& View = ArtScreenshotViews[ArtScreenshotIndex];
    UE_LOG(LogTemp, Display, TEXT("Ashfrontier art screenshot capture view %d/%d: %s"), ArtScreenshotIndex + 1, ArtScreenshotViews.Num(), *View.FileStem);
    ArtCaptureCamera->SetActorLocationAndRotation(View.Location, (View.Target - View.Location).Rotation());

    if (APlayerController* Controller = World->GetFirstPlayerController())
    {
        Controller->SetViewTarget(ArtCaptureCamera);
        if (AHUD* Hud = Controller->GetHUD())
        {
            Hud->bShowHUD = View.bShowHud;
        }
    }

    const FString ScreenshotPath = FPaths::Combine(FPaths::ProjectDir(), TEXT("Reports/Art/Screenshots"), View.FileStem + TEXT(".png"));
    FScreenshotRequest::RequestScreenshot(ScreenshotPath, true, false);
    ++ArtScreenshotIndex;

    FTimerHandle TimerHandle;
    World->GetTimerManager().SetTimer(TimerHandle, this, &AAshfrontierGameMode::CaptureNextArtScreenshot, 1.35f, false);
}

void AAshfrontierGameMode::FinishArtScreenshotCapture()
{
    UE_LOG(LogTemp, Display, TEXT("Ashfrontier art screenshot capture finished."));
    if (GEngine)
    {
        GEngine->Exec(GetWorld(), TEXT("QUIT"));
    }
}
