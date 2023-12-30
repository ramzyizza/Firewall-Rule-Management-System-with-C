#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct firewallRule_t {
    int ipaddr1[4];
    int ipaddr2[4];
    int port1;
    int port2;
};

struct firewallRules_t {
    struct firewallRule_t *rule;
    struct firewallRules_t *next;
};
    
struct ruleErrors_t  {
    char * line;
    struct ruleErrors_t *next;
};

void printIPaddress (int *ipaddr) {
	printf ("%d.%d.%d.%d", ipaddr[0],
		ipaddr[1],
		ipaddr[2],
		ipaddr[3]);
}

int compareIPAddresses (int *ipaddr1, int *ipaddr2) {
    int i;
    for (i = 0; i < 4; i++) {
	if (ipaddr1[i] > ipaddr2[i]) {
	    return 1;
	}
	else if (ipaddr1[i] < ipaddr2[i]) {
	    return -1;
	}
    }
    return 0;
}

void printRule (struct firewallRule_t *rule) {
	printf ("Rule: %d.%d.%d.%d", rule->ipaddr1[0],
		rule->ipaddr1[1],
		rule->ipaddr1[2],
		rule->ipaddr1[3]);
	if (rule->ipaddr2[0] != -1) {
	    	printf ("-");
		printIPaddress(rule->ipaddr2);
	}
	printf (" %d", rule->port1);
	if (rule->port2 != -1) {
	    printf ("-");
	    printf ("%d", rule->port2);
	}
	printf ("\n");
}

/* parses one IP address. Returns NULL if text does not start with a valid IP address, and a pointer  to the first character after the valid IP address otherwise */
char *parseIPaddress (int *ipaddr, char *text) {
    char *oldPos, *newPos;
    long int addr;
    int i;

    oldPos = text;
    for (i = 0; i <4; i++) {
	if (oldPos == NULL || *oldPos < '0' || *oldPos > '9') {
	    return NULL;
	}
	addr = strtol(oldPos, &newPos, 10);
	if (newPos == oldPos) {
	    return NULL;
	}
	if ((addr < 0)  || addr > 255) {
	    ipaddr[0] = -1;
	    return NULL;
	}
	if (i < 3) {
	    if ((newPos == NULL) || (*newPos != '.')) {
		ipaddr[0] = -1;
		return NULL;
	    }
	    else newPos++;
	}
	else if ((newPos == NULL) || ((*newPos != ' ') && (*newPos != '-'))) {
	    ipaddr[0] = -1;
	    return NULL;
        }
	ipaddr[i] = addr;
	oldPos = newPos;
    }
    return newPos;
}

int compareRules (const void *arg1, const void *arg2) {
    struct firewallRules_t *rule1, *rule2;

    
    rule1 = *((struct firewallRules_t **) arg1);
    rule2 = *((struct firewallRules_t **) arg2);
    if (rule1->rule->port1 < rule2->rule->port1) {
	return -1;
    }
    else if (rule1->rule->port1 > rule2->rule->port1) {
	return 1;
    }
    else 
	return (compareIPAddresses (rule1->rule->ipaddr1, rule2->rule->ipaddr1));
}

										  
struct firewallRules_t *sortRules(struct firewallRules_t *rules, int noOfRules) {
    struct firewallRules_t **allRules, **tmp, *sortedRules; 
    int i;

    /* empty list is already sorted; rest of the function assumes noOfRules > 0 */
    if (noOfRules == 0) {
	return NULL;
    }
    
    allRules = malloc(sizeof (struct firewallRules_t *) * noOfRules);
    tmp = allRules;
    while (rules) {
	*tmp = rules;
	tmp++;
	rules = rules->next;
    }
    qsort (allRules, noOfRules, sizeof(struct firewallRules_t *), compareRules);

    for (i = 0; i < noOfRules-1; i++) {
        allRules[i]->next = allRules[i+1];
    }
    allRules[noOfRules -1]->next = NULL;
    
    sortedRules = allRules[0];
    free(allRules);
    return sortedRules;
}

char *parsePort (int *port, char *text) {
    char *newPos;

    
    if ((text == NULL) || (*text < '0') || (*text > '9')) {
	 return NULL;
     }
    *port = strtol(text, &newPos, 10);
    if (newPos == text) {
	*port = -1;
	return NULL;
    }
    if ((*port < 0) || (*port > 65535)) {
	*port = -1;
	return NULL;
    }
    return newPos;
}
	

