from SCons.Script import DefaultEnvironment, Default
import shutil, os

env = DefaultEnvironment()

if env.get("PROGNAME", "program") == "program":
    env.Replace(PROGNAME="firmware")

# print(env.Dump())


env.AddPlatformTarget("build", None, env.GetBuildPath(f"$PROJECT_DIR/app/build.sh"), "Upload")
env.AddPlatformTarget("upload", "build", env.GetBuildPath(f"$PROJECT_DIR/app/upload.py"), "Upload")
              
Default(["build"])