#include <stdio.h>
#include <string.h>

void reverseString(char *str, int start, int end) {
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void reverseByDot(char *str) {
    int len = strlen(str);
    int start = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == '.') {
            reverseString(str, start, i - 1);
            start = i + 1;
        }
    }
    reverseString(str, start, len - 1);
    reverseString(str, 0, len - 1);
}

int main() {
    char str[] = "19.128.172.admin@185";
    printf("Original string: %s\n", str);
    
    reverseByDot(str);
    
    printf("Reversed string: %s\n", str);
    
    return 0;
}
