/*	CMPSC 311 Project 8 Linked List Toolkit
 *	lister.c
 *		- A linked-list kit for targets, sources, and recipes.
 *	
 *	Author: Jonathan Swarts
 *	Email:	jbs5144@psu.edu
 *
 *	Version 1.0, 23 Apr 2013
 *		created list, sublists and funtionality.
 *		   
*/

#include <stdio.h>
#include <stdlib.h>
#include "lister.h"

extern int verbose;

void pr8_list_init(struct pr8_target_list * const list){
  list->count = 0;
  list->head = NULL;
  list->tail = NULL;
  list->name = NULL;
}

struct pr8_target *list_add(struct pr8_target_list * const list,const char *name,const char *line, int lnumber){
  struct pr8_target* target= malloc(sizeof(struct target));
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
  if(list->tail = NULL)
	list->tail = target;
	
	list->length++;
	return target; //returns the target node so you can add sources and recipes.
}

struct pr8_source *list_add_source(struct pr8_target *node, char *name){
  struct pr8_source* source= malloc(sizeof(struct source));
  if (source== NULL)
  {
    fprintf(stderr, "malloc() failed: %s: %d\n", __FILE__, __LINE__);
    return NULL;
  }
  if (verbose) { printf("%s: malloc() allocated %p\n", __FUNCTION__, source); }
  //Add information to source
  source->string = name;
  source->father = node;
  //put it to the top of the source list of target
  source->next = target->s_head;
  node->s_head = source;
  source->prev = NULL;
  if (source->next != NULL) source->next->prev = source;
	return source; //returns the target node so you can add sources and recipes
}

struct pr8_recipe *list_add_recipe(struct pr8_target *node, char *name){
  struct pr8_recipe* recipe= malloc(sizeof(struct recipe));
  if (recipe== NULL)
  {
    fprintf(stderr, "malloc() failed: %s: %d\n", __FILE__, __LINE__);
    return NULL;
  }
  if (verbose) { printf("%s: malloc() allocated %p\n", __FUNCTION__, recipe); }
  //Add information to recipe
  recipe->string = name;
  recipe->father = node;
  //put it to the top of the recipe list of target
  recipe->next = target->r_head;
  node->r_head = recipe;
  recipe->prev = NULL;
  if (recipe->next != NULL) recipe->next->prev = recipe;	
	return recipe; //returns the target node so you can add recipes and recipes
}

int pr8_list_remove(struct pr8_target_list *list, struct pr8_target *name){
  struct pr8_target *temp = name;
  struct pr8_source *s_temp;
  struct pr8_recipe *r_temp;
  //go through the sources and recipes and free() them!
  while(name->s_head != NULL){
	s_temp = name->s_head;
	s_head = s_head->next;
	free(s_temp);
  }
  while(name->r_head != NULL){
	r_temp = name->r_head;
	r_head = r_head->next;
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
	while((p!= NULL) && !(strcmp(p->name, name)){
		p = p->next;
	}
	return p;
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

