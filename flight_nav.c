#define MAX_ARRAY 255

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdbool.h>

typedef struct TypeAirportInList {
    int id;
    char* name;
    int num_of_adjacent_airport;
    int* adjacent_airport_ids;
    bool visited;
    int previous_airport_id;
    struct TypeAirport* next;
} Airport;

typedef struct TypeAirportInQueue {
    int id;
    char* name;
    int num_of_adjacent_airport;
    int* adjacent_airport_ids;
    bool visited;
    struct TypeAirportInQueue* next_element_in_queue;
} AirportInQueue;

typedef struct TypeQueue {
    AirportInQueue* first_element;
} Queue;

/**
 * for each line from airports_file, create a new Airport and add it to linked list
 * for each line from connection_file, traverse the existed linked list and init adjacent airport id for each element
 * run boolean BFS() on airport linked list, if true print Congrats, else print Fail
 */
void run(char airports_file[], char connection_file[], char starting_airport[], char ending_airport[]);
Airport* createAirport(char airport_string[]);
void addAdjacentAirport(int airport_id, int num_of_adjacent_airport, int adjacent_airport_ids[], Airport* airport_head);
void addAirportToLast(Airport* airport_new, Airport* airport_head);
Airport* removeAirportHead(Airport* airport_head);
Airport* getAirportById(int id, Airport* airport_head);
Airport* getAirportByName(char name[], Airport* airport_head);
void printAllAirportInLinkedList(Airport* airport_head);
void printAirportInfo(Airport* airport);
bool breathFirstSearch(int starting_airport_id, int ending_airport_id, Airport* airport_head);
AirportInQueue* createAirportInQueue(Airport* airport);
void enqueue(AirportInQueue* new_airport_in_queue, Queue* queue);
AirportInQueue* dequeue(Queue* queue);
void printAirportTrace(Airport* airport, Airport* airport_head);

int main(int argc, char **argv) {
    char airports_file[] = "D:\\STUDY\\Passed Subject\\2ndYear\\Ki1\\CauTrucDuLieuVaGiaiThuat\\BaiTap\\flight-planner\\airports1.txt";
    char connection_file[] = "D:\\STUDY\\Passed Subject\\2ndYear\\Ki1\\CauTrucDuLieuVaGiaiThuat\\BaiTap\\flight-planner\\A1_basic.txt";
    char starting_airport[] = "Sydney";
    char ending_airport[] = "Quebec";

    // run for debug
//    run(airports_file, connection_file, starting_airport, ending_airport);

    // run for published
    run(argv[1], argv[2], argv[3], argv[4]);
    return 0;
}

