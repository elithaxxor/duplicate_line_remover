/**
 * dedup.c - Fast and memory-efficient duplicate line remover
 * 
 * Removes duplicate lines from a text file while preserving order.
 * Creates a log of removed duplicates.
 * Uses hash table for O(1) lookups resulting in O(n) overall time complexity.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Hash table implementation */
#define HASH_TABLE_SIZE 1000003  // A prime number for better distribution

typedef struct Node {
    char *line;
    struct Node *next;
} Node;

typedef struct {
    Node **buckets;
    size_t size;
} HashTable;

// Create a new hash table
HashTable* create_hash_table(size_t size) {
    HashTable *table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) {
        perror("Failed to allocate hash table");
        exit(EXIT_FAILURE);
    }
    
    table->size = size;
    table->buckets = (Node**)calloc(size, sizeof(Node*));
    if (!table->buckets) {
        perror("Failed to allocate hash buckets");
        free(table);
        exit(EXIT_FAILURE);
    }
    
    return table;
}

// Hash function using djb2 algorithm
unsigned long hash(char *str) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    
    return hash;
}

// Check if a line exists in the hash table, add it if not
bool exists_in_hash(HashTable *table, char *line) {
    unsigned long index = hash(line) % table->size;
    Node *current = table->buckets[index];
    
    // Check if line exists in this bucket
    while (current) {
        if (strcmp(current->line, line) == 0) {
            return true;
        }
        current = current->next;
    }
    
    // Line not found, add it
    Node *new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        perror("Failed to allocate node");
        exit(EXIT_FAILURE);
    }
    
    new_node->line = strdup(line);
    if (!new_node->line) {
        perror("Failed to duplicate line");
        free(new_node);
        exit(EXIT_FAILURE);
    }
    
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    
    return false;
}

// Free memory used by the hash table
void free_hash_table(HashTable *table) {
    for (size_t i = 0; i < table->size; i++) {
        Node *current = table->buckets[i];
        while (current) {
            Node *temp = current;
            current = current->next;
            free(temp->line);
            free(temp);
        }
    }
    free(table->buckets);
    free(table);
}

// Create a new output filename by adding a suffix before the extension
char* create_filename(const char* original, const char* suffix) {
    char *dot = strrchr(original, '.');
    size_t base_len = dot ? (dot - original) : strlen(original);
    size_t ext_len = dot ? strlen(dot) : 0;
    size_t suffix_len = strlen(suffix);
    
    char* new_name = (char*)malloc(base_len + suffix_len + ext_len + 1);
    if (!new_name) {
        perror("Failed to allocate memory for filename");
        exit(EXIT_FAILURE);
    }
    
    strncpy(new_name, original, base_len);
    strcpy(new_name + base_len, suffix);
    if (dot) {
        strcpy(new_name + base_len + suffix_len, dot);
    } else {
        new_name[base_len + suffix_len] = '\0';
    }
    
    return new_name;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *input_file = argv[1];
    char *output_file = create_filename(input_file, "_deduped");
    char *log_file = create_filename(input_file, "_duplicates.log");
    
    FILE *in_fp = fopen(input_file, "r");
    if (!in_fp) {
        perror("Failed to open input file");
        free(output_file);
        free(log_file);
        return EXIT_FAILURE;
    }
    
    FILE *out_fp = fopen(output_file, "w");
    if (!out_fp) {
        perror("Failed to open output file");
        fclose(in_fp);
        free(output_file);
        free(log_file);
        return EXIT_FAILURE;
    }
    
    FILE *log_fp = fopen(log_file, "w");
    if (!log_fp) {
        perror("Failed to open log file");
        fclose(in_fp);
        fclose(out_fp);
        free(output_file);
        free(log_file);
        return EXIT_FAILURE;
    }
    
    // Initialize hash table
    HashTable *table = create_hash_table(HASH_TABLE_SIZE);
    
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t line_count = 0;
    size_t unique_count = 0;
    
    // Write log header
    fprintf(log_fp, "Duplicate lines removed from %s:\n", input_file);
    fprintf(log_fp, "----------------------------------------\n");
    
    // Process file line by line
    while ((read = getline(&line, &len, in_fp)) != -1) {
        line_count++;
        
        // Remove newline if present
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        
        // Check if line already exists
        if (!exists_in_hash(table, line)) {
            // First occurrence, write to output file
            fprintf(out_fp, "%s\n", line);
            unique_count++;
        } else {
            // Duplicate found, write to log
            fprintf(log_fp, "Line %zu: %s\n", line_count, line);
        }
    }
    
    // Print summary
    printf("\nResults:\n");
    printf("  Original lines: %zu\n", line_count);
    printf("  Unique lines: %zu\n", unique_count);
    printf("  Duplicates removed: %zu\n", line_count - unique_count);
    printf("\n");
    printf("Deduplicated file saved as: %s\n", output_file);
    printf("Removed duplicates logged in: %s\n", log_file);
    
    // Clean up
    free(line);
    free_hash_table(table);
    fclose(in_fp);
    fclose(out_fp);
    fclose(log_fp);
    free(output_file);
    free(log_file);
    
    return EXIT_SUCCESS;
}
