import sys

if len(sys.argv) != 2:
    print("Incorrect number of arguments")
    sys.exit(1)

filepath = sys.argv[1]
messages = []

with open(filepath, "rb") as file:
    while length := file.read(2):
        length = int.from_bytes(length, "big")
        messages.append(file.read(length))

        
        
        
    