import os,sys

dir = os.walk("../nyx_test")

for root,dirs,files in dir:
    for file in files:
        path = os.path.join(root,file)
        ret = os.system("../build/nyx "+path)
        if ret!=0:
            print("Error running "+path)

print("All tests passed!")

