#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 1000
#define WORD_LEN 50
#define HASH_SIZE 1009
#define MAX_FILENAME_LEN 100
#define ALPHABET_SIZE 26

// Global variable for shingle size
int K_VALUE = 1;

// Hashmap structures
typedef struct HashNode {
    char shingle[256];
    struct HashNode *next;
} HashNode;

// Trie structures
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int isEndOfWord;
    int count;
} TrieNode;

// Hashmap functions
unsigned int hashFunc(const char *str) {
    unsigned int h = 0;
    while (*str)
        h = (h * 131) + (*str++);
    return h % HASH_SIZE;
}

void insertHash(HashNode **table, const char *str) {
    unsigned int h = hashFunc(str);
    HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
    if (newNode == NULL) {
        return;
    }
    strcpy(newNode->shingle, str);
    newNode->next = table[h];
    table[h] = newNode;
}

int searchHash(HashNode **table, const char *str) {
    unsigned int h = hashFunc(str);
    HashNode *cur = table[h];
    while (cur) {
        if (strcmp(cur->shingle, str) == 0)
            return 1;
        cur = cur->next;
    }
    return 0;
}

void freeHashTable(HashNode **table) {
    for (int i = 0; i < HASH_SIZE; i++) {
        HashNode *cur = table[i];
        while (cur) {
            HashNode *temp = cur;
            cur = cur->next;
            free(temp);
        }
    }
}

// Trie functions
TrieNode *createTrieNode() {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    if (node) {
        node->isEndOfWord = 0;
        node->count = 0;
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

void insertTrie(TrieNode *root, const char *word) {
    TrieNode *current = root;
    
    for (int i = 0; word[i]; i++) {
        int index = tolower(word[i]) - 'a';
        if (index < 0 || index >= ALPHABET_SIZE) continue;
        
        if (!current->children[index]) {
            current->children[index] = createTrieNode();
        }
        current = current->children[index];
    }
    
    current->isEndOfWord = 1;
    current->count++;
}

int searchTrie(TrieNode *root, const char *word) {
    TrieNode *current = root;
    
    for (int i = 0; word[i]; i++) {
        int index = tolower(word[i]) - 'a';
        if (index < 0 || index >= ALPHABET_SIZE) return 0;
        
        if (!current->children[index]) {
            return 0;
        }
        current = current->children[index];
    }
    
    return (current != NULL && current->isEndOfWord);
}

void freeTrie(TrieNode *root) {
    if (!root) return;
    
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (root->children[i]) {
            freeTrie(root->children[i]);
        }
    }
    free(root);
}

// File processing functions
int tokenizeFile(char words[][WORD_LEN], const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("  ERROR: Cannot open file %s\n", filename);
        return 0;
    }
    
    int count = 0;
    char word[WORD_LEN];
    
    while (fscanf(fp, "%49s", word) != EOF && count < MAX_WORDS) {
        int idx = 0;
        for (int i = 0; word[i] && idx < WORD_LEN - 1; i++) {
            if (isalnum((unsigned char)word[i])) {
                word[idx++] = tolower((unsigned char)word[i]);
            }
        }
        word[idx] = '\0';
        
        if (idx > 0) {
            strcpy(words[count], word);
            count++;
        }
    }
    
    fclose(fp);
    return count;
}

int formShingles(char words[][WORD_LEN], int wordCount, char shingles[][256]) {
    if (wordCount < K_VALUE) {
        return 0;
    }
    
    int shingleCount = 0;
    for (int i = 0; i <= wordCount - K_VALUE && shingleCount < MAX_WORDS; i++) {
        shingles[shingleCount][0] = '\0';
        
        for (int j = 0; j < K_VALUE; j++) {
            if (j > 0) strcat(shingles[shingleCount], " ");
            strcat(shingles[shingleCount], words[i + j]);
        }
        
        shingleCount++;
    }
    
    return shingleCount;
}

