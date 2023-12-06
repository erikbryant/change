/******************************************************************************
**
** static char Sccsid[] = "@(#) File: change.c  Version: 1.16  Update: 12/21/92 20:18:45";
**
** Author:
**
**    Written by Erik Bryant (erikbryantology@gmail.com)
**    Modified by Greg Molik (molik@ubvms.cc.buffalo.edu)
**
** Module Name:
**
**    change.c
**
** Description:
**
**    This program takes as input two words and finds the shortest path from
**    one word to the other, changing only one letter at a time.  For example:
**
**    ERIK ZOBO
**    ERIK ERIE BRIE BRAE BRAS BOAS BOBS HOBS HOBO ZOBO
**
** Local Functions:
**
**    wordlist Make_Node ();
**    wordlist Make_Empty_List ();
**    void     Add_To_End ();
**    wordlist Read_Dict ();
**    wordlist Find_String ();
**
** Things to do:
**
**     Optimize the code in main().
**     Run timings on the program to see which routines are slowest.
**     If you do 'change erik zoob' you get "Word /zoob/ not found."
**         If you do 'change zoob erik' you get "No path."  This needs
**         to be more consistent.
**     Maybe print path one word per line so it can be used in a pipe?
**     See about making the MAX_WORD_SIZE constant a variable so that only
**         the space needed is allocated.
**     Look into implementing XOR lists for space savings.
**     Abstract some of the messier code from main() to make it clearer.
**     Make a C++ version for the fun of it.
**
******************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
** The maximum length a word can be is this many characters.
** Be sure to leave space for the null.
*/
#define MAX_WORD_SIZE 10 + 1

/*
** Do some things in-line to save on function calls.
*/
#define ADD_CHILD(p, c) (c->parent = p)

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

typedef struct node_pointer
{
   wordlist first;
   wordlist last;
} NODE_POINTER;

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
** linked list from it.  Only store the words that are of
** the same length as the source and target words.
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
   char source[MAX_WORD_SIZE];
   char target[MAX_WORD_SIZE];
   NODE_POINTER dict, search;
   wordlist temp, tree, ptr;
   int count, i, word_length;

   /*
   ** There must be exactly two arguments.
   */
   if (argc != 3)
   {
      fprintf(stderr, "Usage: change WORD1 WORD2\n");
      exit(1);
   }

   /*
   ** If the words are not the same length then exit.
   */
   if (strlen(argv[1]) != strlen(argv[2]))
   {
      fprintf(stderr, "The words must be the same length.\n");
      exit(1);
   }

   /*
   ** The words are the same length, but are they too long?
   */
   if (strlen(argv[1]) > MAX_WORD_SIZE - 1)
   {
      fprintf(stderr, "Words must be no longer than %d characters.\n", MAX_WORD_SIZE - 1);
      exit(1);
   }

   /*
   ** If the words are the same then we are done.
   */
   if (!strcmp(argv[1], argv[2]))
   {
      printf("%s %s\n", argv[1], argv[2]);
      exit(0);
   }

   /*
   ** The words look okay.  Make sure they are lowercase so that they match
   ** what is in the dictionary.
   */
   strcpy(source, argv[1]);
   strcpy(target, argv[2]);
   for (i = 0; source[i]; source[i] = tolower(source[i]), i++)
      ;
   for (i = 0; target[i]; target[i] = tolower(target[i]), i++)
      ;
   word_length = strlen(source);

   /*
   ** Initialize the list to store words that have not been processed.
   */
   search.first = Make_Empty_List();
   if (search.first->prev)
   {
      search.first = search.first->prev;
   }
   search.last = search.first->next;

   /*
   ** Load the appropriate dictionary into a list.
   */
   dict.first = Read_Dict(strlen(source));
   while (dict.first->prev)
   {
      dict.first = dict.first->prev;
   }
   dict.last = dict.first;
   while (dict.last->next)
   {
      dict.last = dict.last->next;
   }

   /*
    *  Find the source word in the dictionary and make it the
    *  first node in the list of nodes to be matched.  Also make
    *  it the root of the tree.
    */
   Add_To_End(search.last, Find_String(dict.first, target));

   /*
    *  As long as there are nodes that have not been expanded,
    *  do so.
    */
   while (search.first->next->next)
   {
      /*
      ** Remove the first data node from the search list.
      */
      tree = search.first->next;
      search.first->next = tree->next;
      tree->next->prev = search.first;

      /*
       *  Check each word in the dictionary to see if it
       *  is close to the current word.
       */
      for (temp = dict.first->next; temp->next; temp = temp->next)
      {
         /*
          *  Check to see how many letters match.  If all but
          *  one of the letters match then it is a good word.
          */
         count = (tree->word[0] != temp->word[0]) +
                 (tree->word[1] != temp->word[1]) +
                 (tree->word[2] != temp->word[2]);
         for (i = 3; (count < 2) && (i < word_length); i++)
         {
            count += (tree->word[i] != temp->word[i]);
         }
         if (count == 1)
         {
            /*
             *  Check to see if we just found the target word.
             *  If so, traverse the tree backwards to get the
             *  derivation.  If not, add the word to the tree
             *  and to the list of words to match.
             */
            if (!strcmp(source, temp->word))
            {
               ADD_CHILD(tree, temp);
               for (; temp; temp = temp->parent)
               {
                  printf("%s ", temp->word);
               }
               printf("\n");
               exit(0);
            }
            else
            {
               /*
                *  Remove temp from the dictionary before
                *  adding it to the tree and the list.
                */
               ptr = temp;
               temp->prev->next = temp->next;
               temp->next->prev = temp->prev;
               temp = temp->prev;
               ADD_CHILD(tree, ptr);
               Add_To_End(search.last, ptr);
            }
         }
      }
   }

   /*
   ** Everything has been exhausted.  There is no path.
   */
   fprintf(stderr, "No path.\n");
   exit(0);
}
