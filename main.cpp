#include <iostream>
#include <sstream>
#include <limits>
#include <fstream>


#define BUFFER_SIZE (8192*8)

#define BARCODE_LENGTH 16
unsigned int cbc_length;

class Node {
private: Node *A= nullptr, *C= nullptr, *T= nullptr, *G = nullptr;
    unsigned int count = 0;
public:
    Node* get_child(char base){
        switch(base){
            case 'A':
                if (A == nullptr)
                    A = new Node();
                return A;
            case 'C':
                if (C == nullptr)
                    C = new Node();
                return C;
            case 'G':
                if (G == nullptr)
                    G = new Node();
                return G;
            case 'T':
                if (T == nullptr)
                    T = new Node();
                return T;
            default:
                return nullptr;
        }
    }
    void increment_count() {
        count++;
    }
    void print(unsigned int level){
        if (level == 0)
            std::cout << count << "\n";
        else{
            level--;
            if (A!=nullptr)
                A->print(level);
            if (C!=nullptr)
                C->print(level);
            if (G!=nullptr)
                G->print(level);
            if (T!=nullptr)
                T->print(level);
        }

    }
};

Node root = Node();

int parse_line(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &size){
    while (true) {
        for (; i<size; ) {
            if (buffer[i++]=='\n')
                return EXIT_SUCCESS;
        }
        if (0>=(size = fread(buffer, 1, sizeof(buffer), stdin))) {
            std::cerr << "premature end of file";
            return EXIT_FAILURE;
        }
        i = 0;
    }
}


int parse_plus_line(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &size){
    if (i>=size){
        if(0 >= (size = fread(buffer, 1, sizeof(buffer), stdin)))
            return EXIT_FAILURE;
        i = 0;
    }
    if (buffer[i++]!='+'){
        std::cerr << "line does not start  with '+'" << std::endl;
        return EXIT_FAILURE;
    }
    return parse_line(buffer, i, size);
}


int parse_barcode(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &size){
    Node *ptr = &root;
    ptr->increment_count();
    unsigned int j = cbc_length;
    while (true) {
        for (; i<size; i++) {
            ptr = ptr->get_child(buffer[i]);
            if (ptr == nullptr) {
                if(buffer[i]=='N')
                    return parse_line(buffer, i, size);
                else {
                    std::cerr << "barcode contains: " << buffer[i] << std::endl;
                    return EXIT_FAILURE;
                }
            }
            ptr->increment_count();
            j--;
            if (j == 0) {
                return parse_line(buffer, i, size);
            }
        }
        i = 0;
        if (0>=(size = fread(buffer, 1, sizeof(buffer), stdin))) {
            std::cerr << "premature end of file";
            return EXIT_FAILURE;
        }
    }
}


int parse_at_line(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &size){
    if (i>=size){
        i = 0;
        if(0 >= (size = fread(buffer, 1, sizeof(buffer), stdin)))
            return EXIT_FAILURE;
    }
    if (buffer[i++]!='@'){
        std::cerr << "line does not start  with '@'" << std::endl;
        return EXIT_FAILURE;
    }
    return parse_line(buffer, i, size);
}

int parse_eof(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &size){
    if (i>=size){
        i = 0;
        if(0 >= (size = fread(buffer, 1, sizeof(buffer), stdin)))
            return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int main() {
    /*std::istringstream debug_in("@NB551291:5:H2KCYBGX7:1:11101:7613:1054 1:N:0:1\n"
                                "ATGCGNTCAGACGCTCTGCTAGTCCT\n"
                                "+\n"
                                "6AAAA#EEAEEEEEEEEEAEEEEEEE\n"
                                "@NB551291:5:H2KCYBGX7:1:11101:4427:1054 1:N:0:1\n"
                                "AGCTTNACAATGGAGCGACCGCAGTA\n"
                                "+\n"
                                "AAAAA#EEEEEEEEEEEEEEEEEEEE\n");*/
//std::cin.rdbuf(debug_in.rdbuf());
#ifndef NDEBUG
    FILE * tmp = freopen("debugInput.txt","r", stdin);
    if (tmp == nullptr)
        return EXIT_FAILURE;
#endif
#ifdef FALSE
    FILE * tmp_out = freopen("debugOutput.txt","w", stdout);
    if (tmp_out == nullptr)
        return EXIT_FAILURE;
#endif
    cbc_length = 16;
    char buffer[BUFFER_SIZE];
    size_t size = 0;
    size_t i = 0;
    int ret = EXIT_SUCCESS;
    while (parse_eof(buffer, i , size)==EXIT_FAILURE) {
        ret = parse_at_line(buffer, i, size) ||
              parse_barcode(buffer, i, size) ||
              parse_plus_line(buffer, i, size) ||
              parse_line(buffer, i, size);
    }
    root.print(cbc_length);
    return ret;
}