#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int close_checked(FILE *stream, const char *name) {
    if (fclose(stream) != 0) {
        fprintf(stderr, "Не удалось закрыть %s: %s\n", name, strerror(errno));
        return 1;
    }
    return 0;
}

static int valid_identifier(const char *value) {
    if (value == NULL || (*value != '_' && !isalpha((unsigned char)*value))) {
        return 0;
    }
    for (++value; *value != '\0'; ++value) {
        if (*value != '_' && !isalnum((unsigned char)*value)) {
            return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv) {
    unsigned char buffer[4096];
    size_t total = 0;
    size_t column = 0;
    FILE *input;
    FILE *output;

    const char *symbol = argc == 4 ? argv[3] : "j2s_code2bin_bin";

    if ((argc != 3 && argc != 4) || !valid_identifier(symbol)) {
        fprintf(stderr, "Использование: %s INPUT OUTPUT [SYMBOL]\n", argv[0]);
        return 2;
    }

    input = fopen(argv[1], "rb");
    if (input == NULL) {
        fprintf(stderr, "Не удалось открыть %s: %s\n", argv[1], strerror(errno));
        return 1;
    }
    output = fopen(argv[2], "wb");
    if (output == NULL) {
        fprintf(stderr, "Не удалось открыть %s: %s\n", argv[2], strerror(errno));
        fclose(input);
        return 1;
    }

    if (fprintf(output, "unsigned char %s[] = {\n  ", symbol) < 0) {
        fprintf(stderr, "Ошибка записи %s\n", argv[2]);
        fclose(input);
        fclose(output);
        return 1;
    }

    for (;;) {
        const size_t count = fread(buffer, 1, sizeof(buffer), input);
        for (size_t index = 0; index < count; ++index) {
            if (total != 0) {
                if (fprintf(output, ", ") < 0) {
                    fprintf(stderr, "Ошибка записи %s\n", argv[2]);
                    fclose(input);
                    fclose(output);
                    return 1;
                }
            }
            if (column == 12) {
                if (fprintf(output, "\n  ") < 0) {
                    fprintf(stderr, "Ошибка записи %s\n", argv[2]);
                    fclose(input);
                    fclose(output);
                    return 1;
                }
                column = 0;
            }
            if (fprintf(output, "0x%02x", buffer[index]) < 0) {
                fprintf(stderr, "Ошибка записи %s\n", argv[2]);
                fclose(input);
                fclose(output);
                return 1;
            }
            ++total;
            ++column;
        }
        if (count < sizeof(buffer)) {
            if (ferror(input)) {
                fprintf(stderr, "Ошибка чтения %s: %s\n", argv[1], strerror(errno));
                fclose(input);
                fclose(output);
                return 1;
            }
            break;
        }
    }

    if (fprintf(output, "\n};\nunsigned int %s_len = %zu;\n", symbol, total) < 0) {
        fprintf(stderr, "Ошибка записи %s\n", argv[2]);
        fclose(input);
        fclose(output);
        return 1;
    }

    if (close_checked(input, argv[1]) != 0) {
        fclose(output);
        return 1;
    }
    return close_checked(output, argv[2]);
}
