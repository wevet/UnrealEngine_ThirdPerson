# -*- coding utf-8 -*-

import unreal
import numpy as np
import sklearn
from sklearn.cluster import KMeans


"""
using
import TPS_Modules.instancing_generator as instancing_generator
import importlib
importlib.reload(instancing_generator)
instancing_generator.create_instancing_mesh()
"""


def create_instancing_mesh():
    print("create_instancing_mesh")

    # 用意したインスタンス用Blueprintクラスをロード
    bp_instance = unreal.EditorAssetLibrary.load_blueprint_class('/Game/Game/Blueprints/Instancing/BP_Foliage_Instancing.BP_Foliage_Instancing')

    # 選択したStatic Mesh Actorを取得
    list_actors = unreal.EditorLevelLibrary.get_selected_level_actors()
    list_static_mesh_actors = unreal.EditorFilterLibrary.by_class(list_actors, unreal.StaticMeshActor)
    # list_unique = np.array([])
    list_unique = []

    # 選択したアクタに含まれるStatic Meshの種類を調べる
    for lsm in list_static_mesh_actors:
        static_mesh = lsm.get_component_by_class(unreal.StaticMeshComponent).get_editor_property("StaticMesh")
        # list_unique = np.append(list_unique, static_mesh)
        list_unique.append(static_mesh)

    list_unique = np.unique(list_unique)

    for lu in list_unique:
        list_transform = np.array([])
        list_locations = np.array([[0, 0, 0]])

        # クラスタリング用の位置情報と、インスタンスに追加するトランスフォームの配列を作成
        for lsm in list_static_mesh_actors:
            if lsm.get_component_by_class(unreal.StaticMeshComponent).get_editor_property("StaticMesh") == lu:
                list_transform = np.append(list_transform, lsm.get_actor_transform())
                position_x = lsm.get_actor_location().x
                position_y = lsm.get_actor_location().y
                position_z = lsm.get_actor_location().z
                location = np.array([[position_x, position_y, position_z]])
                list_locations = np.append(list_locations, location, axis=0)

        list_locations = np.delete(list_locations, 0, axis=0)

        # クラスタ数を指定
        num_clusters = 5
        # 位置情報を元にクラスタリング
        fit_predict = KMeans(n_clusters=num_clusters).fit_predict(list_locations)

        # 格納する配列を作成
        instanced_components = np.array([])
        # instanced_components = []

        # インスタンス用アクタをスポーン
        for i in range(num_clusters):
            instanced_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(bp_instance, (0, 0, 0), (0, 0, 0))
            instanced_component = instanced_actor.get_component_by_class(unreal.InstancedStaticMeshComponent)
            instanced_components = np.append(instanced_components, instanced_component)
            # instanced_components.append(instanced_component)

        # クラスタ番号を元にインスタンスを追加
        for j, pd in enumerate(fit_predict):
            component = instanced_components[pd]
            component.add_instance(list_transform[j])

        # インスタンスにStatic Meshを割り当て
        for k in range(num_clusters):
            component = instanced_components[k]
            component.set_editor_property("StaticMesh", lu)

    # 最初に選択したアクタを削除
    for lsm in list_static_mesh_actors:
        lsm.destroy_actor()


"""
using
import TPS_Modules.instancing_generator as instancing_generator
import importlib
importlib.reload(instancing_generator)
instancing_generator.split_instancing_mesh()
"""


def split_instancing_mesh():
    print("split_instancing_mesh")

    # 選択したアクタを取得
    selected_actors = unreal.EditorLevelLibrary.get_selected_level_actors()

    # インスタンスコンポーネントを取得
    for selected_actor in selected_actors:
        # instanced_components = np.array([])
        instanced_components = []
        instanced_component = selected_actor.get_components_by_class(unreal.InstancedStaticMeshComponent)
        # instanced_components = np.append(instanced_components, instanced_component)
        instanced_components.append(instanced_component)

        # コンポーネントに含まれるインスタンス数を取得
        for component in instanced_components:
            # instance_transform = np.array([])
            instance_transform = []
            instance_count = component.get_instance_count()

            # インスタンスの数分Static Mesh Actorをスポーンし、トランスフォーム割り当て、Static Mesh割り当てを繰り返す
            for j in range(instance_count):
                # instance_transform = np.append(instance_transform, component.get_instance_transform(j, 1))
                instance_transform.append(component.get_instance_transform(j, 1))
                spawned_actor = unreal.EditorLevelLibrary.spawn_actor_from_class(unreal.StaticMeshActor, (0, 0, 0), (0, 0, 0))
                spawned_actor.set_actor_transform(instance_transform[j], 0, 0)
                smc = spawned_actor.get_component_by_class(unreal.StaticMeshComponent)
                smc.set_editor_property("StaticMesh", component.get_editor_property("StaticMesh"))

    # 最初に選択していたアクタを削除
    for selected_actor in selected_actors:
        selected_actor.destroy_actor()


