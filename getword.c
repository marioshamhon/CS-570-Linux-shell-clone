/*
    Name: Mario Shamhon
    
    Instructor: John Carroll
    
    Description: The purpose of this function is to read in from stdin
    until a word is found. Words are delimited by a space, tab, newline
    or until End of file (EOF) has occured. Words are also delimited by 
    these  meta-characters: '<', '>',  '<<', '&' and, '|' Then the program
    returns the word  and the  number of characters in a word. This function
    also returns those meta-characters as a word of size one 
    (except '<<' which returns a word of size two). This program 
    also returns  the  negative number of characters in a word 
    if a dollar sign is the first character found on the input stream.
    In addtion, if a dollar sign is  followed by a newline, tab, space, 
    '<', '>',  '<<', '&', '|' or EOF then it will return that 
    character as a zero length word. -This function also handles 
    other meta-characters in a different way. These other meta-characters 
    are: '~', '$', and '\'. If a backslash comes before a meta-character 
    that meta-character becomes a part of of the word, and is no longer 
    considered a word delimiter. Also, a backslash that precides a space 
    will cause a space character to be placed in the string, and that 
    space counts as a part of the word. A backslash followed by a newline
    will act as if a space character was found, and delimate the word. A
    backlash followed, by another backslash with place a single backslash 
    character into the string. As for the '$' meta-characxter if it is 
    found in the middle of a word then it just becomes a part of that word 
    returning the positive number of characters in that word. Finally, a 
    tilde when found at the start of a word will return the user's home 
    directory followed the rest of the string on the input stream (if applicable).
    If a tilde if found in the middle of a word then it just become a part 
    of the word. Lastly, if a '$' character is followed a '~' character then
    the program will return that word as a size of negative one. 
    
    The only update to this function is that now it checks when an "&" character 
    is found if it is the last word on the commandline, and sets a flag to signify 
    that this command will be a background job. The way this new feature works is: 
    
    if an "&" character is found and the number of characters is zero then
    getchar is called and checks if the adjacent character is either a "$" 
    or a newline character then a null terminator is put in place of the "&"
    character, and zero is returned to parse. If the adjacent character is 
    not a "$" or newline character then ungetc is called to put the adjacent 
    character back on the input stream to be read in the next time getchar 
    is called. 
    
    SIDE NOTE: This new feature also removes a space if it is between the 
    "&" and the adjacent character. 
    
    This function now includes an addtional update. When a "~" character is
    found getword now checks if the next character after the tilde is not a
    newline then collects those "words" after the tilde to be used later to 
    lookup usernames. 
                 
    Project: Four    
  
    Class: CS570
   
    Due Date: 11/292020
*/

#include "getword.h" 
#include <stdlib.h> //We need this to be able to use the getenv() function.

/*Global variable declerations */
int lastampFlag = 0; //A flag to indicate "&" was the last character on the line.
int tildeFlag = 0; //A flag to indicate "~" was the first character on the line.

