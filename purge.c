/******************************************************************************
**
** static char Sccsid[] = "@(#) File: purge.c  Version: 1.4  Update: 12/21/92 19:53:50";
**
** Author:
**
**    Written by Erik Bryant (erikbryantology@gmail.com)
**
** Module Name:
**
**    purge.c
**
** Description:
**
**    This program removes all of the words from a dictionary that are dead
**    ends.  That is, they can not be derived from any other words.
**
** Local Functions:
**
**    wordlist Make_Node ();
**    wordlist Make_Empty_List ();
**    void     Add_To_End ();
**    wordlist Read_Dict ();
**    wordlist Find_String ();
**
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** The maximum length a word can be is this many characters.
** Be sure to leave space for the null.
*/
#define MAX_WORD_SIZE 10 + 1

/*
** Define the dictionary file name.
*/
#define DICTIONARY "change.dic"

/*
** Data structures.
*/
typedef struct word_node *wordlist;
struct word_node
{
   char word[MAX_WORD_SIZE];
   wordlist next, prev;
   wordlist parent;
};

#define TRUE 1
#define FALSE 0

/*
** Given a word create a node and put the word into that node.
*/
wordlist Make_Node(char string[])
{
   wordlist newWord;

   newWord = (wordlist)malloc(sizeof(struct word_node));
   if (newWord != NULL)
   {
      strcpy(newWord->word, string);
   }
   else
   {
      fprintf(stderr, "Could not add word /%s/, out of memory.\n", string);
      exit(9);
   }

   return newWord;
}

/*
** Create an empty doubly linked list with sentinel nodes.
*/
wordlist Make_Empty_List()
{
   wordlist list;

   /*
    *  Create the two sentinel nodes that make up an empty list.
    *  Hook the nodes together.
    */
   list = Make_Node("->");
   list->next = Make_Node("<-");
   list->next->prev = list;
   list->prev = NULL;
   list->next->next = NULL;

   return list;
}

/*
** Add the node on to the end of the given list.
*/
void Add_To_End(wordlist list, wordlist node)
{
   /*
    *  Go to the last node of the list.
    */
   while (list->next)
   {
      list = list->next;
   }

   /*
    *  Add the new node right before the last node of the list.
    */
   node->prev = list->prev;
   node->next = list;
   node->prev->next = node;
   list->prev = node;
}

/*
** Read an entire dictionary file into memory and build a
** linked list from it.
*/
wordlist Read_Dict(int dict_number)
{
   FILE *in;
   char string[MAX_WORD_SIZE];
   wordlist node, dict;

   /*
    *  Make an empty list to put words in.
    */
   dict = Make_Empty_List();

   /*
    *  Open the data file.  Read the words in and add them
    *  to the end of the list.
    */
   in = fopen(DICTIONARY, "r");
   if (!in)
   {
      fprintf(stderr, "Unable to open %s dictionary file.\n", DICTIONARY);
      exit(2);
   }

   while (fscanf(in, "%s\n", string) != EOF)
   {
      if (strlen(string) == dict_number)
      {
         node = Make_Node(string);
         Add_To_End(dict, node);
         dict = node;
      }
   }

   /*
   ** Close the file.
   */
   fclose(in);

   return dict;
}

/*
** Print out all of the dictionary file except for those words of
** a given length.
*/
void Print_Dict(int dict_number)
{
   FILE *in;
   char string[MAX_WORD_SIZE];

   /*
    *  Open the data file.  Read the words in and add them
    *  to the end of the list.
    */
   in = fopen(DICTIONARY, "r");
   if (!in)
   {
      fprintf(stderr, "Unable to open %s dictionary file.\n", DICTIONARY);
      exit(2);
   }

   while (fscanf(in, "%s\n", string) != EOF)
   {
      if (strlen(string) != dict_number)
      {
         printf("%s\n", string);
      }
   }

   /*
   ** Close the file.
   */
   fclose(in);

   return;
}

/*
** Search a linked list for a string.  Once it is found remove
** it from the list and return it.  Start scanning from the
** current position in the list.
*/
wordlist Find_String(wordlist list, char string[])
{
   wordlist ptr;

   for (ptr = list; ptr->next; ptr = ptr->next)
      if (!strcmp(string, ptr->word))
      {
         ptr->prev->next = ptr->next;
         ptr->next->prev = ptr->prev;
         return (ptr);
      }
   fprintf(stderr, "Word /%s/ not found in dictionary.\n", string);
   exit(1);
}

int main(int argc, char *argv[])
{
   wordlist dict, temp, current;
   int count, i, dead_count;
   int match;

   /*
   ** There must be exactly one argument.
   */
   if (argc != 2)
   {
      fprintf(stderr, "Usage: purge <word-length>\n");
      exit(1);
   }

   /*
   ** The argument must be a cardinal number.
   */
   if (atoi(argv[1]) < 1)
   {
      fprintf(stderr, "You must specify a positive integer for the word length.\n");
      exit(1);
   }

   /*
   ** Load the appropriate dictionary into a list.
   */
   dict = Read_Dict(atoi(argv[1]));
   for (; dict->prev; dict = dict->prev)
      ;

   /*
    *  As long as there are nodes that have not been expanded,
    *  do so.
    */
   dead_count = 0;
   for (current = dict->next; current->next != NULL; current = current->next)
   {
      match = FALSE;
      for (temp = dict->next; temp->next != NULL; temp = temp->next)
      {
         /*
          *  Check to see how many letters match.  If all but
          *  one match then it is a good word.
          */
         count = 0;
         for (i = 0; current->word[i] != 0; i++)
            if (current->word[i] != temp->word[i])
               count++;
         match = match || (count == 1);
      }
      if (!match)
      {
         fprintf(stderr, "%s\n", current->word);
         Find_String(dict, current->word);
         current = current->prev;
         dead_count++;
      }
   }

   /*
   ** Print out the new dictionary.
   */
   fprintf(stderr, "Removed %d dead ends.\n", dead_count);
   for (temp = dict->next; temp->next != NULL; temp = temp->next)
   {
      printf("%s\n", temp->word);
   }

   /*
   ** Print out the words from the rest of the dictionary.
   */
   Print_Dict(atoi(argv[1]));

   return (0);
}
