/**
 * @file    main.c
 * \mainpage HW04 Documentation
 *
 *
 * On the 'Files' page, there is a list of documented files with brief descriptions.
 *
*/
#include "graph.h"
#include "heap.h"
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#define LineMaxSize 202

void close_files(FILE *first, FILE *second)
{
    fclose(first);
    fclose(second);
}

void close_output(FILE *file)
{
    if (file != stdout) {
        fclose(file);
    }
}

int convert(uint32_t *id, char *token)
{
    char *end;
    *id = strtol(token, &end, 10);
    if (*end != '\0' || errno == ERANGE) {
        fprintf(stderr, "No such node.\n");
        return 1;
    }
    return 0;
}

int read_nodes(FILE *node_file, Graph *graph)
{
    char buffer[LineMaxSize] = { 0 };
    while (fgets(buffer, sizeof(buffer), node_file) != NULL) {
        if (strlen(buffer) > 200) {
            fprintf(stderr, "Invalid input (too long).\n");
            return 1;
        }
        char *token = strtok(buffer, ",");
        int count = 0;
        while (token) {
            count++;
            if (count == 1) {
                uint32_t node_id = 0;
                if (convert(&node_id, token) != 0) {
                    return 1;
                }
                if (!graph_insert_node(graph, node_id)) {
                    fprintf(stderr, "Lack of memory.\n");
                    return 1;
                }
            }
            token = strtok(NULL, ",");
        }
        if (count != 7) {
            fprintf(stderr, "Wrong amount of data in NODE_FILE.\n");
            return 1;
        }
    }
    return 0;
}

int read_edges(FILE *edge_file, Graph *graph)
{
    char buffer[LineMaxSize] = { 0 };
    while (fgets(buffer, sizeof(buffer), edge_file) != NULL) {
        if (strlen(buffer) > 200) {
            fprintf(stderr, "Invalid input (too long).\n");
            return 1;
        }
        char *token = strtok(buffer, ",");
        int count = 0;
        uint32_t start_node = 0;
        uint32_t end_node = 0;
        uint32_t min_time = 0;
        while (token) {
            count++;
            switch (count) {
                case 1: {
                    if (convert(&start_node, token) != 0) {
                        return 1;
                    }
                }
                case 2: {
                    if (convert(&end_node, token) != 0) {
                        return 1;
                    }
                }
                case 4: {
                    if (convert(&min_time, token) != 0) {
                        return 1;
                    }
                }
                default: {
                    token = strtok(NULL, ",");
                }
            }
        }
        if (count != 7) {
            fprintf(stderr, "Wrong amount of data in EDGE_FILE.\n");
            return 1;
        }
        if (!graph_insert_edge(graph, start_node, end_node, min_time)) {
            fprintf(stderr, "Can not insert the edge from %d to %d.\n", start_node, end_node);
            return 1;
        }
    }
    return 0;
}

int dijkstra(Graph *graph, Node *start)
{
    Heap *heap = heap_new_from_graph(graph);
    if (!heap) {
        fprintf(stderr, "Can not create heap.\n");
        return 1;
    }
    heap_decrease_distance(heap, start, 0, NULL);
    while (!heap_is_empty(heap)) {
        Node *current = heap_extract_min(heap);
        if (node_get_distance(current) == UINT_MAX) {
            break;
        }
        struct edge *edges = node_get_edges(current);
        unsigned short count = node_get_n_outgoing(current);
        for (int i = 0; i < count; i++) {
            if (edges[i].destination == NULL) {
                continue;
            }
            Node *dest = edges[i].destination;
            int length_edge = edges[i].mindelay;
            if (node_get_distance(dest) > node_get_distance(current) + length_edge) {
                heap_decrease_distance(heap, dest, node_get_distance(current) + length_edge, current);
            }
        }
    }
    heap_free(heap);
    return 0;
}

int output(Node *start, Node *end, FILE *output_file)
{
    if (start == end) {
        fprintf(output_file, "digraph {\n}\n");
        return 0;
    }
    fprintf(output_file, "digraph {\n");
    Node *prev = node_get_previous(end);
    while (prev != NULL) {
        unsigned int length = node_get_distance(end) - node_get_distance(prev);
        uint32_t second_id = node_get_id(end);
        uint32_t first_id = node_get_id(prev);
        fprintf(output_file, "\t%u -> %u [label=%u];\n", first_id, second_id, length);
        end = prev;
        prev = node_get_previous(end);
    }
    fprintf(output_file, "}\n");
    return 0;
}

int check_path(Node *start, Node *end)
{
    Node *current = end;
    while (current != start) {
        if (current == NULL) {
            fprintf(stderr, "No such path.\n");
            return 1;
        }
        current = node_get_previous(current);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 5 || argc > 6) {
        fprintf(stderr, "Usage: ./graph-traverse NODE_FILE EDGE_FILE START_NODE END_NODE [OUTPUT_FILE]\n");
        return 1;
    }
    FILE *node_file;
    FILE *edge_file;
    FILE *output_file;
    uint32_t start_node = 0;
    uint32_t end_node = 0;
    if ((node_file = fopen(argv[1], "r")) == NULL) {
        perror("Can not open node file.\n");
        return 1;
    }
    if ((edge_file = fopen(argv[2], "r")) == NULL) {
        perror("Can not open edge file.\n");
        fclose(node_file);
        return 1;
    }
    if (argc == 6) {
        if ((output_file = fopen(argv[5], "w")) == NULL) {
            fprintf(stderr, "Can not open output file.\n");
            close_files(node_file, edge_file);
            return 1;
        }
    }
    else {
        output_file = stdout;
    }
    char *end;
    start_node = strtol(argv[3], &end, 10);
    if (*end != '\0' || errno == ERANGE) {
        fprintf(stderr, "No such node.\n");
        close_files(edge_file, node_file);
        return 1;
    }
    char *end2;
    end_node = strtol(argv[4], &end2, 10);
    if (*end2 != '\0' || errno == ERANGE) {
        fprintf(stderr, "No such node.\n");
        close_files(edge_file, node_file);
        return 1;
    }
    Graph *graph = graph_new();
    if (graph == NULL) {
        fprintf(stderr, "Lack of memory.\n");
        return 1;
    }
    if (read_nodes(node_file, graph) != 0) {
        close_files(node_file, edge_file);
        graph_free(graph);
        return 1;
    }
    if (read_edges(edge_file, graph) != 0) {
        close_files(node_file, edge_file);
        graph_free(graph);
        return 1;
    }
    close_files(node_file, edge_file);
    Node *from = graph_get_node(graph, start_node);
    Node *to = graph_get_node(graph, end_node);
    if (!from || !to) {
        fprintf(stderr, "No such node.\n");
        close_output(output_file);
        graph_free(graph);
        return 1;
    }
    if (dijkstra(graph, from) != 0) {
        graph_free(graph);
        return 1;
    }
    if (check_path(from, to) != 0) {
        close_output(output_file);
        graph_free(graph);
        return 1;
    }
    if (output(from, to, output_file) != 0) {
        close_output(output_file);
        graph_free(graph);
        return 1;
    }
    close_output(output_file);
    graph_free(graph);
    return 0;
}

/*
 * If you want to document it Doxygen, the first comment must remain as
 * it is now. The comment tells Doxygen that this file should be
 * processed
 *  */
