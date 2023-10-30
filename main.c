#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/errno.h>

// Declare Variables
FILE *gInputFile;
const int gMaxNameLength = 81;
const int gMaxCandidates = 21; // Max possible   1 .. 21

// Define structures
struct candidate {
  char *name;
  int votes;
  bool isEliminated;
};

struct treeNode {
  int numVotes;
  bool path[gMaxCandidates];
  struct treeNode *branches[gMaxCandidates];
};

// Define Variables
int gNumCandidates; // Actually used
struct candidate gCandidates [gMaxCandidates];

// Declare Functions I/O
void usage (char *);

// Declare Functions - Candidate evaluation
int isWinner (void); // Done
void printWinner (int); // Done
int trailingVotes (void); // Done

// Declare Functions - Manipulate tree
void createTree (struct treeNode *); // Done

int getNumElections (void);
int getNumCandidates (void);
char *getCandidateName (void);
void getVotePath (void);
void processVote (struct treeNode *, int []);

void eliminateCandidate (struct treeNode *, int);


int main(int argc, char *argv[]) {

  // File Initialization
  if (argc != 2) usage (argv[0]);

  if (NULL == (gInputFile = fopen(argv[1], "r"))) {
    fprintf (stderr, "inputFile: %s: %s\n", argv[1], strerror(errno));
    exit (EXIT_FAILURE);
  }
  else {
    fprintf (stderr, "%s opened for reading.\n" , argv[1]);
  }
  
  // Create candidate vote tree
  struct treeNode *root = (struct treeNode*) calloc(1, sizeof(struct treeNode));
  createTree (root);


  
  // Process votes - Fill tree
  int numElections = getNumElections(); printf ("numElections = %d\n", numElections);
  for (int i = 0; i < numElections; i++) {
    gNumCandidates = getNumCandidates(); printf ("gNumCandidates = %d\n", gNumCandidates);
  }
  for (int i = 1; i < gNumCandidates; i++) {
    gCandidates[i].name = getCandidateName();
    printf ("gCandidates[%d].name = %s\n", i, gCandidates[i].name);
  }
//    while (true) {
//      getVotePath();
//      processVote ();
//      break;
//    }


  // Determine and declare winner
  int winner = isWinner();
  if (winner != 0) {
    printWinner(winner);
  }
  
  // Eliminate trailing candidate(s)
  int minVotes = trailingVotes();

  for (int i = 0; i < gNumCandidates; i++) {
    if (gCandidates[i].votes == minVotes) {
      gCandidates[i].isEliminated = true;
      eliminateCandidate(root, i);
    }
  }
      

  // Close File
  fclose (gInputFile);
  
  return 0;
}


// Define Functions
void usage (char *cmd) {
  fprintf (stderr, "usage: %s inputFileName\n", cmd);
  exit (EXIT_SUCCESS);
}

int isWinner (void) {
  // Returns the ID of the winning candidate if there is a winner 
  // else returns 0 to indicate no winner
  
  int leader = 0;
  int gNumVotes = 0;
  for (int i = 1; i < gNumCandidates; i++) {
    if (gCandidates[i].isEliminated == false) {
      gNumVotes += gCandidates[i].votes;
    }
  }

  for (int i = 1; i < gNumCandidates; i++) {
    if (gCandidates[i].isEliminated == false) {
      if (gCandidates[i].votes > (double) gNumVotes / 2) {
        leader = i;
      }
    }
  }

  return leader;
}

void printWinner (int ID) {
  printf ("%s\n", gCandidates[ID].name);
}

void createTree (struct treeNode * tr) {
  for (int i = 1; i < gNumCandidates; i++) {
    if (tr->path[i] != true) {
      tr->branches[i] = (struct treeNode*) calloc(1, sizeof(struct treeNode));
      createTree (tr->branches[i]);
    }
    else {
      tr->branches[i] = NULL;
    }
  }
}

int getNumElections (void) {
  char c = fgetc(gInputFile);
  fgetc(gInputFile); fgetc(gInputFile); // strip newline x2
  return c - '0';
}

int getNumCandidates (void) {
  int c = fgetc(gInputFile);
  fgetc(gInputFile); // strip newline x1
  return c - '0' + 1;
}

char *getCandidateName (void) {
  char *name = (char *) calloc (1, 81*sizeof(char));
  fgets(name, gMaxNameLength, gInputFile);
  if (name[strlen(name)-1] == '\n') {
    name[strlen(name)-1] = '\0';
  }
  return name;
}

int trailingVotes (void) {
  int minVotes = INT_MAX;
  for (int i = 0; i < gNumCandidates; i++) {
    if (!gCandidates[i].isEliminated && gCandidates[i].votes <= minVotes) {
      minVotes = gCandidates[i].votes;
    }
  }
  return minVotes;
}

void eliminateCandidate (struct treeNode *t, int e) {
  for (int i = 1; i < gNumCandidates; i++) {
    if (t->path[i] == true) {
      // DO NOTHING
    }
    if (gCandidates->isEliminated == true) {
      // RECURSIVE
      eliminateCandidate(t->branches[i], e);
    }
    else if (gCandidates[i].isEliminated == false) {
      // ADD VOTES
      gCandidates[i].votes += t->numVotes;
    }
  }
}