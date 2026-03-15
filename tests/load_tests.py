import subprocess
import os
import platform

failed = 0
amount_of_files = 0

def run_test(input_file, expected_file):
    exe = "./tier" if platform.system() != "Windows" else "tier.exe"

    result = subprocess.run(
        [exe, input_file],
        capture_output=True,
        text=True
    )

    with open(expected_file, "r") as f:
        expected = f.read()
    
    if result.stdout.strip() == expected.strip():
        print(f"{input_file} PASSED.")
    else:
        failed += 1;
        print(f"{input_file} FAILED.")

for file in os.listdir("tests/lexer/"):
    if file.endswith(".tier"):
        amount_of_files += 1;
        name = file.removesuffix(".tier")
        run_test(f"tests/lexer/{name}.tier", f"tests/lexer/{name}.expected")

print(f"{amount_of_files-failed}/{amount_of_files} files passed.")