import os
import sys


def execute(file, path, redirect):
    cmd = "./build/nyx " + path
    if redirect:
        cmd += ">/dev/null 2>&1"
    ret = os.system(cmd)
    if ret != 0:
        print("\n\033[31mTest failed: " + file)
    else:
        print("\n\033[32mTest passed: " + file)


def run_test(all):
    dir = os.walk("./nyx_test")
    for root, dirs, files in dir:
        for file in files:
            path = os.path.join(root, file)
            if all:
                execute(file, path, True)
            else:
                if sys.argv[1] == file:
                    execute(file, path, False)


if len(sys.argv) > 1:
    run_test(False)
else:
    run_test(True)
