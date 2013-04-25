/*	CMPSC 311 Project 8 Linked List Toolkit
 *	lister.c
 *		- A linked-list kit for targets, sources, and recipes.
 *	
 *	Author: Jonathan Swarts
 *	Email:	jbs5144@psu.edu
 *
 *	Version 1.1, 24 Apr 2013
 *		created list, sublists and funtionality.
 *		+	turned stacks into queues for proper output.   
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lister.h"

extern int verbose;

void pr8_list_init(struct pr8_target_list * const list){
  list->length = 0;
  list->head = NULL;
  list->tail = NULL;
  list->name = NULL;
}

struct pr8_target *list_add(struct pr8_target_list * const list,char * const name,char * const line, int lnumber){
  struct pr8_target* target= malloc(sizeof(struct pr8_target));
  if (target == NULL)
  {
    fprintf(stderr, "malloc() failed: %s: %d\n", __FILE__, __LINE__);
    return NULL;
  }
  if (verbose) { printf("%s: malloc() allocated %p\n", __FUNCTION__, target); }
  //Add information to target
  target->name = name;
  target->string = line;
  target->linenumber = lnumber;
  target->s_head = NULL;
  target->r_head = NULL;
  //put it to the top of the list
  target->next = list->head;
  list->head = target;
  target->prev = NULL;
  if (target->next != NULL) target->next->prev = target;
  if(list->tail == NULL)
    list->tail = target;
	
	list->length++;
	return target; //returns the target node so you can add sources and recipes.
}

struct pr8_source *list_add_source(struct pr8_target *node, char *name){
  struct pr8_source* source= malloc(sizeof(struct pr8_source));
  if (source== NULL)
  {
    fprintf(stderr, "malloc() failed: %s: %d\n", __FILE__, __LINE__);
    return NULL;
  }
  if (verbose) { printf("%s: malloc() allocated %p\n", __FUNCTION__, source); }
  //Add information to source
  source->string = name;
  source->father = node;
  //Check if it's first
  if(node->s_head != NULL){
	struct pr8_source* temp = node->s_head;
	while(temp->next != NULL){
		temp = temp->next;
	}
	//At last one
	//Add
	source->next = NULL;
	temp->next = source;
	source->prev = temp;
	return source;
  }
  //put it to the top of the source list of target
  source->next = node->s_head;
  node->s_head = source;
  source->prev = NULL;
  if (source->next != NULL) source->next->prev = source;
	return source; //returns the target node so you can add sources and recipes
}

struct pr8_recipe *list_add_recipe(struct pr8_target *node, char *name){
  struct pr8_recipe* recipe= malloc(sizeof(struct pr8_recipe));
  if (recipe== NULL)
  {
    fprintf(stderr, "malloc() failed: %s: %d\n", __FILE__, __LINE__);
    return NULL;
  }
  if (verbose) { printf("%s: malloc() allocated %p\n", __FUNCTION__, recipe); }
  //Add information to recipe
  recipe->string = name;
  recipe->father = node;
  //Check if it's first
  if(node->r_head != NULL){
	struct pr8_recipe* temp = node->r_head;
	while(temp->next != NULL){
		temp = temp->next;
	}
	//At last one
	//Add
	recipe->next = NULL;
	temp->next = recipe;
	recipe->prev = temp;
	return recipe;
  }
  //put it to the top of the recipe list of target
  recipe->next = node->r_head;
  node->r_head = recipe;
  recipe->prev = NULL;
  if (recipe->next != NULL) recipe->next->prev = recipe;	
	return recipe; //returns the target node so you can add recipes and recipes
}

int pr8_list_remove(struct pr8_target_list *list, struct pr8_target *name){
  struct pr8_source *s_temp;
  struct pr8_recipe *r_temp;
  //go through the sources and recipes and free() them!
  while(name->s_head != NULL){
	s_temp = name->s_head;
	name->s_head = name->s_head->next;
	free(s_temp);
  }
  while(name->r_head != NULL){
	r_temp = name->r_head;
	name->r_head = name->r_head->next;
	free(r_temp);
  }
  //Go around this node in the list.
  if(name->next!= NULL)
  name->next->prev = name->prev;
  if(name->prev!= NULL)
  name->prev->next = name->next->prev;
  
  free(name);
  list->length--;
  return list->length;
}

struct pr8_target *list_search(struct pr8_target_list *list, char *name){
	if(list == NULL) return NULL;
	if(list->head == NULL) return NULL;
	
	struct pr8_target *t = list->head;
	while((t!= NULL) && (strcmp(t->name, name) != 0)){
		t = t->next;
	}
	return t;
}

void pr8_list_print(struct pr8_target_list *list){
	//Used for debugging.
	printf("%s: %d targets\n", list->name, list->length);
	struct pr8_target *temp = list->head;
	struct pr8_source *s_temp;
	struct pr8_recipe *r_temp;
	while(temp != NULL){
		printf("%s: \n", temp->name);
		s_temp = temp->s_head;
		r_temp = temp->r_head;
		printf("\tSources: \n");
		while(s_temp != NULL){
			printf("%s\n", s_temp->string);
			s_temp = s_temp->next;
		}
		printf("\tRecipes: \n");
		while(r_temp != NULL){
			printf("%s\n", r_temp->string);
			r_temp = r_temp->next;
		}
		temp = temp->next;
	}
}

void pr8_list_print_source(struct pr8_target *node){
	struct pr8_source *s_temp = node->s_head;
	while(s_temp != NULL){
			printf("\t%s\n", s_temp->string);
			s_temp = s_temp->next;
		}
}

void pr8_list_print_recipe(struct pr8_target *node){
	struct pr8_recipe *r_temp = node->r_head;
	while(r_temp != NULL){
			printf("\t%s\n", r_temp->string);
			r_temp = r_temp->next;
		}
}

int pr8_list_clear(struct pr8_target_list *list){
  struct pr8_target *t_temp;
  while(list->head != NULL){
	  //grab node and delete what's inside
	  struct pr8_source *s_temp;
	  struct pr8_recipe *r_temp;
	  //go through the sources and recipes and free() them!
	  while(list->head->s_head != NULL){
		s_temp = list->head->s_head;
		list->head->s_head = list->head->s_head->next;
		free(s_temp);
	  }
	  while(list->head->r_head != NULL){
		r_temp = list->head->r_head;
		list->head->r_head = list->head->r_head->next;
		free(r_temp);
	  }
	t_temp = list->head;  
	list->head = list->head->next;
	free(t_temp);
  }
  if(list->head != NULL){
		//ERROR somewhere
		return 0;
  }
  return 1;
}