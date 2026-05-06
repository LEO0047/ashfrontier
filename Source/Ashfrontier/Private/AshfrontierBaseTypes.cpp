#include "AshfrontierBaseTypes.h"

FAshfrontierItemQuantity::FAshfrontierItemQuantity(const FName& InId, int32 InCount)
    : Id(InId)
    , Count(InCount)
{
}

namespace
{
FAshfrontierItemQuantity Qty(const TCHAR* Id, int32 Count)
{
    return FAshfrontierItemQuantity(FName(Id), Count);
}
}

void AshfrontierBaseData::BuildDefaultBuildingDefinitions(TMap<FName, FAshfrontierBuildingDefinition>& OutDefinitions)
{
    OutDefinitions.Reset();

    FAshfrontierBuildingDefinition CampKitchen;
    CampKitchen.BuildingId = TEXT("building_camp_kitchen");
    CampKitchen.NameZh = TEXT("營火灶");
    CampKitchen.Category = TEXT("production");
    CampKitchen.AllowedZone = TEXT("wilderness_build_zone");
    CampKitchen.Cost = {Qty(TEXT("item_salvaged_plank"), 2), Qty(TEXT("item_scrap_ore"), 1)};
    OutDefinitions.Add(CampKitchen.BuildingId, CampKitchen);

    FAshfrontierBuildingDefinition StorageCrate;
    StorageCrate.BuildingId = TEXT("building_storage_crate");
    StorageCrate.NameZh = TEXT("粗木儲物箱");
    StorageCrate.Category = TEXT("storage");
    StorageCrate.AllowedZone = TEXT("wilderness_build_zone");
    StorageCrate.Cost = {Qty(TEXT("item_salvaged_plank"), 4)};
    OutDefinitions.Add(StorageCrate.BuildingId, StorageCrate);

    FAshfrontierBuildingDefinition ScrapMine;
    ScrapMine.BuildingId = TEXT("building_scrap_mine");
    ScrapMine.NameZh = TEXT("碎鐵採集點");
    ScrapMine.Category = TEXT("gathering");
    ScrapMine.AllowedZone = TEXT("wilderness_build_zone");
    ScrapMine.Cost = {Qty(TEXT("item_salvaged_plank"), 3), Qty(TEXT("item_building_plate"), 1)};
    ScrapMine.ResourceId = TEXT("resource_scrap_ore");
    OutDefinitions.Add(ScrapMine.BuildingId, ScrapMine);

    FAshfrontierBuildingDefinition FieldWorkshop;
    FieldWorkshop.BuildingId = TEXT("building_field_workshop");
    FieldWorkshop.NameZh = TEXT("野地工坊");
    FieldWorkshop.Category = TEXT("production");
    FieldWorkshop.AllowedZone = TEXT("wilderness_build_zone");
    FieldWorkshop.Cost = {Qty(TEXT("item_salvaged_plank"), 5), Qty(TEXT("item_crude_ingot"), 2)};
    OutDefinitions.Add(FieldWorkshop.BuildingId, FieldWorkshop);

    FAshfrontierBuildingDefinition PalisadeWall;
    PalisadeWall.BuildingId = TEXT("building_palisade_wall");
    PalisadeWall.NameZh = TEXT("釘板防線");
    PalisadeWall.Category = TEXT("defense");
    PalisadeWall.AllowedZone = TEXT("wilderness_build_zone");
    PalisadeWall.Cost = {Qty(TEXT("item_salvaged_plank"), 6), Qty(TEXT("item_building_plate"), 2)};
    OutDefinitions.Add(PalisadeWall.BuildingId, PalisadeWall);
}

