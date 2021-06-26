# -*- coding utf-8 -*-

import unreal


"""
using
import TPS_Modules.generate_skeletalmesh_lod as generate_skeletalmesh_lod
import importlib
importlib.reload(generate_skeletalmesh_lod)
generate_skeletalmesh_lod.generate_lod()
"""


def generate_skeletalmesh_lod():
    # 選択しているアセットを取得
    util = unreal.EditorUtilityLibrary.get_default_object()
    asset_list = util.get_selected_asset_data()

    unreal.log("Start generate_skeletalmesh_lod")

    for asset in asset_list:
        skeletal_mesh = asset.get_asset()

        # LOD配列
        lod_info = []

        # Baseのモデルは変更したくないためBaseのLODを取得し、設定
        base_lod_info = skeletal_mesh.get_editor_property("lod_info")
        lod_info.append(base_lod_info[0])
        screen_size_array = [0.8, 0.6, 0.4, 0.3]

        # Fixed ScreenSize
        for screen_size in screen_size_array:
            per_platform_float = unreal.PerPlatformFloat()
            per_platform_float.set_editor_property("default", screen_size)
            info = unreal.SkeletalMeshLODInfo()
            info.set_editor_property("screen_size", per_platform_float)
            lod_info.append(info)

        # BaseLODからの頂点の割合を設定
        vert_parcentage_array = [0.06, 0.03, 0.02, 0.01]
        # リダクションの方法を設定
        termination_array = [1, 1, 1, 1]
        # BaseLodの次から開始するためにcount変数を1に設定
        count = 1
        for i in range(len(screen_size_array)):
            optimization_setting = unreal.SkeletalMeshOptimizationSettings()
            optimization_setting.set_editor_property("num_of_vert_percentage", vert_parcentage_array[i])
            # 頂点の%率で削減率を決定
            optimization_setting.set_editor_property("termination_criterion", unreal.SkeletalMeshTerminationCriterion.cast(termination_array[i]))
            lod_info[count].set_editor_property("reduction_settings", optimization_setting)
            count += 1

        # LODInfoを設定
        skeletal_mesh.set_editor_property("lod_info", lod_info)
        # LODを再生成
        skeletal_mesh.regenerate_lod(len(lod_info), True)
        # MinLodの設定
        min_lod = [1]
        for setting_val in min_lod:
            per_platform_int = unreal.PerPlatformInt()
            per_platform_int.set_editor_property("default", setting_val)
            skeletal_mesh.set_editor_property("min_lod", per_platform_int)

        # Save SkeletalMesh LOD
        unreal.EditorAssetLibrary.save_asset(asset.get_full_name(), only_if_is_dirty=False)

    unreal.log("End generate_skeletalmesh_lod")

