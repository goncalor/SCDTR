#include <stdlib.h>
#include <ctype.h>


/* Turns spaces in 'str' into \0s and and writes a list of pointers to the
 * beginning of words into 'splitlst'.  Only the first 'splitlstlen' words are
 * written into 'splitlst'. 'splitlstlen' must be at most the length of
 * 'splitlst'.  The unused positions in 'splitlst' will be NULL.  Returns the
 * number of words read. Returns at most 'splitlstlen'. */
short split(char *str, char **splitlst, short splitlstlen)
{
    short lstindex;
    char *c;
    bool prevnotspace = false;   // whether the prev char was a space

    // intialise 'splitlst' with NULLs
    for(lstindex=0; lstindex < splitlstlen; lstindex++)
        splitlst[lstindex] = NULL;

    c = str;
    lstindex = 0;
    // while \0 is not found and read less than 'splitlstlen'
    while(*c && lstindex < splitlstlen)
    {
        if(isspace(*c))
        {
            // terminate previous string
            *c = '\0';
            if(prevnotspace)
                lstindex++;
            prevnotspace = false;
        }
        else
        {
            if(!prevnotspace)
                splitlst[lstindex] = c;
            prevnotspace = true;
        }

        c++;
    }

    // case of string ...xyz\0
    if(prevnotspace)
        lstindex++;

    return lstindex;
}

