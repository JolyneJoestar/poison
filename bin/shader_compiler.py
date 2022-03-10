import os
import subprocess

vulkan_sdk_path = os.getenv('VULKANSDK_PATH')

glslc_path = ''

if vulkan_sdk_path == None
    print("There is no Vulkan SDk, Please install it first")
else
    glslc_path = vulkan_sdk_path + "\Bin\glslc.exe"

def runCmd(cmd)
    res = subprocess.Popen(cmd, shell = True, stdout = subprocess.PIPE, stderr = subprocess.STDOUT)
