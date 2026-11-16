#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

typedef struct // making am stuct and nicknaming Inode
{
	uint32_t Inode; // the long inode that belongs to each file/dir
	uint32_t ParentInode; // the inode that lead to it
	char type; // is it a "file" or "dir"
	char name[32]; // what is the char name of that "file"
} Inode;

Inode InodeList[1024]; // 1024 is the max amount of Inodes we can store and InodesList is where they will be stored
size_t InodeCount =0; // this is how many Inodes are in our array list
uint32_t CurrentInode=0; // this is the current Inode that we are looking at in our array list
int file_saves = 0;

void loadinodeklist(); //this is what allows us to initially load out inode values (done)0
void saveinodelist(); // save before "closing" (done)
void changedirectory(const char *name); // function to use when user wants to change directories "cd"
void listcontents();// function to use when user wants to see what is in the current directory "ls"
void createdirectory(const char *name); // function to use when user wants to create a directory "mkdir"
void createfile(const char*name);// the function to use when user wants to create a new file "touch"

char *uint32_to_str(uint32_t i); //this function converts an unsigned 32 bit integer into a string 

int main(int argc, char*argv[]) // we are taking command line argument from the user to let us know which directory to use
{	
	char command[40];

	if(argc!=2) // we are checking to see if the user provided some file directory, the which is the second
		    // command line arugment index 1
	{
		printf("please provide the director to start in\n");
		exit(1);
	}
	if(chdir(argv[1])==0) // if we are able to change into that directory continue onward
	{
		loadinodeklist(); // if we were able to open the directory then load the inodes_list       
	}

	else // there was an issue loading into that directory
	{
			printf("There was an error with the directory\n");
			return 1; 
	}
	FILE *usr = stdin; // file pointer that will get input from user
	while ((fgets(command,40,usr)) !=NULL) // So long as the user doesn't use  cntrl+d or if there was no stream error the loop will continue 
	{
		
		char *argument_first_split = strchr(command,'\n'); // using strchr we are looking for the first instance of '\n' and replacing it with a null terminator
                        if ((argument_first_split)!=NULL)
                        {
                                *argument_first_split ='\0';
                        }

		if (strcmp(command,"ls")==0) // if the user typed in "ls" the list command function will launch
                {
                        listcontents();
                }

		 else if(strcmp(command,"exit")==0) // if they did not type "ls" check if they typed "exit"
		 {
		 	saveinodelist(); // if the user uses exit, save their progress and end the program.
			return 0;
		 }

		else // if not "ls" or "exit", then they must be using "cd","touch",or "mkdir""
		{
			 char *argument_word_split = strchr(command,' '); // using strchr we are looking for ' ' and replacing it with '\0' which will give turn the string we got from the
									  // user into two seperate pointers that we can use to identify two seperate words
                	if ((argument_word_split)!=NULL)
                	{
                        	*argument_word_split ='\0';
                	

				 if (strcmp(command,"mkdir")==0) // check if the command the user typed is "mkdir"
                		{	
						
                        		createdirectory(argument_word_split+1); // off set the pointer by 1 to point to second word the user provided
					
				}

                		if (strcmp(command,"touch")==0) // check if the user typed "touch"
                		{	
					
                        		createfile(argument_word_split+1);
					
                		}

                		if (strcmp(command,"cd")==0) // check if they typed "cd"
                		{
                        			changedirectory(argument_word_split+1);
                		}
			}

		}
        	
	}

	if(feof(usr)) // when the user uses "cntrl+d" to end stdin, save their progress
                {
                	saveinodelist(); //save and exit program
                }
              else if (ferror(usr))
                {       
                        perror("Steam error");
                }

	return 0;
}
void loadinodeklist() // Function for loading our inode list, we only need to run it once, every root directory that the user provides is assumed to have an "inodes_list"  
{
	FILE *fp =fopen("inodes_list","rb"); // we are opening the file "inodes_list" that is assumed to be in the directory the user provided, we are reading it now, but we will
        if (fp==NULL) // error check to see if there was an issue attempting to open the file
        	{
                                printf("We were not able to open the file\n");
                                return;
                }	
	  size_t ct =(fread(&InodeList,sizeof(Inode),1024,fp)); //using fread we are passing the contents of Inodes list which we got from our file pointer, which is assumed to always be present
								//in the root directory that the user provides. We read it in terms of structs up to 1024 times, but we will only record how may there was
								//
		InodeCount = ct; // our inode count is populated initially by how many 
				
	if (ferror(fp)) // error checking to make sure that no error occured will reading. 
	{
		printf("there was an issue\n");

	}
	

	if('d'!=InodeList[0].type) // checking to see if the root "file" aka inode 0 is a directory or not, if not don't proceedd forward
	{
	
	printf("Inode 0 is not a directry\n");
	
	}
	fclose(fp);// we will close the file in another function where we save to the file first before closing it
}