struct firewallRule_t *readRule (char * line) {
    struct firewallRule_t *newRule;
    char *pos;

    // parse IP addresses 
    newRule = malloc (sizeof(struct firewallRule_t));
    pos = parseIPaddress (newRule->ipaddr1, line);
    if ((pos == NULL) || (newRule->ipaddr1[0] == -1)) {
	free (newRule);
	return NULL;
    }
    if (*pos == '-') {
	// read second IP address
	pos = parseIPaddress (newRule->ipaddr2, pos+1);
	if ((pos == NULL) || (newRule->ipaddr2[0] == -1)) {
	    free (newRule);
	    return NULL;
	}
    
	if (compareIPAddresses (newRule->ipaddr1, newRule->ipaddr2) != -1) {
	    free(newRule);
	    return NULL;
	}
    }
    else {
	newRule->ipaddr2[0] = -1;
    }
    if (*pos != ' ') {
	free(newRule);
	return NULL;
    }
    else pos++;

    // parse ports
    pos = parsePort (&(newRule->port1), pos);
    if ((pos == NULL) || (newRule->port1 == -1)) {
	free(newRule);
	return NULL;
    }
    if ((*pos == '\n') || (*pos == '\0')) {
	newRule->port2 = -1;
	return newRule;
    }
    if (*pos != '-') {
	free(newRule);
	return NULL;
    }
    
    pos++;
    pos = parsePort (&(newRule->port2), pos);
    if ((pos == NULL) || (newRule->port2 == -1)) {
	free(newRule);
	return NULL;
    }
    if (newRule->port2 <= newRule->port1) {
	free(newRule);
	return NULL;
    }
    if ((*pos == '\n') || (*pos == '\0')) {
	return newRule;
    }
    free(newRule);
    return NULL;
}

struct ruleErrors_t *allErrors = NULL;

struct ruleErrors_t *addErrorRule (struct ruleErrors_t *errors, char * line) {
    struct ruleErrors_t *newError;

    newError = malloc(sizeof(struct ruleErrors_t));
    newError->line = line;
    newError->next = errors;
    return newError;
}


struct firewallRules_t * addRule (struct firewallRules_t * rules, struct firewallRule_t *rule) {
    struct firewallRules_t *newRule;

    newRule = malloc(sizeof(struct firewallRules_t));
    newRule->rule = rule;
    newRule->next = rules;
    return newRule;
}
    
	
struct firewallRules_t *readFile (char *filename) {
    FILE *file;
    int result;
    char *line = NULL;
    size_t lineSize;
    struct firewallRule_t *newRule;
    char *errorLine;
    struct firewallRules_t * allRules = NULL;
    int noOfRules = 0;

    file = fopen (filename, "r");
    if (file == NULL) {
	fprintf (stderr, "Could not open file, exiting!\n");
	exit (1);
    }

    while ((result = getline(&line, &lineSize, file)) != -1) {
        newRule = readRule(line);
	if (newRule == NULL) {
	    errorLine = malloc(lineSize+1);
	    strcpy(errorLine, line);
	    allErrors = addErrorRule(allErrors, errorLine);
	}
	else {
	    allRules = addRule(allRules, newRule);
	    noOfRules++;
	}
    }
    free(line);
    fclose(file);
    allRules = sortRules(allRules, noOfRules);
    return allRules;
}



int main (int argc, char **argv) {
    char *filename;
    struct firewallRules_t *allRules = NULL, *tmp;
    struct ruleErrors_t *next;

    
    

    if (argc != 2) {
	fprintf (stderr, "Exactly one argument required, exiting!\n");
	exit (1);
    }

    filename = argv[1];
    allRules = readFile(filename);

    // print and free all rules
    while(allRules) {
	tmp = allRules->next;
	printRule(allRules->rule);
	free(allRules->rule);
	free(allRules);
	allRules = tmp;
    }

    // print and free all errors 
    while(allErrors) {
	next = allErrors->next;
	printf ("Ill-formed rule: %s", allErrors->line);
	free(allErrors->line);
	free(allErrors);
	allErrors = next;
    }
    return 0;
}
