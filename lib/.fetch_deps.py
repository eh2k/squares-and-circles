import subprocess
import os
import io
import urllib.request
import zipfile
import shutil

subprocess.run('git submodule update --init', shell=True)

from os.path import isdir, join

from SCons.Script import DefaultEnvironment
env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()

print(env["PIOENV"])

