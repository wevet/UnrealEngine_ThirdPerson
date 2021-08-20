# -*- coding utf-8 -*-

import unreal
import os.path

# configure the factory
import_settings = unreal.FbxImportUI()
# import_settings.create_physics_asset = True
import_settings.import_materials = True
# import_settings.import_textures = True
# import_settings.import_animations = True
# メッシュを1つにまとめる
import_settings.static_mesh_import_data.combine_meshes = True


# register tasks
task = unreal.AssetImportTask()
task.automated = True
# UE4上のアセット名
task.destination_name = "SM_Kaiju"
# アセットを保存するフォルダ
task.destination_path = "Game/Kaiju"
# 読み込みたいFBXファイル名を指定する
task.filename = "D:/test.fbx"
task.options = import_settings

tasks = [task]

# タスクを実行
# FBXとマテリアルがインポートされる
asset_tool = unreal.AssetToolsHelpers.get_asset_tools()
asset_tool.import_asset_tasks(tasks)


# インポートされたマテリアルを削除して、同名のマテリアルインスタンスを作成
# このインスタンスをインポートしたメッシュに割り当てる
mesh_data = unreal.EditorAssetLibrary.find_asset_data(task.destination_path + task.destination_name)
master_mat = unreal.EditorAssetLibrary.find_asset_data('/Game/Test/M_Master_base')
mesh = mesh_data.get_asset()