int getword(char *w)
{
int iochar = 0; // Holds the read-in character.
int adjacentCharacter = 0; // Holds the adjucent character.
int numberOfCharacters = 0; // A counter to keep track of the number of charcters found within a word.
int index = 0; // keeps track of the array position (pointer address).
int startsWithADollarSign = 0; // A boolean value that indicates if a '$' was the first character found in the input stream 0 = No 1 = Yes.

char *homePath = getenv("HOME"); // Stores the path of our home directory.
 
 while( (iochar = getchar() ) != EOF) //Keep reading-in characters until EOF is found. 
 {
    /*START: This section of codes handles the buffer overflow case. */
    if( (numberOfCharacters) == (STORAGE - 1) ) //This prevents buffer overflow and makes sure only 254 characters are returned per getword function call.
    {  
      w[index] = '\0'; //Null terminates the string.
      ungetc(iochar, stdin); //Puts back the character before we return the word so we don't lose it.
      return numberOfCharacters;
    }
    //END of section.
    
    
    /* START: This section handles "&' as the last word on the line */
    if( (numberOfCharacters == 0) && (iochar == '&') ) //Checks if "&" is the last word on the line
    {
      adjacentCharacter = getchar();
      if( (adjacentCharacter == '\n') || (adjacentCharacter == '$') ) //checks if the next character is line terminator, so we know to return to parse
      {
        w[index] = '\0'; 
        lastampFlag = 1; //Sets a flag letting us know that "&" was the last word found on the line.
        return 0;
      }
      
      if( (iochar == '&') && (adjacentCharacter == ' ') ) //This checks if there is a "space" between "&" and the next character, and removes it if need be.  
      {
         ungetc(iochar, stdin); //This puts the "&" back on the input stream after removing the space, so the "&" character can be processed appropriately, the next time getchar is called. 
         continue;
      }
      
      if( (adjacentCharacter != '\n') && (adjacentCharacter != '$') ) //if the adjacent character isn't a newline or a dollar sign
      {
        ungetc(adjacentCharacter, stdin); //puts the adjacenter character when it is not a line terminator, so getword can get the correct "word"
      }
    
    }  
    //END of section.

    /*START: This section of code deals with all the special dollar sign cases.*/  
    if( (numberOfCharacters == 0) && (iochar == '$') ) //Handles when a '$' is the first character of a word.
    {
      if( (adjacentCharacter = getchar() ) == '$') //Handles the cases when the first character is a '$' and the adjacent character is a also a '$'.
      {
        w[index] = adjacentCharacter; //This puts the read-in character in the *w buffer.
        index++; //Moves pointer to next address.
        numberOfCharacters++; //Increases the number of charcters count by 1.
        startsWithADollarSign = 1;
        continue;
      }
      
      if( (adjacentCharacter != '$') ) //puts back the adjacent character if its not a '$'.
      {
        ungetc(adjacentCharacter, stdin); // puts the adjacent character on the input stream if its not a '$'
      }
      startsWithADollarSign = 1; // Sets the "boolean" to "yes" 
      continue; // This restarts at the while loop so the '$' isn't collected and not put into the buffer.
    }
    
    if( (numberOfCharacters > 0) && (iochar == '$') ) //Handles when a '$' is in the middle of a word.
    {
      w[index] = iochar; //put the '$' in *w
      index++; //Move the index 
      numberOfCharacters++;
      continue;
    }
    
    if( (startsWithADollarSign == 1) && (numberOfCharacters == 0) && (iochar == '\n' || iochar == ' ' || iochar == '\t') ) //When a '$' is followed by a space, tab, or NL and returns a zero length word. 
    {
      if(iochar == '\n') //Test to see if the character after '$' is a NL character. 
      {
        ungetc(iochar, stdin); //Puts the NL character back on the input stream.
      }
      w[index] = '\0';  
      return 0;
    }
   /*END of section */

  //START: This section handles all of the '~' character cases. 
   if( (numberOfCharacters == 0) && (iochar == '~') ) //Handles when the '~' is the first character on the input stream.
   {
     if(startsWithADollarSign == 1) //This handle the '$''~' case.
     {
       w[index] = iochar; //Puts the '~' character in *w.
       index++; //Moves the index 
       w[index] = '\0'; //Null terminates the word.
       numberOfCharacters++; //Increases the number of characters by one.
       return -(numberOfCharacters);
     }
     
     if( (adjacentCharacter = getchar() ) != '\n' && (adjacentCharacter != ' ') && (adjacentCharacter != '/')  ) //This check if the next character after the tilde was a NL, spaces or "/"
     {
       ungetc(adjacentCharacter, stdin); //put the non-newline character back, so getword can get the word after the "~" character.
       tildeFlag = 1; //Sets a flag to notify that a tilde was found. 
       continue; //go to the top of the loop to collect the string found after the tilde. 
     }
     else //if the adjacent character is a newline 
     {
       ungetc(adjacentCharacter, stdin); //Put the newline back.
     }
     strcpy(w, homePath); //copies the home directory path to the *w buffer.
     numberOfCharacters = (int) strlen(w); //stores the number of characters of the home directory path. (strlen returns an unsigned int so we have to cast it to an int to avoid code warnings)
     index = numberOfCharacters; //Moves the index to the null terminator so we can write over with the characters that get read-in.  
     continue;
   } 
   
   if( (numberOfCharacters > 0) && (iochar == '~') ) //Handles when the '~' character is found in the middle of a word.
   {
     w[index] = iochar; //put the '~' character in the buffer. 
     index++; //moves the index 
     numberOfCharacters++; 
     continue;
   }
  //END of section.
  
  //START: This section handles all the non-meta-characters word deliminators (space, tab, or NL). 
    if( (numberOfCharacters > 0) && (iochar == ' ' || iochar == '\t' ) ) //Handles when a space or tab character is found after a word.    
    {
      if(startsWithADollarSign == 1) //Handles when a word with a '$' in front was ended by a space.
      {
        w[index] = '\0';
        return -(numberOfCharacters);   
      }
      w[index] = '\0'; //replaces the "space" character with a null terminator.
      return numberOfCharacters;
    }

    if( (numberOfCharacters == 0) && (iochar == ' ' || iochar == '\t') && (startsWithADollarSign == 0) ) //Handles leading spaces and tabs.
    {
      continue; 
    } 

    if( (numberOfCharacters > 0) && (iochar == '\n') ) //Handles when a NL character is found after a word.
    {
      if(startsWithADollarSign == 1) //Handles when a word with a '$' in front was ended by a NL.
      {
        ungetc(iochar, stdin); //Puts the "NL" character back on the input stream.
        w[index] = '\0';
        return -(numberOfCharacters);   
      }
      ungetc(iochar, stdin); //Puts the "NL" character back on the input stream.
      w[index] = '\0'; //Replaces the "NL" character with a null terminator.
      return numberOfCharacters;
    }

    if( (numberOfCharacters == 0) && (iochar == '\n') && (startsWithADollarSign == 0) )//This return the NL character when there is not a dollar sign in front.
    {
      w[index] = '\0'; //Replaces the "NL"  character with a null terminator.
      return 0;
    }
    //END of section

    //START: This section handles all meta-characters that are deliminators. 
    if( (numberOfCharacters > 0) && (iochar == '>' || iochar == '<' || iochar == '|' || iochar == '&') ) //This returns words after a meta-character is found.
    {
     if(startsWithADollarSign == 1) //Returns the negative value when a '$' is in front of a word that gets deliminated by a meta-character. 
      {
        ungetc(iochar, stdin); //Puts the meta-character back on the input stream so we don't lose it.
        w[index] = '\0';
        return -(numberOfCharacters);   
      } 
      ungetc(iochar, stdin); //Puts the meta-character back on the input stream so we don't lose it.
      w[index] = '\0'; //Null terminates the word.
      return numberOfCharacters; 
    } 

    if( (numberOfCharacters == 0) && (iochar == '>' || iochar == '|' || iochar == '&') ) //This returns the actual meta-character.
    {
      if(startsWithADollarSign == 1) //Returns a zero legnth word when a '$' is in front a meta-character that delimates a word.
      {
        ungetc(iochar, stdin); //Put the meta-character back.
        w[index] = '\0'; //Null terminates the word.
        return 0;
      }
    
      w[index] = iochar; //This puts the meta-character into the *w buffer.
      index++; //Moves the index
      w[index] = '\0'; // Null terminates the word.
      numberOfCharacters++;
      return numberOfCharacters;
    }
    
    if( (iochar == '<') && (numberOfCharacters == 0)  ) //returns the '<<' word.
    {
      if ( (adjacentCharacter = getchar() ) == '<') //if the adjacent character is another '<' character
      {
      if(startsWithADollarSign == 1) //Returns a zero length word when a '$' is in front of a '<<' character.
      {
        w[index] = '\0'; //Null terminates the word.
        ungetc(iochar, stdin); //put back the first '<' character 
        ungetc(adjacentCharacter, stdin); // put back the second '<'   
        return 0;
      }
      w[index] = iochar; //puts the first '<' meta-character into *w.
      index++; // moves the index 
      w[index] = adjacentCharacter; // puts the second '<' meta-character into *w.
      index++; //Moves the index
      w[index] = '\0'; //Null terminates the word.
      numberOfCharacters += 2; //Increases the number of characters by two.
      return numberOfCharacters;
      }
      
      if( (adjacentCharacter != '<') && (numberOfCharacters == 0) ) //returns the '<' word.
      {
       if(startsWithADollarSign == 1) //Retuns a zero length word when a '$' is in front of a '<' character.
       {
         ungetc(adjacentCharacter, stdin); //Put the character back on the input stream.
         ungetc(iochar, stdin); //Put the character back on the input stream.
         w[index] = '\0';
         return 0;
       }
        ungetc(adjacentCharacter, stdin); //Put the character back on the input stream.
        w[index] = iochar; //puts the '<' character in *w
        index++; //Moves the index
        w[index] = '\0';
        numberOfCharacters++;
        return numberOfCharacters;
      }
          
    }
     //END of section.

    //START: Handles the backslash cases.
    adjacentCharacter = getchar(); // Gets the adjacent character from the input stream.
    
    if( (iochar == '\\') && (numberOfCharacters > 0) ) //Handles when a backslash is found in the middle of a word.
    {
    
       if( (adjacentCharacter == '\n') ) //This handles the backslash-newline case. 
       {
         iochar = ' '; //replaces the NL character with a space character 
         ungetc(iochar, stdin); //This puts the spaces character on the input stream.
         continue; 
       }
       
       if( (adjacentCharacter == '<' || adjacentCharacter == '>' || adjacentCharacter == '&' || adjacentCharacter == '|' || adjacentCharacter == '~' || adjacentCharacter == '$' || adjacentCharacter == '\\'        || adjacentCharacter == ' ') ) //when the next character a backslash is a meta-character or one of the special backslash cases 
       {
         w[index] = adjacentCharacter; // replace the '\' with the other meta-character.
         index++; //moves the index 
         numberOfCharacters++;
         continue; //finish getting the rest of the word. 
       }

       if( (adjacentCharacter != '<' || adjacentCharacter != '>' || adjacentCharacter != '&' || adjacentCharacter != '|' || adjacentCharacter != '~' || adjacentCharacter != '$' || adjacentCharacter != '\\'        || adjacentCharacter != ' ') )// when the adjacent character is not a meta-character  put it back on the input stream.
       {
         ungetc(adjacentCharacter, stdin); //puts the adjacent character back when its not a meta-character.
         continue; //finish getting the rest of the word. 
      }

    }
 
    if( (iochar == '\\') && (numberOfCharacters == 0) ) //Handles when a backslash is found in the beginning of a word.
    {
      if( (adjacentCharacter == '\n') ) //This handles the backslash-newline case. 
      {
         iochar = ' '; //replaces the NL character with a space character 
         ungetc(iochar, stdin); //This puts the spaces character on the input stream.
         continue; 
      }
    
      if( (adjacentCharacter == '<' || adjacentCharacter == '>' || adjacentCharacter == '&' || adjacentCharacter == '|' || adjacentCharacter == '~' || adjacentCharacter == '$' || adjacentCharacter == '\\'        || adjacentCharacter == ' ') ) //when the next character a backslash is a meta-character or one of the special backslash cases
      {
        w[index] = adjacentCharacter; // replace the '\' with the other meta-character.
        index++; //moves the index 
        numberOfCharacters++;
        continue; //finish getting the rest of the word. 
      }

      if( (adjacentCharacter != '<' || adjacentCharacter != '>' || adjacentCharacter != '&' || adjacentCharacter != '|' || adjacentCharacter != '~' || adjacentCharacter != '$' || adjacentCharacter != '\\'        || adjacentCharacter != ' ') )// when the adjacent character is not a part of one of the backslash cases put it back on the input stream.
      {
        ungetc(adjacentCharacter, stdin); //puts the adjacent character back when its not a meta-character.
        continue; //finish getting the rest of the word. 
      }

    }
    //END of section.
    
    //START: this section handle  putting non-meta-characters into the array.    
    if( (iochar != '>' && iochar != '<' && iochar != '&' && iochar != '|' && iochar != '$' && iochar != '~' && iochar != '\\' ) ) //This places non-meta characters in the *w buffer
    {
      w[index] = iochar; //This puts the read-in character in the *w buffer.
      index++; //Moves pointer to next address.
      numberOfCharacters++; //Increases the number of charcters count by 1.
      ungetc(adjacentCharacter, stdin); //puts the adjacent character when it isn't a meta-character (because getchar() gets called when checking the backslash cases so we have to put it back)
    }
    //END of section. 

 }//END of while loop.

    /*START: This section handles all of the EOF cases. */
    if( (numberOfCharacters > 0) && (startsWithADollarSign == 1) ) // Handles when '$' is first the character and returns the neagtive value. (The premature EOF case.)    
    {
      w[index] = '\0'; //Puts a null terminator to "end" the word.
      return -(numberOfCharacters); //Returns the negative number of characters. 
    }

    if( (startsWithADollarSign == 1) && (iochar == EOF) ) //Handles when a '$' is followed by EOF.
    {
      w[index] = '\0';
      return 0;
    }

    if( (numberOfCharacters > 0) && (iochar == EOF) ) //Handles when EOF is found after a word. 
    {
      w[index] = '\0'; //Replaces the EOF signal with a null terminator.
      return numberOfCharacters;
    }

    if( (numberOfCharacters == 0) && (iochar == EOF) ) //Handles the case when all characters have been read-in and the last thing left in the input is EOF and returns -255 to end the program.
    {
      w[index] = '\0'; // Replaces the EOF signal with a null terminator.
      return -255;
    }
    //END of section.   

}//END of getword function.
