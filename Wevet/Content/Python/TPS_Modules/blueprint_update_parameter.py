# -*- coding utf-8 -*-

import unreal


@unreal.uclass()
class EUL(unreal.EditorUtilityLibrary):
    pass


def get_default_class_object(asset_path):
    # アセットのパスを"."で分ける
    path = asset_path.spit(".")
    # 分けた右側部分 + "Default__" + 分けた左側部分 + "_C"となるように文字列を生成
    result_path = ".".join((path[0], "Default__{}_C".format(path[1])))
    # 生成したデフォルトクラスオブジェクトのパスからオブジェクトを生成して返す
    return unreal.load_object(None, result_path)


def set_default_property(asset_path, property_name, property_value):
    # デフォルトクラスのオブジェクトを取得
    default_class_object = get_default_class_object(asset_path)

    # 指定した変数のデフォルトの値をセットする
    default_class_object.set_editor_property(property_name, property_value)


def set_selected_assets_property(property_name, property_value):
    # EditorUtilityLibraryのインスタンスを生成
    # editor_utility_instance = EUL()

    # 現在コンテンツブラウザで選択しているアセットを取得
    selected_assets = EUL.get_selected_assets()

    # 選択しているアセットをFor分で回す
    for asset in selected_assets:

        # Blueprintクラスかをチェック
        try:
            unreal.Blueprint.cast(asset)
        except:
            # BPクラスじゃないなら次へ
            error_log = str(asset.get_name()) + "は、BlueprintClassではありません。"
            unreal.log_error(error_log)
            continue

        try:
            # 指定した変数のデフォルトの値をセットする
            set_default_property(asset.get_path_name(), property_name, property_value)
        except:
            # セットできなくても次へ
            error_log = str(asset.get_name()) + "は、Property「" + property_name + "」を持っていません。"
            unreal.log_error(error_log)
            continue



