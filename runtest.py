import os

dir = os.walk("./nyx_test")

for root, dirs, files in dir:
    for file in files:
        path = os.path.join(root, file)
        ret = os.system("./build/nyx " + path + "> /dev/null 2>&1")
        if ret != 0:
            print("\033[31mTest failed: " + file)
        else:
            print("\033[32mTest passed: " + file)
