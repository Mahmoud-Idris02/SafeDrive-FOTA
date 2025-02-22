#include "AES_DEC.h"

#define Nb 4        // Number of columns (32-bit words) comprising the State. For AES, Nb is always 4.
#define Nr 10       // Number of rounds, which is 10 for AES-128.
#define Nk 4        // Number of 32-bit words comprising the Cipher Key.

static uint32_t RoundKey[Nb * (Nr + 1)];
static uint8_t key[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x61,0x62,0x63,0x64,0x65,0x66};
static const uint8_t  RoundConstants[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};

/*Inverse MixCol Matrix used in decryption*/
uint8_t InvMixColMatrix[4][4] = {
    {0x0e, 0x0b, 0x0d, 0x09},
    {0x09, 0x0e, 0x0b, 0x0d},
    {0x0d, 0x09, 0x0e, 0x0b},
    {0x0b, 0x0d, 0x09, 0x0e}
};

/*S_box table used in encryption process*/
static const uint8_t  SBOX[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};
	
/* Inv S_box table used in Decryption process*/
static const uint8_t  INV_SBOX[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};


uint32_t SubWord(uint32_t word) {
    return (SBOX[(word >> 24) & 0xFF] << 24) |
           (SBOX[(word >> 16) & 0xFF] << 16) |
           (SBOX[(word >> 8) & 0xFF] << 8) |
           (SBOX[word & 0xFF]);
}

uint32_t RotWord(uint32_t word) {
    return (word << 8) | (word >> 24);
}

void KeyExpansion(uint8_t* key) {
    uint32_t temp;
    int i = 0;

    // Load the key into the first 4 words
    while (i < Nk) {
        RoundKey[i] = (uint32_t)key[4 * i] << 24 |
                      (uint32_t)key[4 * i + 1] << 16 |
                      (uint32_t)key[4 * i + 2] << 8 |
                      (uint32_t)key[4 * i + 3];
        i++;
    }


    // Expand the key
    while (i < Nb * (Nr + 1)) {
        temp = RoundKey[i - 1];
        if (i % Nk == 0) {
            temp = SubWord(RotWord(temp)) ^ (RoundConstants[i/Nk - 1] << 24);  
        }
        RoundKey[i] = RoundKey[i - Nk] ^ temp;
        i++;
    }
}
void AES_init_ctx_dec(uint8_t* key) {
    KeyExpansion(key);
}

static void InvSubBytes(uint8_t* state) {
    for (int i = 0; i < 16; i++) {
        state[i] = INV_SBOX[state[i]];
    }
}

static void InvShiftRows(uint8_t* state) {
    uint8_t temp;
    // Row 1
    temp = state[13];
    state[13] = state[9];
    state[9] = state[5];
    state[5] = state[1];
    state[1] = temp;
    // Row 2
    temp = state[2];
    state[2] = state[10];
    state[10] = temp;
    temp = state[6];
    state[6] = state[14];
    state[14] = temp;
    // Row 3
    temp = state[3];
    state[3] = state[7];
    state[7] = state[11];
    state[11] = state[15];
    state[15] = temp;
}

// Function to reverse bytes within each 32-bit word of the RoundKey array
void ReverseBytesInWords(void) {
    for (int i = 0; i < Nb * (Nr + 1); i++) {
        uint32_t word = RoundKey[i];
        RoundKey[i] = ((word >> 24) & 0xFF) |       
                      ((word << 8) & 0xFF0000) |   
                      ((word >> 8) & 0xFF00) |     
                      ((word << 24) & 0xFF000000); 
    }
}

// Function to multiply by 2 in GF(2^8)
uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1b : 0x00);
}

// Function to multiply in GF(2^8)
uint8_t multiply(uint8_t x, uint8_t y) {
    return (((y & 1) * x) ^
            ((y>>1 & 1) * xtime(x)) ^
            ((y>>2 & 1) * xtime(xtime(x))) ^
            ((y>>3 & 1) * xtime(xtime(xtime(x)))));
}