// Comparison using Hashmap
double compareFilesHashmap(const char *file1, const char *file2) {
    char words1[MAX_WORDS][WORD_LEN] = {0};
    char words2[MAX_WORDS][WORD_LEN] = {0};
    char shingles1[MAX_WORDS][256] = {0};
    char shingles2[MAX_WORDS][256] = {0};
    
    printf("Processing first file...\n");
    int wc1 = tokenizeFile(words1, file1);
    printf("  Starting to read words\n");
    printf("  Finished reading\n");
    printf("  Total words: %d\n", wc1);
    
    printf("Processing second file...\n");
    int wc2 = tokenizeFile(words2, file2);
    printf("  Starting to read words\n");
    printf("  Finished reading\n");
    printf("  Total words: %d\n", wc2);
    
    if (wc1 == 0 || wc2 == 0) {
        return 0.0;
    }
    
    printf("Creating %d-word shingles...\n", K_VALUE);
    int sc1 = formShingles(words1, wc1, shingles1);
    int sc2 = formShingles(words2, wc2, shingles2);
    
    if (sc1 == 0 || sc2 == 0) {
        printf("  Not enough words to form %d-word shingles\n", K_VALUE);
        return 0.0;
    }
    
    printf("Building HASHMAP...\n");
    HashNode *hashTable[HASH_SIZE] = {NULL};
    for (int i = 0; i < sc1; i++) {
        insertHash(hashTable, shingles1[i]);
    }
    
    printf("Counting HASHMAP matches...\n");
    int matches = 0;
    for (int i = 0; i < sc2; i++) {
        if (searchHash(hashTable, shingles2[i])) {
            matches++;
        }
    }
    
    double similarity = (double)matches / sc2 * 100.0;
    
    printf("\nHASHMAP COMPARISON RESULTS:\n");
    printf("  File 1 shingles: %d\n", sc1);
    printf("  File 2 shingles: %d\n", sc2);
    printf("  Matching shingles: %d\n", matches);
    printf("  Hashmap Similarity: %.2f%%\n", similarity);
    
    freeHashTable(hashTable);
    return similarity;
}

// Comparison using Trie
double compareFilesTrie(const char *file1, const char *file2) {
    char words1[MAX_WORDS][WORD_LEN] = {0};
    char words2[MAX_WORDS][WORD_LEN] = {0};
    char shingles1[MAX_WORDS][256] = {0};
    char shingles2[MAX_WORDS][256] = {0};
    
    printf("Processing first file...\n");
    int wc1 = tokenizeFile(words1, file1);
    printf("  Total words: %d\n", wc1);
    
    printf("Processing second file...\n");
    int wc2 = tokenizeFile(words2, file2);
    printf("  Total words: %d\n", wc2);
    
    if (wc1 == 0 || wc2 == 0) {
        return 0.0;
    }
    
    printf("Creating %d-word shingles...\n", K_VALUE);
    int sc1 = formShingles(words1, wc1, shingles1);
    int sc2 = formShingles(words2, wc2, shingles2);
    
    if (sc1 == 0 || sc2 == 0) {
        printf("  Not enough words to form %d-word shingles\n", K_VALUE);
        return 0.0;
    }
    
    printf("Building TRIE...\n");
    TrieNode *trieRoot = createTrieNode();
    for (int i = 0; i < sc1; i++) {
        insertTrie(trieRoot, shingles1[i]);
    }
    
    printf("Counting TRIE matches...\n");
    int matches = 0;
    for (int i = 0; i < sc2; i++) {
        if (searchTrie(trieRoot, shingles2[i])) {
            matches++;
        }
    }
    
    double similarity = (double)matches / sc2 * 100.0;
    
    printf("\nTRIE COMPARISON RESULTS:\n");
    printf("  File 1 shingles: %d\n", sc1);
    printf("  File 2 shingles: %d\n", sc2);
    printf("  Matching shingles: %d\n", matches);
    printf("  Trie Similarity: %.2f%%\n", similarity);
    
    freeTrie(trieRoot);
    return similarity;
}

