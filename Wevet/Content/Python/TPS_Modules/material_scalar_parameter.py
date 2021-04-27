# -*- coding: utf-8 -*-

# =======================================================================================================#
# ContentBrowserで選択しているMaterialInstanceの「Parameter.py」で指定したScalarParameterの値を上書きする#
# =======================================================================================================#
import unreal


# ---------------------------#
# GlobalEditorUtilityのクラス#
# ---------------------------#
@unreal.uclass()
class GEditUtil(unreal.GlobalEditorUtilityBase):
    pass


@unreal.uclass()
class MatEditLib(unreal.MaterialEditingLibrary):
    pass


# ------------------------------------------------------------------------------#
# Dictで作られたパラメーターの一覧から代入するScalarParameterValue型の配列を作成#
# ------------------------------------------------------------------------------#
def set_scalar_parameter(pnd, mi):
    mel = MatEditLib()
    pnd_keys = pnd.keys()

    for key in pnd_keys:
        # 指定したScalarParameterの値を指定した値にする
        value = mel.set_material_instance_scalar_parameter_value(mi, unreal.Name(key), pnd[key])

        # RetValがFalseなら編集できなかったというログはいてるけど、実際は絶対Falseなので、これ不要
        if not value:
            print("Failed set_material_instance_scalar_parameter_value MI = " + mi.get_name() + " Name = " + key + "")


def set_selected_scalar_parameter(pnd):
    util = GEditUtil()
    asset_list = util.get_selected_asset()

    # 選択しているアセットをFor分で回して、MaterialInstanceだったら、指定したScalarParameterを指定した値にする
    for asset in asset_list:
        asset_name = asset.get_name()

        if type(asset) != unreal.MaterialInstanceConstant:
            print("Error:Selected [" + asset_name + "] is Not MaterialInstanceAsset.")
            continue

        material_instance = unreal.MaterialInstanceConstant.cast(asset)

        set_scalar_parameter(pnd, material_instance)

        print("Edit MaterialParameterValue that " + asset_name + "has.")

