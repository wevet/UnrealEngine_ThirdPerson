# -*- coding: utf-8 -*-

import os


def export_log(log_string, log_path, log_name):
    # Pathのフォルダがない場合は作成
    try:
        os.makedirs(log_path)
    except:
        pass
    file = open(log_path + "\\" + log_name, "w")
    file.write(log_string)
    file.close()
