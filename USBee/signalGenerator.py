import struct

def fill_buffer_freq(filename, size_in_gb, freq):
    # 1GB = 1024 * 1024 * 1024 bytes
    size = size_in_gb * 1024 * 1024 * 1024 // 4  # Each u32 is 4 bytes, so divide by 4
    
    t = freq / 4800 * 2  # Scaling factor based on frequency

    x = 0x00000000  # Initialize x
    with open(filename, "wb") as f:
        for i in range(size * 8):
            x = x << 1  # Shift left

            # Toggle bit based on frequency and index
            if int(i * t) % 2 == 0:
                x += 1

            # Every 32 iterations, store the value into the file
            if (i % 32) == 31:
                f.write(struct.pack("<I", x))  # Store as little-endian 32-bit integer
                x = 0x00000000  # Reset x

    print(f"1GB binary file '{filename}' created successfully.")

# Example usage:
fill_buffer_freq("output.bin", 1, 4600)  # Generate 1GB file with 4 600 x 100 Hz frequency