void InvMixCol(uint8_t *state) {
    uint8_t temp[16];
    for (int i = 0; i < 4; i++) { // Iterate over each column
        temp[4 * i + 0] = multiply(state[4 * i + 0], 0x0e) ^
                          multiply(state[4 * i + 1], 0x0b) ^
                          multiply(state[4 * i + 2], 0x0d) ^
                          multiply(state[4 * i + 3], 0x09);
        temp[4 * i + 1] = multiply(state[4 * i + 0], 0x09) ^
                          multiply(state[4 * i + 1], 0x0e) ^
                          multiply(state[4 * i + 2], 0x0b) ^
                          multiply(state[4 * i + 3], 0x0d);
        temp[4 * i + 2] = multiply(state[4 * i + 0], 0x0d) ^
                          multiply(state[4 * i + 1], 0x09) ^
                          multiply(state[4 * i + 2], 0x0e) ^
                          multiply(state[4 * i + 3], 0x0b);
        temp[4 * i + 3] = multiply(state[4 * i + 0], 0x0b) ^
                          multiply(state[4 * i + 1], 0x0d) ^
                          multiply(state[4 * i + 2], 0x09) ^
                          multiply(state[4 * i + 3], 0x0e);
    }

    // Copy the new column values back to the state
    for (int j = 0; j < 16; j++) {
        state[j] = temp[j];
    }
}

// Function to add the round key to the state
void AddRoundKey(uint8_t* state, int round) {
    int i;
    uint8_t* roundKeyBytes = (uint8_t*)(RoundKey + round * Nb);  // Calculate the pointer to the round key

    for (i = 0; i < 16; i++) {  // Each state is 16 bytes
        state[i] ^= roundKeyBytes[i];

    }
}

void AES_decrypt_block(uint8_t* ciphertext, uint8_t* plaintext) {
    uint8_t state[16];
    memcpy(state, ciphertext, 16);

    AddRoundKey(state, 10);
    for (int round = Nr - 1; round >= 1; round--) {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(state, round);
        InvMixCol(state);
    }

    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state, 0);

    memcpy(plaintext, state, 16);
}


int AES_decrypt_file(const char* input_file_path, const char* output_file_path) {
    FILE *file = fopen(input_file_path, "rb");
    FILE *outFile = fopen(output_file_path, "wb");
    if (!file || !outFile) {
        if (file) fclose(file);
        if (outFile) fclose(outFile);
        return 1; // Error opening file
    }

    uint8_t ciphertext[16], plaintext[16];

    while (fread(ciphertext, sizeof(uint8_t), 16, file) == 16) {
        AES_decrypt_block(ciphertext, plaintext);
        fwrite(plaintext, sizeof(uint8_t), 16, outFile);
    }

    fclose(file);
    fclose(outFile);
    return 0; // Success
}



int read_file_to_buffer(const char* filename, uint8_t** buffer, size_t* size) {
    FILE* file;
    long file_size;
    size_t result;

    // Open file
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    // Allocate memory for the entire file
    *buffer = (uint8_t*) malloc(file_size * sizeof(uint8_t));
    if (*buffer == NULL) {
        perror("Memory error");
        fclose(file);
        return 2;
    }


    // Copy the file into the buffer
    result = fread(*buffer, 1, file_size, file);
    if (result != file_size) {
        perror("Reading error");
        free(*buffer);
        fclose(file);
        return 3;
    }

    // Close file and return
    fclose(file);
    *size = file_size;
    return 0;
}


int AES_decrypt_buffer(const uint8_t* input_buffer, size_t input_size, uint8_t* output_buffer) {
	  uint8_t Dectyption_Status = DEC_NOT_VERIFIED;
    if (!input_buffer || !output_buffer) {Dectyption_Status = DEC_NOT_VERIFIED;}; // Error in input/output buffer
		AES_init_ctx_dec(key);
		ReverseBytesInWords();
    uint8_t ciphertext[16], plaintext[16];
    size_t processed_bytes = 0;
		
    // Decrypt block by block
    while (processed_bytes + 16 <= input_size) {
        memcpy(ciphertext, input_buffer + processed_bytes, 16);
        AES_decrypt_block(ciphertext, plaintext);
        memcpy(output_buffer + processed_bytes, plaintext, 16);
        processed_bytes += 16;
				Dectyption_Status = DEC_VERIFIED;
    }

    return Dectyption_Status; // Success
}


int write_buffer_to_file(const char* filename, const uint8_t* data, size_t size) {
    FILE* file;

    // Open file
    file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Write data
    if (fwrite(data, 1, size, file) != size) {
        perror("Failed to write data");
        fclose(file);
        return 2;
    }

    // Close file
    fclose(file);
    return 0;
}


