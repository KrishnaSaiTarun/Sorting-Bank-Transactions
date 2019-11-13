#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>


#include "cs402.h"

#include "my402list.h"

typedef struct Transaction_input
{
	char type;
	unsigned int timestamp;
	int amount;
	char description[1024];
} T_input;

int line_number=0;
My402List listaddr;
int balance=0;


int CommandLineIn(int argc, char* argv[])
{
	struct stat check_dir;

	if(argc<2 || argc>3)
	{
		fprintf(stderr, "Error: %s\n", "Malformed command");
		return FALSE;
	}

	else if(strcmp(argv[1] , "sort") != 0)
	{
		fprintf(stderr, "Error: %s\n", "Malformed command");
		return FALSE;		
	}
	
	if(argc == 3)
	{
		if(stat(argv[2], &check_dir)!=0)
		{
			fprintf(stderr, "Error: %s\n", "File Cannot be retrieved.");
			return FALSE;		
		} 
       	
   		if( S_ISDIR(check_dir.st_mode) == 1)
		{
			fprintf(stderr, "Error: %s\n", "It is a directory");
			return FALSE;		
		}
	}
	

	return TRUE; 
}

void checkType(char* type)
{

	if(strlen(type)!= 1)
	{
		if(strlen(type) == 0)
		{
			fprintf(stderr, "File line number: %d : Error: Transaction type must be specified\n", line_number);
			exit(1)	;	
		}

		fprintf(stderr, "File line number: %d : Error: Invalid size of transaction type\n", line_number);
		exit(1)	;	
	}	
	
	if(strcmp(type,"+") !=0 && strcmp(type,"-") !=0)
	{
		fprintf(stderr, "File line number: %d : Error: Invaid transaction type\n", line_number);
		exit(1);
	}
}

void checkTime(char* timer)
{
	if(strlen(timer)>10)
	{
		fprintf(stderr, "File line number: %d : Error: Invaid length of timestamp\n", line_number);
		exit(1);		
	}

	if(strlen(timer) == 0)
	{
		fprintf(stderr, "File line number: %d : Error: Timestamp cannot be empty\n", line_number);
		exit(1);		
	}

	unsigned int current = time(NULL);
	unsigned int t = atoi(timer);

	if(current < t)
	{
		fprintf(stderr, "File line number: %d : Error: Time mentioned is more than the current time\n", line_number);
		exit(1);
	} 
	//printf("The current time stamp is: %d\n", current);
}

void checkAmount(char* amt)
{
	int i=0, j=0, k=0;

	if(strlen(amt) == 0)
	{
		fprintf(stderr, "File line number: %d : Error: Amount cannot be empty\n", line_number);
		exit(1);		
	}

	while(amt[i]!= '\0')
	{
		if(amt[i] >= '0' && amt[i] <= '9')
		i++;

		else if(amt[i] == '.')
		{
			j=i;
			i++;	
			k=i;		
		}

		else
		{
			fprintf(stderr, "File line number: %d : Error: Invaid Amount type, Amount need to be in numbers\n", line_number);
			exit(1);
		}
	}
	
	if(j==0)
	{
		fprintf(stderr, "File line number: %d : Error: There has to be a decimal point in the amount with 2 digits after the decimal point\n", line_number);
		exit(1);
	}
	if(j>7)
	{
		fprintf(stderr, "File line number: %d : Error: Amount too big\n", line_number);
		exit(1);
	}

	if(i-k != 2)
	{
		fprintf(stderr, "File line number: %d : Error: There has to be 2 digits after decimal point in amount\n", line_number);
		exit(1);
	}
}

void checkDes(char* des)
{
	if(strlen(des)>1024)
	{
		fprintf(stderr, "File line number: %d :Error: Description too long\n", line_number);
		exit(1);
	}

	if(strlen(des)==0)
	{
		fprintf(stderr, "File line number: %d :Error: Description required, Cannot be empty\n", line_number);
		exit(1);
	}
}


int processAmount(char* trans_amount)
{
	char cents[strlen(trans_amount)];
	int i=0, j=0;
	
	while(trans_amount[i]!='\0')
	{
		if(trans_amount[i] == '.')
		i++;
		
		cents[j] = trans_amount[i];
		i++;
		j++;
	}
	
	cents[j] = '\0';

	int int_cents = atoi(cents);
	if(int_cents<0)
	{
		fprintf(stderr, "File line number: %d :Error: The amount can not be negative\n", line_number);
		exit(1);
	}
	return int_cents;	
}

