 #include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define FILENAME "data.txt"
#define KEY 0x5A

const char *EMOJI_0[] = {"🙂", "😀", "😃"};
const char *EMOJI_1[] = {"😌", "😉", "😁"};

void xor_encrypt_decrypt(char *message) {
    for (int i = 0; message[i] != '\0'; i++) {
        message[i] ^= KEY;
    }
}

void message_to_binary(char *message, char *binary_message) {
    int index = 0;
    for (int i = 0; message[i] != '\0'; i++) {
        for (int j = 7; j >= 0; j--) {
            binary_message[index++] = (message[i] & (1 << j)) ? '1' : '0';
        }
    }
    binary_message[index] = '\0';
}

void binary_to_emojis(char *binary_message) {
    srand(time(NULL));
    FILE *file = fopen(FILENAME, "w");
    if (!file) return;

    for (int i = 0; binary_message[i] != '\0'; i++) {
        fprintf(file, "%s", binary_message[i] == '0' ? EMOJI_0[rand() % 3] : EMOJI_1[rand() % 3]);
    }
    fclose(file);
}

void binary_to_text(char *binary_message, char *decoded_message) {
    int index = 0;
    for (int i = 0; binary_message[i] != '\0'; i += 8) {
        char byte = 0;
        for (int j = 0; j < 8; j++) {
            if (binary_message[i + j] == '1') {
                byte |= (1 << (7 - j));
            }
        }
        decoded_message[index++] = byte;
    }
    decoded_message[index] = '\0';
}

void modify_file() {
    char new_message[200];
    printf("Enter new message: ");
    fgets(new_message, sizeof(new_message), stdin);
    new_message[strcspn(new_message, "\n")] = '\0';

    xor_encrypt_decrypt(new_message);
    char binary_message[8 * strlen(new_message) + 1];
    message_to_binary(new_message, binary_message);
    binary_to_emojis(binary_message);
}

void read_file() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return;

    char encoded_message[1000];
    fgets(encoded_message, sizeof(encoded_message), file);
    fclose(file);
    printf("\nEncoded Message: %s\n", encoded_message);
}

void decode_file() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return;

    char encoded_message[1000];
    fgets(encoded_message, sizeof(encoded_message), file);
    fclose(file);

    char binary_message[800];
    int index = 0;
    for (int i = 0; encoded_message[i] != '\0'; i++) {
        binary_message[index++] = (encoded_message[i] == '🙂' || encoded_message[i] == '😀' || encoded_message[i] == '😃') ? '0' : '1';
    }
    binary_message[index] = '\0';

    char decoded_message[200];
    binary_to_text(binary_message, decoded_message);
    xor_encrypt_decrypt(decoded_message);
    printf("\nDecoded Message: %s\n", decoded_message);
    FILE *output = fopen("decoded_output.txt", "w");
    if (output) {
        fprintf(output, "%s", decoded_message);
        fclose(output);
    }
}

void check_file_size() {
    FILE *file = fopen(FILENAME, "r");
    if (!file) return;
    fseek(file, 0, SEEK_END);
    printf("\nFile Size: %ld bytes\n", ftell(file));
    fclose(file);
}

void delete_encoded_file() {
    if (remove(FILENAME) == 0) {
        printf("\nFile deleted successfully.\n");
    }
}

int main() {
    int choice;
    while (1) {
        printf("\n===== Emoji Steganography =====\n");
        printf("1. Modify file content\n");
        printf("2. Read encoded file content\n");
        printf("3. Decode and retrieve message\n");
        printf("4. Check file size\n");
        printf("5. Delete encoded file\n");
        printf("6. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: modify_file(); break;
            case 2: read_file(); break;
            case 3: decode_file(); break;
            case 4: check_file_size(); break;
            case 5: delete_encoded_file(); break;
            case 6: return 0;
        }
    }
}
