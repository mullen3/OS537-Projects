struct Node* head;

void addToHead(struct Node * node){
node->next = root;
root = node;
}

void chunkNode(struct Node * node, int size){
	size += size - size % 8;
	node->ptr += size;
	node->size -= size;
	node->used += size;
}

void *findNeighborNode(struct node * node){
	struct node * curr = headFree;
	struct node * end;
	struct node * beginning;
	while (curr != NULL)
	{
		if (curr->ptr + curr->size == node->ptr || node->ptr + node->size == curr->ptr)
	}
}
