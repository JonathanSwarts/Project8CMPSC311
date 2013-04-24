/*
 *	Lister.h
 *		- A linked-list general use program.
 *	
 *	Creator:
 *		- Jonathan Swarts
 *	
 *	Version 1:
 *		- Added Lists
*/
#ifndef LISTER_H
#define LISTER_H

#include <sys/types.h>
//Basic node for storing sources.
struct pr8_source{
	struct source *next;
	struct source *prev;
	struct target *father; //Knows what target it's from.
	char *string;
}
//Basic node for storing recipes.
struct pr8_recipe{
	struct recipe *next;
	struct recipe *prev;
	struct target *father; //Knows what target it's from.
	char *string;
}
//Basic node for storing a target.
struct pr8_target{
	struct target *next;
	struct target *prev;
	struct source *s_head;
	struct recipe *r_head;
	char *string;
	char *name;
	int linenumber;
}
//Basic struct to hold a group of targets.
struct pr8_target_list{
	struct target *head;
	struct target *tail;
	char *name;
	int	length;
}

//Prototype functions here
void pr8_list_init(struct pr8_target_list *list);
//Add target
struct pr8_target *list_add(struct pr8_target_list * const list,const char *name, const char *line, int *lnumber);
//Add recipe to target
struct pr8_source *list_add_source(struct pr8_target *node, char *name);
//Add recipe to target
struct pr8_recipe *list_add_recipe(struct pr8_target *node, char *name);
//removes node, specified by name
int pr8_list_remove(struct pr8_target_list *list, struct pr8_target *name);
//Searchs list, specified by name
struct pr8_target_list *list_search(struct pr8_target_list *list, char *name);
//Prints list
void pr8_list_print(struct pr8_target_list *list);
#endif