void insertElement(My402List* firstaddr, T_input* in)
{
	
	if(My402ListEmpty(firstaddr) == 1)
	{

		if(My402ListPrepend(firstaddr, (void *)in) == 0)
		{
			fprintf(stderr, "Prepending error: Could not add element to the Linked List\n");
			exit(1);
		}
	}
	
	else
	{

	My402ListElem* temp = My402ListFirst(firstaddr);

	while(temp!= NULL)
	{	
		T_input* object = (T_input *)temp->obj;

		//printf("%c\n%d\n%d\n%s\n", object->type, object->timestamp, object->amount, object->description);


		if(object->timestamp > in->timestamp)
		{
			if(My402ListInsertBefore(firstaddr, (void *)in, temp) == 0)
			{
				fprintf(stderr, "Insert Before: error: Could not add element to the Linked List\n");
				exit(1);			
			}

			break;
		}

		if(object->timestamp == in->timestamp)
		{
			fprintf(stderr, "File line number: %d: Error : Two timestamps can not be same\n", line_number);
			exit(1);			
		}

		temp = My402ListNext(firstaddr, temp);
	}

	if(temp == NULL)
	{
		if(My402ListAppend(firstaddr, in) == 0)
		{
			fprintf(stderr, "Append: error: Could not add element to the Linked List\n");
			exit(1);
		}
		
	}
	}

	
}

void print_table_margin()
{
	fprintf(stdout, "+-----------------+--------------------------+----------------+----------------+\n");
}

void print_table_head()
{
	fprintf(stdout, "|       Date      | Description              |         Amount |        Balance |\n");
}

void finalTime(int val)
{
	//printf("The length of the string is: %d\n", strlen(timestring));
	//char* print_time = (char *)malloc(sizeof(timestring));
	char* timestring = ctime((const long int*)&val);
	char print_time[20];
	int i=0, j=0;

	while(i<10)
	{
		print_time[j] = timestring[i];
		j++;
		i++;
	}
	
	
	i = strlen(timestring) - 6;

	while(i<strlen(timestring))
	{
		print_time[j] = timestring[i];
		j++;
		i++;
	}
	
	print_time[j] = '\0';
	i=0;

	fprintf(stdout, "| ");
	
	while(print_time[i]!='\n')
	{
		fprintf(stdout, "%c",print_time[i]);
		i++;
	}
	
	fprintf(stdout, " | ");
	
}

void finalDes(char* des)
{
	
	//printf("%s", des);
	char str[24];
	des[strlen(des)-1] = '\0';
	snprintf(str, 25, "%-24s", des);

	fprintf(stdout, "%s |", str);
}

void finalAmount(char sign, int val)
{	
	
	if(val>=1000000000)
	{
		if(sign == '+')
		fprintf(stdout,"  ?,???,???.??  |");

		else
		fprintf(stdout," (\?,\?\?\?,\?\?\?.\?\?)  |");
	}
	
	else
	{
	int temp = val;
	char str[15];
	int v;
	int j;
	int i=14;
	str[i] = '\0';
	//i--;
	//str[i] = ' ';
	
	i = 12;
	int k = i;
	while(temp!=0)
	{	
			
			v = temp%10;
			temp = temp/10;
			if(k-i == 2)
			{
				str[i] = '.';
				i--;
				j = i;
			}

			if(j - i == 3)
			{
				str[i] = ',';
				i--;
				j = i;
			}
			
			str[i] = '0' + v; 
			i--;
	}
	if(i == 10)
	{
		str[i] = '.';
		i--;
		str[i] = '0';
		i--;
	}
	while(i>0)
	{
			str[i]=' ';
			i--;
	}
	
	if(sign == '+')
	{
		str[13] = ' ';
		str[0] = ' ';
	}
	else
	{
		str[13] = ')';
		str[0] = '(';
	}
	
	
	//| (       45.33) |


	fprintf(stdout, " %s |", str);	
	}
	
	
	
}

