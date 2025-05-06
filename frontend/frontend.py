import subprocess

# Function to call the C++ executable
def call_add_numbers(num1, num2):
    # Path to the compiled C++ executable
    exe_path = "C:\\path_to_your_exe\\add_numbers.exe"  # Update this path with where your executable is located
    
    # Call the executable with the two numbers as arguments
    result = subprocess.run([exe_path, str(num1), str(num2)], capture_output=True, text=True)

    # Get and return the output from the executable
    return result.stdout

# Frontend: Ask user for input
def main():
    print("Enter two numbers to add:")
    num1 = int(input("Enter first number: "))
    num2 = int(input("Enter second number: "))

    # Call the backend and get the result
    result = call_add_numbers(num1, num2)

    # Display the result
    print(f"The result from the backend is: {result}")

if __name__ == "__main__":
    main()
