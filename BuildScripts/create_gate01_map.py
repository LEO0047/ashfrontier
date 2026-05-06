import unreal

MAP_DIR = "/Game/Maps"
MAP_PATH = f"{MAP_DIR}/L_Ashfrontier_Prototype"


def log(message):
    unreal.log(f"Ashfrontier Gate 01: {message}")


def warn(message):
    unreal.log_warning(f"Ashfrontier Gate 01: {message}")


def ensure_directory(path):
    if not unreal.EditorAssetLibrary.does_directory_exist(path):
        unreal.EditorAssetLibrary.make_directory(path)


def main():
    ensure_directory(MAP_DIR)

    if unreal.EditorAssetLibrary.does_asset_exist(MAP_PATH):
        log(f"載入既有 map：{MAP_PATH}")
        unreal.EditorLevelLibrary.load_level(MAP_PATH)
    else:
        log(f"建立新 map：{MAP_PATH}")
        if not unreal.EditorLevelLibrary.new_level(MAP_PATH):
            raise RuntimeError(f"無法建立 map：{MAP_PATH}")

    if not unreal.EditorLevelLibrary.save_current_level():
        raise RuntimeError("無法儲存 Gate 01 map。")

    log("Gate 01 playable map 已建立並儲存。")
    unreal.SystemLibrary.quit_editor()


main()
