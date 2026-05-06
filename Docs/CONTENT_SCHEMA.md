# Ashfrontier 內容資料 Schema

## Summary

Ashfrontier 的內容資料以穩定 ID 驅動，使用者可見文字使用繁體中文。Gate 02 起資料建議放在 `Content/Data/`，也允許放在 `Source/Data/` 供 C++ 測試載入。所有資料必須能被 `Scripts/content_lint.py` 解析。

Gate 02 的初始資料落在 `Content/Data/`，每個檔案使用 JSON 物件包住 `type` 與 `records`：

- `Content/Data/factions.json`
- `Content/Data/items.json`
- `Content/Data/resources.json`
- `Content/Data/recipes.json`
- `Content/Data/buildings.json`
- `Content/Data/npc_schedules.json`
- `Content/Data/legal_rules.json`
- `Content/Data/dialogue_conditions.json`

UE automation test `Ashfrontier.Data.ContentJsonLoads` 會在 Editor automation 中確認這些 JSON 檔存在、可解析，且基本筆數符合 Gate 02 的最低要求。

## 共通規則

- `id`：必填，ASCII slug，使用小寫、數字與底線，例如 `faction_saltwardens`。
- `name_zh`：必填，繁體中文顯示名稱。
- `description_zh`：建議填寫，繁體中文描述。
- 引用其他資料時使用對方的 `id`。
- 價格、成本、數量不可為負數。
- 可玩家看見的文字不得使用簡體中文。
- Gate 02 起缺少必要資料集、筆數不足、重複 ID、無效引用、配方循環或非法成本都會讓 `content_lint.py` 失敗。

## factions

必要欄位：

```json
{
  "id": "faction_saltwardens",
  "name_zh": "鹽脊守望",
  "description_zh": "守住鹽脊商道的武裝自治團。",
  "starting_relation": {
    "faction_glasshouse": -10
  },
  "legal_profile_id": "legal_saltwardens_city"
}
```

規則：

- 至少 3 個原創派系。
- `starting_relation` 引用的 faction id 必須存在。
- 不得使用 Kenshi / Kenshi 2 的派系名稱。

## items

必要欄位：

```json
{
  "id": "item_field_bandage",
  "name_zh": "野外繃帶",
  "description_zh": "粗布與藥粉製成的臨時包紮用品。",
  "category": "consumable",
  "base_price": 18,
  "stack_size": 5,
  "weight": 0.2
}
```

規則：

- 至少 12 種物品。
- `base_price`、`stack_size`、`weight` 不可小於 0。
- `category` 建議使用 `weapon`、`armor`、`consumable`、`material`、`trade_good`、`tool`。

## resources

必要欄位：

```json
{
  "id": "resource_scrap_ore",
  "name_zh": "碎鐵礦",
  "description_zh": "可冶煉成粗鐵的低純度礦石。",
  "gather_time_seconds": 8,
  "produces_item_id": "item_scrap_ore"
}
```

規則：

- 至少 5 種資源。
- 採集時間不可小於 0。
- 若填寫 `produces_item_id`，必須引用存在的 item。

## recipes

必要欄位：

```json
{
  "id": "recipe_cooked_ration",
  "name_zh": "烤行糧",
  "chain": "food",
  "inputs": [
    { "id": "resource_dry_grain", "count": 2 }
  ],
  "outputs": [
    { "id": "item_cooked_ration", "count": 1 }
  ],
  "work_seconds": 20,
  "station_id": "building_camp_kitchen"
}
```

規則：

- 至少 2 條生產鏈：食物鏈與金屬 / 建材鏈。
- `chain` 至少必須涵蓋 `food` 與 `metal`。
- `inputs` 與 `outputs` 的 count 必須大於 0。
- `inputs` 與 `outputs` 必須引用存在的 item 或 resource。
- `station_id` 必須引用存在的 building。
- 不得形成 recipe 循環。

## buildings

必要欄位：

```json
{
  "id": "building_storage_crate",
  "name_zh": "粗木儲物箱",
  "description_zh": "可存放材料與行糧的簡易箱。",
  "category": "storage",
  "cost": [
    { "id": "item_salvaged_plank", "count": 4 }
  ],
  "allowed_zone": "wilderness_build_zone"
}
```

規則：

- 至少 5 種建築。
- 必須包含營火或烹飪點、儲物箱、採集點或礦點、簡易工坊、防禦牆或防禦物。
- 建築成本 count 必須大於 0。
- 建築成本必須引用存在的 item 或 resource。
- 若填寫 `resource_id`，必須引用存在的 resource。

## npc_schedules

必要欄位：

```json
{
  "id": "schedule_market_runner",
  "name_zh": "市集搬運路線",
  "faction_id": "faction_saltwardens",
  "entries": [
    { "time": "08:00", "action": "move_to", "target": "wp_market_gate" }
  ]
}
```

規則：

- 至少 3 組 NPC schedule。
- `faction_id` 必須引用存在的 faction。
- `entries` 不可為空。
- `entries` 內每筆必須包含 `time`、`action` 與 `target`。

## dialogue_conditions

必要欄位：

```json
{
  "id": "dialogue_recruit_medic_intro",
  "name_zh": "醫療 recruit 開場條件",
  "conditions": [
    { "type": "relation_at_least", "faction_id": "faction_saltwardens", "value": 0 }
  ]
}
```

規則：

- faction reference 必須有效。
- item reference 必須有效。
- 條件值必須能被測試或 content lint 檢查。

## legal_rules

必要欄位：

```json
{
  "id": "legal_saltwardens_city",
  "name_zh": "鹽脊城市法規",
  "faction_id": "faction_saltwardens",
  "rules": [
    {
      "event": "theft",
      "reaction": "warning",
      "memory_seconds": 300,
      "relation_delta": -3
    }
  ]
}
```

規則：

- `event` 建議使用 `theft`、`attack`、`self_defense`、`restricted_area`。
- `reaction` 建議使用 `allow`、`warning`、`hostile`、`pursuit`。
- `memory_seconds` 不可小於 0。
- `relation_delta` 若存在必須是數字。
