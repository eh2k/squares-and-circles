import subprocess
from SCons.Script import DefaultEnvironment

env = DefaultEnvironment()

commit_sha = subprocess.run('git rev-parse --short HEAD', shell=True, capture_output=True).stdout.strip().upper().decode("utf-8")

if subprocess.run('git diff-index --name-status --exit-code HEAD', shell=True).returncode != 0:
    commit_sha = commit_sha[:6] + "~"

print("GIT_COMMIT_SHA:", commit_sha)

env.Append(CPPDEFINES=[
    ("GIT_COMMIT_SHA", env.StringifyMacro(commit_sha)),
])


