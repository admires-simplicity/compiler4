import subprocess
import filecmp
import os

n_examples = len(os.listdir("../examples"))
os.makedirs("c results", exist_ok=True)
os.makedirs("exe results", exist_ok=True)

for i in range(n_examples):
    with open(f"c results/ex{i}.c", "w") as outfile:
        subprocess.run(["../compiler4", f"../examples/ex{i}.src"], stdout=outfile)
        subprocess.run(["gcc", f"c results/ex{i}.c", "-o", f"exe results/ex{i}"])

