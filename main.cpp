#include <iostream>
#include <sstream>
#include <limits>
#include <fstream>


#define BUFFER_SIZE (8192*8)

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
    void print(unsigned int depth_to_be_printed){
        if (depth_to_be_printed == 0)
            std::cout << count << "\n";
        else{
            depth_to_be_printed--;
            if (A!=nullptr)
                A->print(depth_to_be_printed);
            if (C!=nullptr)
                C->print(depth_to_be_printed);
            if (G!=nullptr)
                G->print(depth_to_be_printed);
            if (T!=nullptr)
                T->print(depth_to_be_printed);
        }
    }
};

Node root = Node();

int parse_line(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &valid_size){
    while (true) {
        for (; i<valid_size; ) {
            if (buffer[i++]=='\n')
                return EXIT_SUCCESS;
        }
        if (0>=(valid_size = fread(buffer, 1, sizeof(buffer), stdin))) {
            std::cerr << "premature end of file";
            return EXIT_FAILURE;
        }
        i = 0;
    }
}


int parse_plus_line(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &valid_size){
    if (i>=valid_size){
        if(0 >= (valid_size = fread(buffer, 1, sizeof(buffer), stdin)))
            return EXIT_FAILURE;
        i = 0;
    }
    if (buffer[i++]!='+'){
        std::cerr << "line does not start  with '+'" << std::endl;
        return EXIT_FAILURE;
    }
    return parse_line(buffer, i, valid_size);
}


int parse_barcode_and_add_to_tree(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &valid_size){
    Node *ptr = &root;
    ptr->increment_count();
    unsigned int j = cbc_length;
    while (true) {
        for (; i<valid_size; i++) {
            ptr = ptr->get_child(buffer[i]);
            if (ptr == nullptr) {
                if(buffer[i]=='N')
                    return parse_line(buffer, i, valid_size);
                else {
                    std::cerr << "barcode contains: " << buffer[i] << std::endl;
                    return EXIT_FAILURE;
                }
            }
            ptr->increment_count();
            j--;
            if (j == 0) {
                return parse_line(buffer, i, valid_size);
            }
        }
        i = 0;
        if (0>=(valid_size = fread(buffer, 1, sizeof(buffer), stdin))) {
            std::cerr << "premature end of file";
            return EXIT_FAILURE;
        }
    }
}


int parse_at_line(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &valid_size){
    if (i>=valid_size){
        i = 0;
        if(0 >= (valid_size = fread(buffer, 1, sizeof(buffer), stdin)))
            return EXIT_FAILURE;
    }
    if (buffer[i++]!='@'){
        std::cerr << "line does not start  with '@'" << std::endl;
        return EXIT_FAILURE;
    }
    return parse_line(buffer, i, valid_size);
}

int parse_eof(char (&buffer)[BUFFER_SIZE], size_t &i, size_t &valid_size){
    if (i>=valid_size){
        i = 0;
        if(0 >= (valid_size = fread(buffer, 1, sizeof(buffer), stdin)))
            return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

int main() {
    cbc_length = 16;
    char buffer[BUFFER_SIZE];
    size_t valid_size = 0;  // valid_size of the valid part of the buffer
    size_t i = 0; // courser (current postiion in buffer)
    int ret = EXIT_SUCCESS;
    while (parse_eof(buffer, i , valid_size)==EXIT_FAILURE) {
        ret = parse_at_line(buffer, i, valid_size) ||                 // @ID....
              parse_barcode_and_add_to_tree(buffer, i, valid_size) || // ATTCGC...
              parse_plus_line(buffer, i, valid_size) ||               // +
              parse_line(buffer, i, valid_size);                      // ///777788899 ....
    }
    root.print(cbc_length);
    return ret;
}