char** getFilenames(int n, const char* prompt) {
    char **filenames = (char **)malloc(n * sizeof(char *));
    if (filenames == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    
    for (int i = 0; i < n; i++) {
        filenames[i] = (char *)malloc(MAX_FILENAME_LEN * sizeof(char));
        if (filenames[i] == NULL) {
            printf("Memory allocation failed for filename %d!\n", i + 1);
            for (int j = 0; j < i; j++) {
                free(filenames[j]);
            }
            free(filenames);
            return NULL;
        }
        
        printf("%s %d: ", prompt, i + 1);
        if (scanf("%99s", filenames[i]) != 1) {
            printf("Error reading input.\n");
            for (int j = 0; j <= i; j++) {
                free(filenames[j]);
            }
            free(filenames);
            return NULL;
        }
    }
    
    return filenames;
}

void freeFilenames(char **filenames, int n) {
    if (filenames) {
        for (int i = 0; i < n; i++) {
            free(filenames[i]);
        }
        free(filenames);
    }
}

void testFileAccess(char **filenames, int n, char *target) {
    printf("\nTesting file access...\n");
    
    FILE *test = fopen(target, "r");
    if (test) {
        printf("✓ %s can be opened\n", target);
        fclose(test);
    } else {
        printf("✗ Cannot open %s\n", target);
    }
    
    for (int i = 0; i < n; i++) {
        test = fopen(filenames[i], "r");
        if (test) {
            printf("✓ %s can be opened\n", filenames[i]);
            fclose(test);
        } else {
            printf("✗ Cannot open %s\n", filenames[i]);
        }
    }
}

int main() {
    printf("=== Plagiarism Detection System (Hashmap + Trie) ===\n");
    
    // Get shingle size from user
    printf("Enter shingle size (1-5, recommended: 3): ");
    if (scanf("%d", &K_VALUE) != 1 || K_VALUE < 1 || K_VALUE > 5) {
        printf("Invalid shingle size. Using default K=3.\n");
        K_VALUE = 3;
    }
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    printf("Using %d-word shingles for analysis.\n\n", K_VALUE);
    
    int n;
    printf("Enter number of files in corpus: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("Invalid number of files. Please enter a positive integer.\n");
        return 1;
    }
    
    while ((c = getchar()) != '\n' && c != EOF);
    
    char **corpus_files = getFilenames(n, "Enter filename");
    if (corpus_files == NULL) {
        return 1;
    }
    
    char target_file[MAX_FILENAME_LEN];
    printf("Enter the filename to compare: ");
    if (scanf("%99s", target_file) != 1) {
        printf("Error reading target filename.\n");
        freeFilenames(corpus_files, n);
        return 1;
    }
    
    testFileAccess(corpus_files, n, target_file);
    
    printf("\n--- Starting Plagiarism Analysis ---\n");
    printf("Target file: %s\n", target_file);
    printf("Corpus files: %d\n", n);
    printf("Shingle size: %d words\n", K_VALUE);
    
    for (int i = 0; i < n; i++) {
        printf("\n» Comparison %d of %d:\n", i + 1, n);
        printf("\n=== Starting comparison: %s vs %s ===\n", target_file, corpus_files[i]);
        
        // Using Hashmap
        double sim_hash = compareFilesHashmap(target_file, corpus_files[i]);
        
        // Using Trie
        double sim_trie = compareFilesTrie(target_file, corpus_files[i]);
        
        printf("=== End comparison ===\n");
        printf("»» FINAL RESULTS:\n");
        printf("    Hashmap: %.2f%% similarity\n", sim_hash);
        printf("    Trie:    %.2f%% similarity\n", sim_trie);
        printf("    Average: %.2f%% similarity\n", (sim_hash + sim_trie) / 2.0);
    }
    
    freeFilenames(corpus_files, n);
    
    printf("\n=== Analysis Complete ===\n");
    return 0;
}