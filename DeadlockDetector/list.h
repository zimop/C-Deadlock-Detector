struct list;

/* Get a new empty list. */
struct list *newlist(int value);

/* Add an item to the head of the list. Returns the new list. */
struct list *addToBeginning(struct list *list, int value);

/* finds the process with the lowest file id in the deadlock diagram*/
int findTerminateNode(struct list* process_list, int starting_point);

/* */
void freeList(struct list *list);

