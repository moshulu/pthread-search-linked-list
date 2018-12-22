#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

struct node {
  int data;
  int key;
  struct node *next;
  struct node *prev;
};

//this link always point to first Link
struct node *head = NULL;
//this link always point to last Link
struct node *last = NULL;
struct node *current = NULL;
int countA = 0, countB = 0;
int flag = 0;
int threads_running = 0;
pthread_mutex_t lock;
pthread_t threadA;
pthread_t threadB;

//is list empty
bool isEmpty() {
  return head == NULL;
}

int length() {
  int length = 0;
  struct node *current;
  for(current = head; current != NULL; current = current->next){
    length++;
  }
  return length;
}

//display the list in from first to last
void displayForward() {
  //start from the beginning
  struct node *ptr = head;
  //navigate till the end of the list
  printf("\n[ ");
  while(ptr != NULL) {
    printf("(%d,%d) ",ptr->key,ptr->data);
    ptr = ptr->next;
  }
  printf(" ]");
}

void displayBackward() {
  //start from the last
  struct node *ptr = last;

  //navigate till the start of the list
  printf("\n[ ");

  while(ptr != NULL) {
    //print data
    printf("(%d,%d) ",ptr->key,ptr->data);
    //move to next item
    ptr = ptr ->prev;
  }
}

//insert link at the first location

void insertFirst(int key, int data) {
  //create a link
  struct node *link = (struct node*) malloc(sizeof(struct node));
  link->key = key;
  link->data = data;

  if(isEmpty()) {
    //make it the last link
    last = link;
  } else {
  //update first prev link
  head->prev = link;
  }

  //point it to old first link
  link->next = head;
  //point first to new first link
  head = link;
}
/* Checks whether the value x is present in linked list */

//insert link at the last location
void insertLast(int key, int data) {
  //create a link
  struct node *link = (struct node*) malloc(sizeof(struct node));
  link->key = key;
  link->data = data;

  if(isEmpty()) {
    //make it the last link
    last = link;
  } else {
    //make link a new last link
    last->next = link;
    //mark old last node as prev of new link
    link->prev = last;
  }
  //point last to new last node
  last = link;
}

bool insertAfter(int key, int newKey, int data) {
  //start from the first link
  struct node *current = head;
  //if list is empty
  if(head == NULL) {
    return false;
  }

  //navigate through list
  while(current->key != key) {
    //if it is last node
    if(current->next == NULL) {
      return false;
    } else {
      //move to next link
      current = current->next;
    }
  }

  //create a link
  struct node *newLink = (struct node*) malloc(sizeof(struct node));
  newLink->key = newKey;
  newLink->data = data;

  if(current == last) {
    newLink->next = NULL;
    last = newLink;
  } else {
    newLink->next = current->next;
    current->next->prev = newLink;
  }
  newLink->prev = current;
  current->next = newLink;
  return true;
}

struct node* deleteNode(struct node* c){
  struct node* current = c;
  //if the current node is the head...
  if(current->prev == NULL){
    //...just bypass it.
    current = current->next;
    printf("\ncurrent node is the first one in the list. making the current node the next node.\n");
  } else {
    printf("\nsuccessfully made the previous node's next node the next node of the deleted node\n");
    current->prev->next = current->next;
  }

  if(current -> next == NULL){
    printf("\ncurrent node is the last one in the list. making the current node the previous node...\n");
    current = current->prev;
  } else {
    printf("\nsuccessfully made the next node's previous node the previous node of the deleted node\n");
    current->next->prev = current->prev;
  }

  return current;

}

void *forwardSearch(void *x)
{
  struct node *current = head;
  // Initialize current
  int *searchTerm = (int *)x;
  while (current != NULL && flag != 1)
  {

    countA++;
    int y = current->data;
    printf("forwards current->data = %d\n", y);
    if (current->data == *searchTerm){
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~\nfound x going forward: %d. went through %d results\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", *searchTerm, countA);
      pthread_mutex_lock(&lock);
      flag = 1;
      deleteNode(current);
      usleep(500);
      pthread_mutex_unlock(&lock);
      return NULL;
    }
    current = current->next;

  }

  printf("didn't find x going forward. went through %d results\n", countA);
  return NULL;
}

void *backwardSearch(void *x)
{
  struct node *current = last;
  int *searchTerm = (int*)x;
  while(current != NULL && flag != 1){
    countB++;
    int y = current->data;
    printf("backwards current->data = %d\n", y);
    if(current->data == *searchTerm){
      pthread_mutex_lock(&lock);
      flag = 1;
      deleteNode(current);
      usleep(100);
      pthread_mutex_unlock(&lock);
      printf("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~\nfound x going backward: %d. went through %d results\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", *searchTerm, countB) ;
      return NULL;
    }
    current = current -> prev;
  }
  printf("didn't find x going backward. went through %d results\n", countB);
  return NULL;
}

int main()
{
  srand(getpid());
  int x = 0;
  int searchTerm;

  /* initialize the list */
  while(x < 10000){
    insertFirst(x, rand() % 50000 + 1);
    x++;
  }

  /* get user input, store it in searchTerm */
  while(1){
    printf("Welcome. Please enter the number you want to search for, from 0 - 50,000: ");
    scanf("%d", &searchTerm);
    if(searchTerm <= 50000 && searchTerm >= 0) break;
    else continue;
  }

  /* initialize the mutex lock */
  if (pthread_mutex_init(&lock, NULL) != 0){
    printf("\n mutex init failed\n");
    return 1;
  }

  /* create the threads */
  if(pthread_create(&threadA, NULL, forwardSearch, &searchTerm)) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  if(pthread_create(&threadB, NULL, backwardSearch, &searchTerm)) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }

  /* wait for them to finish. If one finishes, then the other automatically finishes */
  if(pthread_join(threadA, NULL)){
    printf("err");
  } else{
    pthread_exit(&threadB);
    printf("didn't find x going backwards either. went through %d results", countB);
  }

  if(pthread_join(threadB, NULL)){
    printf("err");
  } else{
    pthread_exit(&threadA);
    printf("didn't find x going forwards either. went through %d results", countA);
  }

  return 0;

}
