static char Sccsid[] = "/* @(#) File: match.c  Version: 1.4  Update: 12/21/92 19:53:15  */";

/*
** Given a word, find all the other words in the dictionary
** which differ from it by only one letter.
*/

#include "stdio.h"
#define word_size 10 + 1
#define DICTIONARY "change.dic"

typedef struct word_node *wordlist;
struct word_node {
   char word[word_size];
   wordlist next, prev;
   wordlist parent;
};



wordlist Make_Node (string)
   char string[];

{
   wordlist new;

   if ((new = (wordlist) malloc (sizeof (struct word_node))) != NULL)
      {
         strcpy (new->word, string);
	 }
   else
      {
	 printf ("Couldn't add word /%s/\n", string);
	 printf ("Out of memory.\n");
	 exit (9);
      }

   return new;
}



wordlist Make_Empty_List ()

{
   wordlist list;

   /*
    *  Create the two sentinel nodes that make up an empty list.
    *  Hook the nodes together.
    */
   list = Make_Node (">");
   list->next = Make_Node ("<");
   list->next->prev = list;
   list->prev = NULL;
   list->next->next = NULL;

   return list;
}



void Add_To_End (list, node)
   wordlist list, node;

{
   /*
    *  Go to the last node of the list.
    */
   for (; list->next != NULL; list = list->next);

   /*
    *  Add the new node right before the last node of the list.
    */
   node->prev = list->prev;
   node->next = list;
   node->prev->next = node;
   list->prev = node;
}


wordlist Read_Dict (dict_number)
   int dict_number;

{
   FILE *in;
   char string[word_size];
   wordlist node, dict, ptr;

   /*
    *  Make an empty list to put words in.
    */
   dict = Make_Empty_List ();
   ptr = dict;

   /*
    *  Open the data file.  Read the words in and add them
    *  to the end of the list.
    */
   in = fopen (DICTIONARY, "r");
   if (in == NULL)
      {
	 printf ("Unable to open %s dictionary file.\n", DICTIONARY);
	 exit (2);
      }

   while (fscanf (in, "%s\n", string) != EOF)
      {
         if (strlen (string) == dict_number)
         {
            node = Make_Node (string);
	    Add_To_End (ptr, node);
	    ptr = node;
         }
      }

   /*
   ** Close the file.
   */
   fclose (in);

   return dict;
}


wordlist Find_String (list, string)
   wordlist list;
   char string[];

{
   wordlist ptr;

   for (ptr=list; ptr->next != NULL; ptr = ptr->next)
      if (!strcmp(string, ptr->word))
         {
            ptr->prev->next = ptr->next;
            ptr->next->prev = ptr->prev;
            ptr->next = NULL;
            ptr->prev = NULL;
            return (ptr);
         }
   printf ("Word /%s/ not found in dictionary.\n", string);
   exit (1);
   return (NULL);
}


main(argc, argv)
   int argc;
   char *argv[];

{
   wordlist dict, temp, current;
   int count, i, comparisons, found;

   if (argc != 2)
      {
	 printf ("Usage: match WORD\n");
	 exit (1);
      }

   dict = Read_Dict(strlen (argv[1]));

   /*
    *  Find the source word in the dictionary and remove it.
    */
   current = Find_String (dict, argv[1]);

   comparisons = 0;
   found = 0;
   for (temp = dict; temp != NULL; temp = temp->next)
      {
         comparisons++;
         /*
          *  Check to see how many letters match.  If all but
          *  one match then it is a good word.
          */
         count = 0;
         for (i=0; current->word[i] != 0; i++)
            if (current->word[i] != temp->word[i])
               count++;
         if (count == 1)
            {
               found++;
               printf ("%s ", temp);
               /*
                *  Remove temp from the dictionary before
                *  adding it to the tree and the list.
                */
               temp->prev->next = temp->next;
               temp->next->prev = temp->prev;
               temp = temp->prev;
            }
      }
   printf ("\nFound %d words after %d comparisons.\n", found, comparisons);

   return (0);
}