void run(char airports_file[], char connection_file[], char starting_airport[], char ending_airport[]) {
    FILE *fp;
    char airport_file_buff[MAX_ARRAY][MAX_ARRAY];
    char connection_file_buff[MAX_ARRAY][MAX_ARRAY];
    int connection[MAX_ARRAY][MAX_ARRAY];

    // Init airports
    fp = fopen(airports_file, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    fgets(airport_file_buff[0], sizeof(airport_file_buff[0]), fp);
    char* cut = strtok(airport_file_buff[0], "\n");
    Airport* airport_head = createAirport(cut);
    for (int i = 1; fgets(airport_file_buff[i], sizeof(airport_file_buff[i]), fp) != NULL; i++) {
        char *tok = strtok(airport_file_buff[i], "\n");
        addAirportToLast(createAirport(tok), airport_head);
    }
    fclose(fp);

    // Init adjacent units
    fp = fopen(connection_file, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }
    for (int i = 0; fgets(connection_file_buff[i], sizeof(connection_file_buff[i]), fp); i++) {
        char* tok = strtok(connection_file_buff[i], "\n");
        tok = strtok(connection_file_buff[i], ". ");
        tok = strtok(NULL, " ");
        for (int j = 0; j < MAX_ARRAY && tok != NULL; j++) {
            connection[i + 1][j] = atoi(tok);
            tok = strtok(NULL, " ");
        }
    }

    for (int i = 1; i < MAX_ARRAY; i++) {
        int num_of_adjacent = 0;
        for (int j = 0; j < MAX_ARRAY; j++) {
            if (connection[i][j] != 0) {
                num_of_adjacent++;
            } else {
                break;
            }
        }
        addAdjacentAirport(i, num_of_adjacent, connection[i], airport_head);
    }

    // Debugger to check for the proper initialization
//    printAllAirportInLinkedList(airport_head);

    // get start and dest to pass in BFS
    Airport* start = getAirportByName(starting_airport, airport_head);
    Airport* dest = getAirportByName(ending_airport, airport_head);
    if (start == NULL || dest == NULL) {
        perror("Wrong start point or destination point");
        return;
    }

    if (breathFirstSearch(start->id, dest->id, airport_head)) {
        setbuf(stdout, 0);
        printf("\nCongratulations, you made it. Enjoy your holiday in %s.\n", dest->name);
    } else {
        setbuf(stdout, 0);
        printf("%s\n\nCannot reach destination, might want to buy a real navigation system next time.\n", dest->name);
    }
}

Airport *createAirport(char airport_string[]) {
    Airport* new_airport = (Airport*)malloc(sizeof(Airport));
    char* token = strtok(airport_string, ".");
    new_airport->id = atoi(token);
    token = strtok(NULL, ".");
    new_airport->name = token;
    new_airport->num_of_adjacent_airport = 0;
    new_airport->adjacent_airport_ids = NULL;
    new_airport->previous_airport_id = 0;
    new_airport->visited = false;
    new_airport->next = NULL;
    return new_airport;
}

void addAdjacentAirport(int airport_id, int num_of_adjacent_airport, int adjacent_airport_ids[], Airport* airport_head) {
    Airport *temp = airport_head;
    while (temp != NULL) {
        if (temp->id == airport_id) {
            temp->num_of_adjacent_airport = num_of_adjacent_airport;
            temp->adjacent_airport_ids = adjacent_airport_ids;
            break;
        }
        temp = temp->next;
    }
}

void addAirportToLast(Airport *airport_new, Airport *airport_head) {
    if (airport_head == NULL) {
        perror("head can't be NULL");
        return;
    }

    Airport *temp = airport_head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = airport_new;
}

Airport* removeAirportHead(Airport* airport_head) {
    if (airport_head == NULL) {
        return NULL;
    }

    Airport* temp = airport_head;
    airport_head = airport_head->next;

    return temp;
}

Airport* getAirportById(int id, Airport* airport_head) {
    Airport* temp = airport_head;
    while (temp != NULL) {
        if (temp->id == id) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

Airport *getAirportByName(char *name, Airport *airport_head) {
    Airport* temp = airport_head;
    while (temp != NULL) {
        if (strcmp(name, temp->name) == 0) {
            return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void printAllAirportInLinkedList(Airport *airport_head) {
    Airport *temp = airport_head;
    while (temp != NULL) {
        printAirportInfo(temp);
        temp = temp->next;
    }
}

void printAirportInfo(Airport *airport) {
    setbuf(stdout, 0);
    printf("Airport {id: %d, name: %s, num_of_adjacent: %d, adjacent: ", airport->id, airport->name, airport->num_of_adjacent_airport);
    for (int *iter = airport->adjacent_airport_ids; iter < airport->adjacent_airport_ids + airport->num_of_adjacent_airport ; iter += 1) {
        setbuf(stdout, 0);
        printf("%d ", *iter);
    }
    setbuf(stdout, 0);
    printf("}\n");
}

bool breathFirstSearch(int starting_airport_id, int ending_airport_id, Airport* airport_head) {
    // Init empty queue q
    Queue* q = (Queue*)malloc(sizeof(Queue));
    q->first_element = NULL;

    // Get starting point from list
    Airport* starting_point = getAirportById(starting_airport_id, airport_head);

    // starting point is visited, print out and add it into the queue
    starting_point->visited = true;
    setbuf(stdout, 0);
    printf("%s\n", starting_point->name);
    enqueue(createAirportInQueue(starting_point), q);

    // while there are still unvisited airports, keep traversing the graph
    while (q->first_element != NULL) {
        // we are in the current airport w
        AirportInQueue* w = dequeue(q);

        // traversing adjacent airports of current airport w
        for (int i = 0; i < w->num_of_adjacent_airport; i++) {

            // we are in the airport u which is one of the adjacent airports of previous airport w
            Airport* u = getAirportById(*((w->adjacent_airport_ids) + i), airport_head);

            // if we are here in u for the 1st time, mark it visited
            if (!u->visited) {
                u->previous_airport_id = w->id;
                u->visited = true;
                setbuf(stdout, 0);
                printf("%s\n", u->name);

                // u is the final destination, return success
                if (u->id == ending_airport_id) {
//                    printAirportTrace(u, airport_head);
                    return true;
                }

                // u isn't the final destination, add it to the queue to wait till its turn to be explore
                enqueue(createAirportInQueue(u), q);
            }
        }
    }
    return false;
}

AirportInQueue* createAirportInQueue(Airport* airport) {
    if (airport == NULL) {
        perror("Can't create airport in queue from null");
        return NULL;
    }
    AirportInQueue* clone = (AirportInQueue*)malloc(sizeof(AirportInQueue));
    clone->id = airport->id;
    clone->name = airport->name;
    clone->num_of_adjacent_airport = airport->num_of_adjacent_airport;
    clone->adjacent_airport_ids = airport->adjacent_airport_ids;
    clone->visited = airport->visited;
    clone->next_element_in_queue = NULL;
    return clone;
}

void enqueue(AirportInQueue *new_airport_in_queue, Queue *queue) {
    if (queue == NULL) {
        perror("Can't enqueue because queue is NULL");
        return;
    }

    if (queue->first_element == NULL) {
        queue->first_element = new_airport_in_queue;
        return;
    }

    AirportInQueue* temp = queue->first_element;
    while (temp->next_element_in_queue != NULL) {
        temp = temp->next_element_in_queue;
    }
    temp->next_element_in_queue = new_airport_in_queue;
}

AirportInQueue *dequeue(Queue *queue) {
    if (queue == NULL) {
        perror("Can't dequeue because queue is NULL");
        return NULL;
    }

    if (queue->first_element == NULL) {
        perror("Can't dequeue because queue is empty");
        return NULL;
    }

    AirportInQueue* temp = queue->first_element;
    queue->first_element = queue->first_element->next_element_in_queue;

    return temp;
}

void printAirportTrace(Airport* airport, Airport* airport_head) {
    if (airport == NULL || airport_head == NULL) {
        perror("cannot print null airport");
        return;
    }

    if (airport->previous_airport_id == 0) {
        setbuf(stdout, 0);
        printf("%s\n", airport->name);
        return;
    }

    Airport* prev = getAirportById(airport->previous_airport_id, airport_head);
    printAirportTrace(prev, airport_head);
    setbuf(stdout, 0);
    printf("%s\n", airport->name);
}











