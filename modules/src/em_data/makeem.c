/** A utility to convert the em definition table to generated code.
 *  This is more portable than the shell scripts that currently
 *  exist.
 *
 *  This tool should only be called if the em_table is changeed, it creates
 *  the following files:
 *   em_spec.h
 *   em_pseu.h
 *   em_pseu.c
 *   em_mnem.h
 *   em_mnem.c
 *   em_flag.c
 *
 *
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "array.h"



size_t trimwhitespace(char *out, size_t len, const char *str)
{
  if(len == 0)
    return 0;

  const char *end;
  size_t out_size;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
  {
    *out = 0;
    return 1;
  }

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;
  end++;

  // Set output size to minimum of trimmed string length and buffer size minus 1
  out_size = (end - str) < len-1 ? (end - str) : len-1;

  // Copy trimmed string and add null terminator
  memcpy(out, str, out_size);
  out[out_size] = 0;

  return out_size;
}


#define BUFFER_SIZE 4096

/** This section contains indexes to the different limits as well as important
 *  constant values */
#define SECTION_INDEXES  0
/** This section contains the "opcodes" for the different mnemonics */
#define SECTION_MNEMONICS SECTION_INDEXES+1
/** This section contains the "opcodes" for the em machine */
#define SECTION_OPCODES SECTION_MNEMONICS+1


static char inbuffer[BUFFER_SIZE];
static char buffer[BUFFER_SIZE];

static struct array section_index;
static struct array section_mnemonics;
static struct array section_opcodes;




/** Separates the string into its separate tokens,
 *  and fills up the column data structure accordingly.
 *
 *  maxcolumns indicates the maximum number of columns
 *  allowed in the text file.
 *
 */
struct array *newrow(char* buffer, int maxcolumns)
{
	char* pch = NULL;
	int i = 0;
	int count = 0;
	struct array *columns = NULL;

	 columns = calloc(1,sizeof(struct array));
	 if (columns == NULL)
	 {
		   fprintf(stderr,"Cannot allocate memory.\n");
		   exit(EXIT_FAILURE);
	 }


	   pch = strtok (buffer,"\t ");
	   if (pch == NULL)
	   {
		   fprintf(stderr,"Expecting %d columns for indexes.\n",maxcolumns);
		   exit(EXIT_FAILURE);
	   }
	   array_append(columns,strdup(pch));
	   count++;
	   for (i = count; i < maxcolumns; i++)
	   {
		   pch = strtok (NULL, "\t ");
		   if (pch == NULL)
		   {
			   fprintf(stderr,"Expecting %d columns for indexes.\n",maxcolumns);
			   exit(EXIT_FAILURE);

		   }
		   count++;
		   array_append(columns,strdup(pch));
	   }
	return columns;
}


void disposerow(struct array *row)
{
	int i = 0;
	char* str;
	for (i = 0; i < row->count; i++)
	{
		 str = (char*)row->item[i];
		 free(str);
		 row->item[i] = NULL;
	}
	free(row);
}




/** Copies a null terminated string,
 *  but allocates the memory.
 *
 */
char* strdup(const char* s)
{
	int length = strlen(s)+1;
	char* ptr = malloc(length);
	memcpy(ptr, s, length);
	return ptr;
}


