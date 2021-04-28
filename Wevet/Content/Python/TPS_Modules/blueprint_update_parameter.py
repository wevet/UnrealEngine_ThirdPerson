# -*- coding utf-8 -*-

import unreal

"""
using

import TPS_Modules.blueprint_update_parameter as blueprint_update_parameter
import importlib
importlib.reload(blueprint_update_parameter)
blueprint_update_parameter.set_selected_assets_property("IntVar", 100)

blueprint_update_parameter.set_selected_assets_property("Var_TestObjClass",
    unreal.load_object(None,"BlueprintGeneratedClass'/Game/Blueprint/BP_Test_Obj.BP_Test_Obj_C'"))

"""


@unreal.uclass()
class EUL(unreal.EditorUtilityLibrary):
    pass


def get_default_class_object(asset_path):
    # アセットのPathを"."で分ける
    Str = asset_path.split(".")
    # 分けた右側部分 + "Default__" + 分けた左側部分 + "_C"となるように文字列を生成
    result_path = ".".join((Str[0], "Default__{}_C".format(Str[1])))
    # 生成したデフォルトクラスオブジェクトのパスからオブジェクトを生成して返す
    return unreal.load_object(None, result_path)


def set_default_property(asset_path, property_name, property_value):
    # デフォルトクラスのオブジェクトを取得
    DefaultClassObj = get_default_class_object(asset_path)

    # 指定した変数のデフォルトの値をセットする
    DefaultClassObj.set_editor_property(property_name, property_value)

    print(DefaultClassObj)


def set_selected_assets_property(property_name, property_value):
    # EditorUtilityLibraryのインスタンスを生成
    editor_utility_instance = EUL()

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