void saveinodelist () // once the user wishes to exit the program, we have to save the changes that they made
{

	FILE *fp =fopen("inodes_list","wb" ); // we are opening the linodes list in writting binary mode
	 if (fp==NULL) // error check to see if there was an issue attempting to open the file
                        {
                                printf("We were not able to open the file\n");
                                return;
                        }

size_t wcheck =fwrite(InodeList,sizeof(Inode),InodeCount,fp); // write inode structs back into linod_list

	if (wcheck != InodeCount)
	{
	
	printf("There was an issue with copying over every struct\n");
	
	}

	fclose(fp); // now that we finished writing the contents we are closing the file properly
}

void changedirectory(const char *name) // function for chaning into the directory that the user specified
{
	int count;
	int chng=0;
	for (count =0;count<InodeCount; count = count + 1 ) // we loop through each of our structs that we have in our buffer
	{
		if (strcmp(name,InodeList[count].name)==0) // we are comparing the string provided by the user with 
							   // the name that we have stored in the "current" index , if they are the
							   // same then we will store that inode value into our global Current Inode
							   // variable
		{

			if('d'==InodeList[count].type)	// check to see if type is d, as an extra safety meassure. 
			{
				CurrentInode = InodeList[count].Inode;
		 		chng++;
			}
		}	
	}              				  // if there is no match then the directory does not exist
	
	if(chng<1)
	{	
		printf("That Directory does not exist\n");
	}
	

}

void listcontents()
{	char buffer[100]; 
	int count;
	for (count =0;count<InodeCount; count = count + 1 ) // we have a loop to go through each inode and inspect who it's pareent was
	{


	 	if (CurrentInode==InodeList[count].ParentInode)
	 	{	
			sprintf(buffer,"inode: %d, type: %c, name: %s",count,InodeList[count].type,InodeList[count].name);
	 		printf("%s\n",buffer);
	 	}

	}
	
}

char *uint32_to_str(uint32_t i) // function for converting unit32 to a string
{
	int length = snprintf(NULL, 0, "%u", i);
	if (length < 0) return NULL;
	char* str = (char*) malloc(sizeof(char) * (length + 1));
	if (str == NULL) return NULL;
	snprintf(str, length + 1, "%u", i);
	return str;
}


void createdirectory(const char *name)
{	
	char buff[100]; 
	int count; 
	for (count =0;count<InodeCount; count = count + 1 )
	{
	 	if (strcmp(name,InodeList[count].name)==0 &&InodeList[count].ParentInode==CurrentInode) // we are comparing the string provided by the user with 
                                                           // the name that we have stored in the "current" index , in addition if we also check if that conflict i happening in the same directory
                {
			printf("A directory with this name already exist\n");
                	 return;
                }
        }

	if (InodeCount>=1024)
	{
		return;
	}
							// values to be based on the information that the user provided. 
	InodeList[InodeCount].Inode=InodeCount;
	strcpy(InodeList[InodeCount].name,name);
	InodeList[InodeCount].type='d';
	InodeList[InodeCount].ParentInode=CurrentInode;
	
	char *crrtinode = uint32_to_str(InodeList[InodeCount].Inode);
	char *parentinode = uint32_to_str(InodeList[InodeCount].ParentInode);

	FILE *fp =fopen(crrtinode,"w"); // we create a new file and give it the name that the user provided
        sprintf(buff,".%s\n..%s\n",crrtinode,parentinode);  
       	fprintf(fp,"%s",buff);	// 
        fclose(fp);

	InodeCount++;

	free(parentinode);
	parentinode=NULL;
	free(crrtinode);
	crrtinode=NULL;
}


void createfile(const char*name)
{
	int count;
	for (count =0;count<InodeCount; count = count + 1 )
        {
                if (strcmp(name,InodeList[count].name)==0 &&InodeList[count].ParentInode==CurrentInode) // we are comparing the string provided by the user with 
                                                           // the name that we have stored in the "current" index  that we are cyceling through in our loop, we are also checking 
							   // if they are in the same directory, if there is a match we are unable
							   //  to creata new file with that exact name and don't move forward 
                {
                        printf("This file already exist\n"); 
			return;
                }
        }

	if (InodeCount>=1024) // checking to see if space exist to store a few file
        {
		printf("There is not enough space\n");
                return;
        }
        InodeList[InodeCount].Inode=InodeCount; // it's Inodes correpsonds to the current number of Inodes in the list 
        strcpy(InodeList[InodeCount].name,name); // we give it the name that user provided
        InodeList[InodeCount].type='f'; // type f since it's a file
        InodeList[InodeCount].ParentInode=CurrentInode; // which ever dirrectory we are in is the parent of the file we are making. 
        
	char *crrtinode = uint32_to_str(InodeList[InodeCount].Inode);
	// still need to assign an inode list number to this item

	FILE *fp =fopen(crrtinode,"w"); // we create a new file and give it the name that the user provided
	fprintf(fp,"%s",name); // we are printing the file name into the file we just created
	fclose(fp); // closing the file since it is not needed at the moment
	InodeCount++; // once it has all it's inforamtion we can now incremeant our count since we created a new inode

	free(crrtinode); // freeing the heap
        crrtinode=NULL;
}
