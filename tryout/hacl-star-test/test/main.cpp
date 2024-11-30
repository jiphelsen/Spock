#include "Hacl_Hash_Blake2b.h"
#include <stdio.h>
#include <string.h>

void print_hex_ln(size_t bytes_len, uint8_t *bytes)
{
    for (size_t i = 0; i < bytes_len; ++i)
    {
        printf("%02x", bytes[i]);
    }
    printf("\n");
}

int main(int argc, char const *argv[])
{
    uint32_t output_len = 64; // BLAKE2b 512-bit output
    uint8_t output[64];

    const char *message = "Hello, HACL Packages!";
    uint32_t message_len = strlen(message);

    uint32_t key_len = 0; // No key for this example
    uint8_t *key = NULL;

    // Hashing
    Hacl_Hash_Blake2b_hash_with_key(
        output, output_len, (uint8_t *)message, message_len, key, key_len);

    // Print the hashed output
    print_hex_ln(output_len, output);

    return 0;
}
