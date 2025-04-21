#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ENCODED_FILE "C:\\emoji_encoded.txt"
#define DECODED_OUTPUT "C:\\decoded_output.bin"
#define KEY 0x5A

const char *EMOJI_0[] = {"🙂", "😀", "😃"};
const char *EMOJI_1[] = {"😌", "😉", "😁"};
const int EMOJI_SET_SIZE = 3;

long get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return -1;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fclose(file);
    return size;
}

void byte_to_binary(unsigned char byte, char *binary) {
    for (int i = 7; i >= 0; i--) {
        binary[7 - i] = (byte & (1 << i)) ? '1' : '0';
    }
    binary[8] = '\0';
}

unsigned char binary_to_byte(const char *binary) {
    unsigned char byte = 0;
    for (int i = 0; i < 8; i++) {
        if (binary[i] == '1') {
            byte |= (1 << (7 - i));
        }
    }
    return byte;
}

int is_emoji(const char *ch, const char **emoji_set) {
    for (int i = 0; i < EMOJI_SET_SIZE; i++) {
        if (strcmp(ch, emoji_set[i]) == 0) return 1;
    }
    return 0;
}

int get_utf8_length(unsigned char c) {
    if ((c & 0xF8) == 0xF0) return 4; 
    if ((c & 0xF0) == 0xE0) return 3; 
    if ((c & 0xE0) == 0xC0) return 2; 
    return 1;                         
}


void encode_file(const char *filename) {
    FILE *in = fopen(filename, "rb");
    if (!in) {
        printf("Error opening input file!\n");
        return;
    }

    FILE *out = fopen(ENCODED_FILE, "w, ccs=UTF-8");  
    if (!out) {
        printf("Error creating encoded file!\n");
        fclose(in);
        return;
    }

    long original_size = get_file_size(filename);
    printf("Original File Size: %ld bytes\n", original_size);

    srand(time(NULL));
    unsigned char byte;
    char binary[9];

    while (fread(&byte, 1, 1, in)) {
        byte ^= KEY;
        byte_to_binary(byte, binary);

        for (int i = 0; i < 8; i++) {
            const char *emoji = (binary[i] == '0') ? EMOJI_0[rand() % EMOJI_SET_SIZE] : EMOJI_1[rand() % EMOJI_SET_SIZE];
            fprintf(out, "%s", emoji);
        }
    }

    fclose(in);
    fclose(out);

    long encoded_size = get_file_size(ENCODED_FILE);
    printf("Encoded File Saved as '%s'\n", ENCODED_FILE);
    printf("Encoded File Size: %ld bytes\n", encoded_size);
}

void decode_file() {
    FILE *in = fopen(ENCODED_FILE, "rb");
    if (!in) {
        printf("Encoded file not found!\n");
        return;
    }

    FILE *out = fopen(DECODED_OUTPUT, "wb");
    if (!out) {
        printf("Error creating decoded output file!\n");
        fclose(in);
        return;
    }

    fseek(in, 0, SEEK_END);
    long file_size = ftell(in);
    rewind(in);

    char *buffer = malloc(file_size + 1);
    fread(buffer, 1, file_size, in);
    buffer[file_size] = '\0';
    fclose(in);

    char binary[9];
    int bin_index = 0;
    char emoji[5];
    int i = 0;

    while (i < file_size) {
        int len = get_utf8_length((unsigned char)buffer[i]);

        if (i + len > file_size) break; 

        strncpy(emoji, &buffer[i], len);
        emoji[len] = '\0';
        i += len;

        if (is_emoji(emoji, EMOJI_0)) {
            binary[bin_index++] = '0';
        } else if (is_emoji(emoji, EMOJI_1)) {
            binary[bin_index++] = '1';
        } else {
            printf("Unknown emoji encountered: %s\n", emoji);
            continue;
        }

        if (bin_index == 8) {
            binary[8] = '\0';
            unsigned char byte = binary_to_byte(binary);
            byte ^= KEY;
            fwrite(&byte, 1, 1, out);
            bin_index = 0;
        }
    }

    fclose(out);
    free(buffer);

    long decoded_size = get_file_size(DECODED_OUTPUT);
    printf("Decoded output saved in '%s'\n", DECODED_OUTPUT);
    printf("Decoded File Size: %ld bytes\n", decoded_size);
}


int main() {
    int choice;
    char filename[256];

    while (1) {
        printf("\n==== Universal File Encoder with Emoji Steganography ====\n");
        printf("1. Encode any file with emojis\n");
        printf("2. Decode emoji file back to binary\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();  

        switch (choice) {
            case 1:
                printf("Enter file name to encode: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = '\0';
                encode_file(filename);
                break;
            case 2:
                decode_file();
                break;
            case 3:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
}
