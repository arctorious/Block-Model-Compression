# Read lines from the input text file
input_file_name = "input.txt"
with open(input_file_name, "r") as f:
    lines = f.readlines()

# Remove trailing whitespaces and newlines
lines = [line.strip() for line in lines]

# Generate C++ code
cpp_code = 'std::string s[] = {' + ','.join(f'"{line}"' for line in lines) + '};'

# Write the C++ code to the output text file
output_file_name = "output.txt"
with open(output_file_name, "w") as f:
    f.write(cpp_code)
print(lines.__len__())
print(f"Generated C++ code written to {output_file_name}")