void finalBalance(char type, int val)
{
	char balance_sign;
	int balance_current = 0;

	if(type == '+')
	balance = balance + val;
		
	else 
	balance = balance - val;

	balance_current = balance;

	if(balance<0)
	{
		balance_sign = '-';
		balance_current = balance * (-1);
	}
	else 
	balance_sign = '+';
 
	finalAmount(balance_sign, balance_current);

	fprintf(stdout,"\n");
	

}

void printSort(My402List* firstaddr)
{
	print_table_margin();
	print_table_head();
	print_table_margin();	
	//printf("\n\n\n\n\n\n\n");
	//printf("The sorted list is:\n\n");
	
	//char* timestring;

	My402ListElem* temp = My402ListFirst(firstaddr);

	while(temp!= NULL)
	{	
		T_input* object = (T_input *)temp->obj;
		//printf("The object timestamp is: %d\n", object->timestamp);

		
		//timestring = ctime((const long int*)&(object->timestamp));
		//printf("The object timestring is: %s\n", timestring);

		//finalTime(timestring);
		finalTime(object->timestamp);

		//printf("Description to be printed is: %s\n", object->description);
		finalDes(object->description);

		finalAmount(object->type, object->amount);

		finalBalance(object->type, object->amount);


		temp = My402ListNext(firstaddr, temp);
	}
	
	print_table_margin();
	//fprintf(stdout, "\n");
}


void ProcessLine(char* line)
{	
	char* type = strtok(line, "\t");
	int count = 0;
	if(type != NULL)
	count++;


	char* trans_time = strtok(NULL, "\t");	
	if(trans_time!= NULL)	
	count++;

	char* trans_amount = strtok(NULL, "\t");
	if(trans_amount!=NULL)	
	count++;

	char* des = strtok(NULL, "\t");
	if(des!=NULL)
	count++;

	char* check = strtok(NULL, "\t");
	if(check!=NULL)
	{
		fprintf(stderr, "File line number: %d: Error : Too many fields defined\n", line_number);
		exit(1);		
	}
	
	if(count!=4)
	{
		fprintf(stderr, "Error: %s\n", "Wrong type/format of the file. Needs to be a tfile with 3 tabs defined");
		exit(1);
	}

	checkType(type);
	checkTime(trans_time);
	checkAmount(trans_amount);
	checkDes(des);

	//AddToStructure
	T_input* in;
	in = (T_input *)malloc(sizeof(T_input));
	if( in == NULL)
	{
		fprintf(stderr, "Error: Dynamic memory could not be allocated for File line number%d ", line_number);
		exit(1);
	}

	//Type: 
	in->type = *type;
	//printf("The character is : %c\n", in->type);

	//Timestamp
	unsigned int time = atoi(trans_time);
	in->timestamp = time;
	//printf("The interger timestamp is: %d\n", in->timestamp);

	//Amount
	int cents = processAmount(trans_amount);
	in->amount = cents;
	//printf("The cents is: %d\n", in->amount);

	//Description
	int i=0;
	while(i<strlen(des))
	{
		in->description[i] = des[i];
		i++;	
	}

	//in->description = des;
	//printf("The Description is: %s\n", in->description);

	//printf("The address of the structure is: %p\n\n", in);
	

	if(line_number == 1)
	{	
		if(My402ListInit(&listaddr) != 1)
		{
			fprintf(stderr, "Error Initializing the linked list\n");
			exit(1);
		}
	}


	insertElement(&listaddr, in);

	//printSort(&listaddr);

}


void ReadFile(FILE *f)
{

	char line[1026];

	while(fgets(line, sizeof(line), f)!=NULL)
	{
		line_number++;
		//printf("%s\n", line);
		ProcessLine(line);
	}
	
}

int main(int argc, char* argv[])
{
	FILE *f = NULL;

	if(CommandLineIn(argc,argv) == 1)
	{
		if(argc == 3)
		{
			f = fopen(argv[2], "r");
			//printf("File Opened\n\n");	
			if(f == NULL)
			{	
				fprintf(stderr, "File could not be opened: %s\n" ,strerror(errno));
				return 0;
			}
			ReadFile(f);
			fclose(f);	
			printSort(&listaddr);
			//printf("File Closed\n\n");		
		}

		else
		{
			ReadFile(stdin);
			printSort(&listaddr);
		}
	}
	return 0;

}