void AshfrontierBaseData::BuildDefaultResourceDefinitions(TMap<FName, FAshfrontierResourceDefinition>& OutDefinitions)
{
    OutDefinitions.Reset();

    FAshfrontierResourceDefinition DryGrain;
    DryGrain.ResourceId = TEXT("resource_dry_grain");
    DryGrain.NameZh = TEXT("旱地穀穗");
    DryGrain.ProducesItemId = TEXT("item_raw_grain");
    DryGrain.GatherTimeSeconds = 6.0f;
    OutDefinitions.Add(DryGrain.ResourceId, DryGrain);

    FAshfrontierResourceDefinition BrackishWater;
    BrackishWater.ResourceId = TEXT("resource_brackish_water");
    BrackishWater.NameZh = TEXT("鹹井水");
    BrackishWater.ProducesItemId = TEXT("item_water_flask");
    BrackishWater.GatherTimeSeconds = 5.0f;
    OutDefinitions.Add(BrackishWater.ResourceId, BrackishWater);

    FAshfrontierResourceDefinition ScrapOre;
    ScrapOre.ResourceId = TEXT("resource_scrap_ore");
    ScrapOre.NameZh = TEXT("碎鐵礦");
    ScrapOre.ProducesItemId = TEXT("item_scrap_ore");
    ScrapOre.GatherTimeSeconds = 8.0f;
    OutDefinitions.Add(ScrapOre.ResourceId, ScrapOre);

    FAshfrontierResourceDefinition SalvageWood;
    SalvageWood.ResourceId = TEXT("resource_salvage_wood");
    SalvageWood.NameZh = TEXT("舊棚木料");
    SalvageWood.ProducesItemId = TEXT("item_salvaged_plank");
    SalvageWood.GatherTimeSeconds = 7.0f;
    OutDefinitions.Add(SalvageWood.ResourceId, SalvageWood);

    FAshfrontierResourceDefinition MedicHerb;
    MedicHerb.ResourceId = TEXT("resource_medic_herb");
    MedicHerb.NameZh = TEXT("苦根藥草");
    MedicHerb.ProducesItemId = TEXT("item_medic_wrap");
    MedicHerb.GatherTimeSeconds = 9.0f;
    OutDefinitions.Add(MedicHerb.ResourceId, MedicHerb);
}

void AshfrontierBaseData::BuildDefaultRecipeDefinitions(TMap<FName, FAshfrontierRecipeDefinition>& OutDefinitions)
{
    OutDefinitions.Reset();

    FAshfrontierRecipeDefinition Ration;
    Ration.RecipeId = TEXT("recipe_grain_to_ration");
    Ration.NameZh = TEXT("烤行糧");
    Ration.Chain = TEXT("food");
    Ration.StationId = TEXT("building_camp_kitchen");
    Ration.Inputs = {Qty(TEXT("item_raw_grain"), 2), Qty(TEXT("item_water_flask"), 1)};
    Ration.Outputs = {Qty(TEXT("item_cooked_ration"), 1)};
    Ration.WorkSeconds = 20.0f;
    OutDefinitions.Add(Ration.RecipeId, Ration);

    FAshfrontierRecipeDefinition MedicWrap;
    MedicWrap.RecipeId = TEXT("recipe_herb_to_medic_wrap");
    MedicWrap.NameZh = TEXT("草藥壓布");
    MedicWrap.Chain = TEXT("food");
    MedicWrap.StationId = TEXT("building_camp_kitchen");
    MedicWrap.Inputs = {Qty(TEXT("item_medic_wrap"), 1), Qty(TEXT("item_field_bandage"), 1)};
    MedicWrap.Outputs = {Qty(TEXT("item_medic_wrap"), 1)};
    MedicWrap.WorkSeconds = 18.0f;
    OutDefinitions.Add(MedicWrap.RecipeId, MedicWrap);

    FAshfrontierRecipeDefinition Ingot;
    Ingot.RecipeId = TEXT("recipe_ore_to_ingot");
    Ingot.NameZh = TEXT("冶煉粗鐵");
    Ingot.Chain = TEXT("metal");
    Ingot.StationId = TEXT("building_field_workshop");
    Ingot.Inputs = {Qty(TEXT("item_scrap_ore"), 3)};
    Ingot.Outputs = {Qty(TEXT("item_crude_ingot"), 1)};
    Ingot.WorkSeconds = 30.0f;
    OutDefinitions.Add(Ingot.RecipeId, Ingot);

    FAshfrontierRecipeDefinition Plate;
    Plate.RecipeId = TEXT("recipe_ingot_to_plate");
    Plate.NameZh = TEXT("拼裝建材板");
    Plate.Chain = TEXT("metal");
    Plate.StationId = TEXT("building_field_workshop");
    Plate.Inputs = {Qty(TEXT("item_crude_ingot"), 1), Qty(TEXT("item_salvaged_plank"), 2)};
    Plate.Outputs = {Qty(TEXT("item_building_plate"), 1)};
    Plate.WorkSeconds = 24.0f;
    OutDefinitions.Add(Plate.RecipeId, Plate);
}
