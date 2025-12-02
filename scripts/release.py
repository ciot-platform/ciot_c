import shutil
import os

release_arduino = True

if release_arduino:
    ciot_dir = 'release/arduino/ciot'
    if os.path.exists(ciot_dir):
        shutil.rmtree(ciot_dir)
    os.makedirs(ciot_dir)
    shutil.copytree('include', 'release/arduino/ciot/include')
    shutil.copytree('src/common', 'release/arduino/ciot/src')
    shutil.copytree('src/ino', 'release/arduino/ciot/src', dirs_exist_ok=True)