int main(int argc, char *argv[])
{
 int i = 0;
 struct array *columns;
 int value;
 char *str;

 char c1;
 char c2;
 FILE* hfile;
 FILE* cfile;
 char str1[3] = {0,0,0};
 char str2[3] = {0,0,0};
 /** First opcode value, -1 is invalid. */
 int fmnem = -1;
 /** First pseudocode value, -1 is invalid. */
 int fpseu = -1;
 if (argc != 2)
 {
	 fprintf(stdout,"Requires em_table file and pathname\n");
	 return EXIT_FAILURE;
 }

 FILE *fd = fopen(argv[1],"r");
 int section = SECTION_INDEXES;
 fprintf(stdout,"Opening : %s\n",argv[1]);
 if (fd == NULL)
 {
	 fprintf(stderr,"Cannot find file.\n");
	 return EXIT_FAILURE;
 }
 while (fgets(inbuffer,BUFFER_SIZE,fd)!=NULL)
 {
	 /* First section, opcode limits. */
	 trimwhitespace(buffer,BUFFER_SIZE,inbuffer);
	 /** Next section */
	 if (strlen(buffer)==0)
	 {
		 section++;
		 continue;
	 }


	 if (section == SECTION_INDEXES)
	 {
		   /* Split the row text into columns. */
		   columns = newrow(buffer, 2);
	       array_append(&section_index, columns);
	 }
	 else
	 if (section == SECTION_MNEMONICS)
	 {
		     columns = newrow(buffer, 3);
	         array_append(&section_mnemonics, columns);
	 } else
	 if (section == SECTION_OPCODES)
	 {
	     columns = newrow(buffer, 3);
		 array_append(&section_opcodes, columns);
	 }
 }
 fclose(fd);

 /* process the indexes */
 hfile = fopen("em_spec.h","w");
 for (i = 0; i < section_index.count; i++)
 {
	 columns = (struct array*)section_index.item[i];
	 str = columns->item[0];
	 if (strcmp("fpseu",columns->item[0])==0)
	 {
		 fpseu = atoi(columns->item[1]);

	 }
	 if (strcmp("fmnem",columns->item[0])==0)
	 {
		 fmnem = atoi(columns->item[1]);

	 }
	 fprintf(hfile,"#define sp_%s\t %s\n",columns->item[0],columns->item[1]);
 }
 fprintf(hfile,"#define sp_lpseu\t% d\n",fpseu+section_mnemonics.count - 1);
 fprintf(hfile,"#define sp_lmnem\t %d\n",fmnem+section_opcodes.count - 1);
 fclose(hfile);


 /** Check validity of first indexes. */
 if ((fpseu == -1) || (fmnem == -1))
 {
	 fprintf(stderr,"Error fpseu or fmnem first index values are not defined.\n");
	 exit(EXIT_FAILURE);
 }

 /* process the pseudocodes */
 hfile = fopen("em_pseu.h","w");
 cfile = fopen("em_pseu.c","w");
 fprintf(cfile,"char em_pseu[][4] = {\n");
 for (i = 0; i < section_mnemonics.count; i++)
 {
	 columns = (struct array*)section_mnemonics.item[i];
	 str = columns->item[1];
	 value = atoi(str);
	 fprintf(hfile,"#define ps_%s\t%d\n",columns->item[0],value+fpseu);
	 fprintf(cfile,"  \"%s\",\n",columns->item[0]);
 }
 fprintf(cfile,"};\n");
 fclose(hfile);
 fclose(cfile);

 /* process the opcodes */
 hfile = fopen("em_mnem.h","w");
 cfile = fopen("em_mnem.c","w");
 fprintf(cfile,"char em_mnem[][4] = {\n");
 for (i = 0; i < section_opcodes.count; i++)
 {
	 columns = (struct array*)section_opcodes.item[i];
	 fprintf(hfile,"#define op_%s\t%d\n",columns->item[0],i+fmnem);
	 fprintf(cfile,"  \"%s\",\n",columns->item[0]);
 }
 fprintf(cfile,"};\n");
 fclose(hfile);
 fclose(cfile);

 /* Create the flag files from opcodes */
 cfile = fopen("em_flag.c","w");
 fprintf(cfile,"#include \"em_flag.h\"\n");
 fprintf(cfile,"char em_flag[] = {\n");
 for (i = 0; i < section_opcodes.count; i++)
 {
	 columns = (struct array*)section_opcodes.item[i];
	 /* 2nd column indicate the parameter format, always on 2 columns. */
	 if (strlen(columns->item[1])!=2)
	 {
		 fprintf(stderr,"Error opcode type characterstic should be on 2 characters.\n");
		 exit(EXIT_FAILURE);
	 }
	 str = (char*)columns->item[1];
	 c1 = toupper(str[0]);
	 c2 = toupper(str[1]);
	 str1[0] = c1;
	 str1[1] = '\0';
	 str2[0] = c2;
	 str2[1] = '\0';
	 if (c1 == '-')
		strcpy(str1,"NO");
	 if (c2 == '-')
	    strcpy(str2,"NO");
	 fprintf(cfile, "PAR_%s | FLO_%s,\n",str1,str2);
 }
 fprintf(cfile,"};\n");
 fclose(cfile);


 return EXIT_SUCCESS;

}
