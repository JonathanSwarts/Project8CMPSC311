/*	CMPSC 311 Project 8 Linked List Toolkit
 *	lister.h
 *		- A linked-list kit for targets, sources, and recipes.
 *	
 *	Author: Jonathan Swarts
 *	Email:	jbs5144@psu.edu
 *
 *	Version 1.0, 23 Apr 2013
 *		created list, sublists and funtionality.
 *		   
*/
#ifndef LISTER_H
#define LISTER_H

#include <sys/types.h>
//Basic node for storing sources.
struct pr8_source{
	struct pr8_source *next;
	struct pr8_source *prev;
	struct pr8_target *father; //Knows what target it's from.
	char *string;
};
//Basic node for storing recipes.
struct pr8_recipe{
	struct pr8_recipe *next;
	struct pr8_recipe *prev;
	struct pr8_target *father; //Knows what target it's from.
	char *string;
};
//Basic node for storing a target.
struct pr8_target{
	struct pr8_target *next;
	struct pr8_target *prev;
	struct pr8_source *s_head;
	struct pr8_recipe *r_head;
	char *string;
	char *name;
	int linenumber;
};
//Basic struct to hold a group of targets.
struct pr8_target_list{
	struct pr8_target *head;
	struct pr8_target *tail;
	char *name;
	int	length;
};

//Prototype functions here
void pr8_list_init(struct pr8_target_list *list);
//Add target
struct pr8_target *list_add(struct pr8_target_list * const list, char * const name, char * const line, int lnumber);
//Add recipe to target
struct pr8_source *list_add_source(struct pr8_target *node, char *name);
//Add recipe to target
struct pr8_recipe *list_add_recipe(struct pr8_target *node, char *name);
//removes node, specified by name
int pr8_list_remove(struct pr8_target_list *list, struct pr8_target *name);
//Searchs list, specified by name
struct pr8_target *list_search(struct pr8_target_list *list, char *name);
//Prints list -- debug
void pr8_list_print(struct pr8_target_list *list);
//Prints Source List
void pr8_list_print_source(struct pr8_target *node);
//Prints Recipe List
void pr8_list_print_recipe(struct pr8_target *node);
//Removes the whole list
int pr8_list_clear(struct pr8_target_list *list);
#endif