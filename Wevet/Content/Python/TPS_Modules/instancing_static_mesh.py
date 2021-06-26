# -*- coding utf-8 -*-

import unreal
import numpy as np
import sklearn
from sklearn.cluster import KMeans

# 用意したインスタンス用Blueprintクラスをロード
bp_instance = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Game/Instancing/BP_Instancing.BP_Instancing')
# bp_instance = unreal.load_object(None, '/Game/Game/Instancing/BP_Instancing.BP_Instancing')

# 選択したStatic Mesh Actorを取得
list_actors = unreal.EditorLevelLibrary.get_selected_level_actors()
list_static_mesh_actors = unreal.EditorFilterLibrary.by_class(list_actors,unreal.StaticMeshActor)


