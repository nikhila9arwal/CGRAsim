
from glob import glob
import os

env = Environment(ENV = os.environ)

env["CCFLAGS"] = ["-Wall", "-Wextra", "-Werror", '-O0', '-gdwarf-3']
env["CXXFLAGS"] = ["-std=c++17"]

env.Append(LIBS = ["config++"])

env.Program("cgra", [Glob("*.cpp")])
