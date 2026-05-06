#include "AshfrontierGameMode.h"

#include "AshfrontierCharacter.h"
#include "AshfrontierHUD.h"
#include "AshfrontierInventoryComponent.h"
#include "AshfrontierPlayerController.h"
#include "AshfrontierWorldBlockoutDirector.h"

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
        FVector Location;
        int32 Cost;
    };

    const FRecruitSeed Recruits[] = {
        {TEXT("苦根醫士 露禾"), FVector(-260.0f, 340.0f, 120.0f), 60},
        {TEXT("塵路斥候 迦南"), FVector(-360.0f, 410.0f, 120.0f), 70},
        {TEXT("鹽棚工匠 莫澤"), FVector(-460.0f, 360.0f, 120.0f), 80},
        {TEXT("舊牆守手 洛槌"), FVector(-540.0f, 440.0f, 120.0f), 90}
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
        Recruit->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
        Recruit->SetRecruitable(true, Seed.Cost);
        Recruit->SpawnDefaultController();
        Gate06Characters.Add(Recruit);
    }

    AAshfrontierCharacter* Merchant = World->SpawnActor<AAshfrontierCharacter>(AAshfrontierCharacter::StaticClass(), FVector(180.0f, 360.0f, 120.0f), FRotator::ZeroRotator, SpawnParams);
    if (Merchant)
    {
        Merchant->SetSquadDisplayName(TEXT("玻璃屋販商 澄瓶"));
        Merchant->SetCharacterTeam(EAshfrontierCharacterTeam::Neutral);
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
