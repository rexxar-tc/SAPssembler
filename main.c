#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define LDA 0b0000
#define ADD 0b0001
#define SUB 0b0010
#define OUT 0b1110
#define HLT 0b1111

typedef uint8_t byte;

int parseHelp(char *);

byte parseLine(char *);

void printHelp() {
    printf("SAP-1 assembler\n");
    printf("Usage: saps input.asm output.bin\n");
}


int main(int argc, char *argv[]) {
    if (argc == 2) {
        if (parseHelp(argv[1])) {
            printHelp();
            return 0;
        }

        FILE *fin;
        fin = fopen(argv[1], "r");
        fseek(fin, 0, SEEK_END);
        long flen = ftell(fin);
        fseek(fin, 0, SEEK_SET);
        char ln[256];
        byte buf[256];
        long line = 0;
        long oPos = 0;
        while (fgets(ln, 255, fin)) {
            line++;
            if (ln[0] == ';') continue; //comments are a semicolon at the start of a line
            char op[3] = {ln[0], ln[1], ln[2]};
            byte curOp = 0;
            if (strcmp(op, "LDA") == 0) {
                curOp = LDA << 4;
            } else if (strcmp(op, "ADD") == 0) {
                curOp = ADD << 4;
            } else if (strcmp(op, "SUB") == 0) {
                curOp = SUB << 4;
            } else if (strcmp(op, "OUT") == 0) {
                curOp = OUT << 4;
            } else if (strcmp(op, "HLT") == 0) {
                curOp = HLT << 4;
            } else {
                printf("Invalid opcode at line %ld", line);
                return 0;
            }
            for (int i = 3; i < 255; i++) {
                char c = ln[i];
                if (c == '\0' || c == '\n')
                    break;

                if ((c >= '0' && c <= '9'))
                    curOp |= c - '0';
                else if (c >= 'A' && c <= 'F')
                    curOp |= c - 'A' + 10;
                else
                    continue;
                break;
            }
            buf[oPos] = curOp;
            oPos++;
        }

        fclose(fin);

        FILE *fout;
        fopen("output.bin", "wb");
        fwrite(buf, 1, oPos, fout);
        fflush(fout);
        fclose(fout);

    } else
        printHelp();
    printf("ok");
    return 0;
}

int parseHelp(char *v) {
    int len = strlen(v);
    if (len <= 2) {
        char c = v[len - 1]; //last non-null char
        if (c == 'h' || c == 'H')
            return 1;
    }
    if (len < 5)  //5 to leave a leading character for / or - etc TODO
        return 0;

    //look for 'help' at the end of the string
    if (v[len - 1] == 'p' || v[len - 1] == 'P')
        if (v[len - 2] == 'l' || v[len - 2] == 'L')
            if (v[len - 3] == 'e' || v[len - 3] == 'E')
                if (v[len - 4] == 'h' || v[len - 4] == 'H')
                    return 1;

    return 0;
}
