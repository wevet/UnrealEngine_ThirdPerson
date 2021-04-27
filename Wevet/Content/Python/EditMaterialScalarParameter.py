# -*- coding: utf-8 -*-

#=======================================================================================================#
#ContentBrowserで選択しているMaterialInstanceの「Parameter.py」で指定したScalarParameterの値を上書きする#
#=======================================================================================================#
import unreal

#---------------------------#
#GlobalEditorUtilityのクラス#
#---------------------------#
@unreal.uclass()
class GEditUtil(unreal.GlobalEditorUtilityBase):
	pass

@unreal.uclass()
class MatEditLib(unreal.MaterialEditingLibrary):
	pass


#------------------------------------------------------------------------------#
#Dictで作られたパラメーターの一覧から代入するScalarParameterValue型の配列を作成#
#------------------------------------------------------------------------------#

def SetScalarParameter(PND, MI):
	MEL = MatEditLib()
	PNDKeys = PND.keys()

	for Key in PNDKeys:
		#指定したScalarParameterの値を指定した値にする
		RetVal = MEL.set_material_instance_scalar_parameter_value(MI, unreal.Name(Key),PND[Key])

		#RetValがFalseなら編集できなかったというログはいてるけど、実際は絶対Falseなので、これ不要
'''
		if RetVal == False:
			print("Failed 'set_material_instance_scalar_parameter_value' MI = '"  + MI.get_name() + "' ParamName = '" + Key + "'")
'''

#---------------------------#
#ScalarParameterの値を上書く#
#---------------------------#

def SetSelectedMIScalarParameter(PND):

	Util = GEditUtil()
	AssetList = Util.get_selected_asset()

	#選択しているアセットをFor分で回して、MaterialInstanceだったら、指定したScalarParameterを指定した値にする
	for Asset in AssetList:
		AssetName = Asset.get_name()

		if type(Asset) != unreal.MaterialInstanceConstant:
			print ("Error:Selected [" + AssetName + "] is Not MaterialInstanceAsset.")
			continue

		MI = unreal.MaterialInstanceConstant.cast(Asset)

		SetScalarParameter(PND, MI)

		print("Edit MaterialParameterValue that " + AssetName + "has.")


