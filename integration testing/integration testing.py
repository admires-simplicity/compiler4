import subprocess
import filecmp
import os

n_examples = len(os.listdir("../examples"))
os.makedirs("c results", exist_ok=True)
os.makedirs("exes", exist_ok=True)
os.makedirs("exe outputs", exist_ok=True)
c_fails = set()
exe_fails = set()
missing_expect = set()
expect_fail = set()
expect_success = set()

for i in range(n_examples):
    ## compile source to c
    with open(f"c results/ex{i}.c", "w") as of:
        subprocess.run(["../compiler4", f"../examples/ex{i}.src"], stdout=of) # TODO: error check on this
    ## compile c to exe
    try:
        subprocess.run(["gcc", f"c results/ex{i}.c", "-o", f"exes/ex{i}"], check=True)
    except:
        print(f"compiling ex{i} failed")
        c_fails.add(i)
        continue
    ## run exe
    with open(f"exe outputs/ex{i} output", "w") as of:
        print(f"running ex{i}")
        try:
            subprocess.run([f"exes/ex{i}"], check=True, stdout=of)
        except:
            exe_fails.add(i)
            continue
    ##compare output
    try:
        if filecmp.cmp(f"exe outputs/ex{i} output", f"expects/ex{i} expect"):
            expect_success.add(i)
        else:
            expect_fail.add(i)
    except:
        missing_expect.add(i)

print(f"failed to compile to c: {c_fails}")
print(f"exe compiled but exe returned error: {exe_fails}")
print(f"exe ran successfully but is missing expect: {missing_expect}")
print(f"exe output did not match expect: {expect_fail}")
print(f"exe output matched expect: {expect_success}")
