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
  struct treeNode *branches[gMaxCandidates];
  int depth;
  int path[gMaxCandidates];
  int numVotes;
};

// Define Variables
int gNumElections;
int gNumCandidates; // Actually used
struct candidate gCandidates [gMaxCandidates];

// Declare Functions I/O
void usage (char *);

// Declare Functions - Candidate evaluation
int isWinner (void); // Done
void printWinner (int); // Done
int trailingVotes (void); // Done

// Declare Functions - Manipulate tree
bool inPath (int, struct treeNode*);
struct treeNode* createBranch (int, struct treeNode*);
void createTree (struct treeNode *); // Done
void printOutput (struct treeNode *);
void printTree (struct treeNode *);

int getNumElections (void);
int getNumCandidates (void);
char *getCandidateName (void);
bool peek (FILE*);
void getVotePath (int[]);
void processVote (int[], struct treeNode*);

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






  
  // Process votes - Fill tree
  gNumElections = getNumElections(); printf ("numElections = %d\n", gNumElections);
  gNumCandidates = getNumCandidates(); printf ("gNumCandidates = %d\n", gNumCandidates);

  for (int i = 1; i < gNumCandidates; i++) {
    gCandidates[i].name = getCandidateName();
    printf ("gCandidates[%d].name = %s\n", i, gCandidates[i].name);
  }

  // Create candidate vote tree
  struct treeNode *root = (struct treeNode*) calloc(1, sizeof(struct treeNode));
  for (int i = 0; i < gMaxCandidates; i++) {
    root->branches[i] = NULL;
    root->path[i] = -1;
  }
  root->depth = 0;
  root->numVotes = 0;
  printf ("root->depth = %d\n", root->depth);



  
  printf ("CREATE TREE\n");
  createTree (root);


  
  printf ("PROCESS VOTE\n");
  int votePath [gNumCandidates];
  while (peek(gInputFile) == false) { 
    getVotePath(votePath);
    processVote (votePath, root);
  }
  
  printf ("PRINT TREE - DEPTH FIRST\n");
  printTree(root);


  


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

bool inPath (int branch, struct treeNode* rt) {
  for (int i = 0; i < rt->depth; i++) {
    if (rt->path[i] == branch) {
      return true;
    }
  }
  return false;
}

struct treeNode* createBranch (int branch, struct treeNode* rt) {
  struct treeNode *returnBranch = (struct treeNode*) calloc(1, sizeof(struct treeNode));
  returnBranch->depth = rt->depth + 1;
  for (int i = 0; i < rt->depth; i++) {
    returnBranch->path[i] = rt->path[i];
  }
  returnBranch->path[rt->depth] = branch;
  return returnBranch;
}


void createTree (struct treeNode * rt) {
  for (int branch = 0; branch < gNumCandidates; branch++) {
    if (inPath (branch, rt) == true) {
      rt->branches[branch] = NULL;
    }
    else {
      rt->branches[branch] = createBranch(branch, rt);
      createTree(rt->branches[branch]);
    }
  }
}
    


void printOutput (struct treeNode * rt) {
  printf ("{ ");
  for (int i = 0; i < rt->depth; i++) {
    printf ("%d ", rt->path[i]);
  }
  printf ("} ");
  printf ("%d\n", rt->numVotes);
}


void printTree (struct treeNode * rt) {
  static int pNum = 0;
  printf ("PT#%d: ", pNum);
  pNum++;
  // Print tree node - {path} votes\n
  printOutput(rt);
  for (int branch = 0; branch < gNumCandidates; branch++) {
    if (rt->branches[branch] == NULL) {
      continue;
    }
    else {
      printTree(rt->branches[branch]);
    }
  }
  printf ("\n");
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

bool peek (FILE* fp) {
  char c = fgetc(fp);
  if (c == '\n') {
    ungetc(c, fp);
    return true;
  }
  else {
    ungetc(c, fp);
    return false;
  }
  return false;
}
  

void getVotePath (int p[]) {
  for (int i = 1; i < gNumCandidates; i++) {
    fscanf (gInputFile, "%d", &p[i]);
  }
}

int trailingVotes (void) {
  int minVotes = INT_MAX;
  for (int i = 1; i < gNumCandidates; i++) {
    if (!gCandidates[i].isEliminated && gCandidates[i].votes <= minVotes) {
      minVotes = gCandidates[i].votes;
    }
  }
  return minVotes;
}


// vp[] =  { na, 1, 2, 3 }
// depth = {  0, 1, 2, 3 }
void processVote (int vp[], struct treeNode *rt) {
  printf ("PV_INIT: ");
  if (rt->depth == 0) {
    printf ("depth=%d: ", rt->depth);
    printf ("PV_ROOT: nextBranch=%d\n", vp[1]); // vp[0] is empty
    processVote (vp, rt->branches[vp[1]]);
  }
  else if (rt->depth  < gNumCandidates - 1) {
    rt->numVotes++;
    printf ("depth=%d: ", rt->depth);
    printf ("PV_BRANCH: nextBranch=%d\n", vp[rt->depth + 1]);
    processVote (vp, rt->branches[vp[rt->depth + 1]]);
  }
  else {
    rt->numVotes++;
    printf ("depth=%d: ", rt->depth);
    printf ("PV_LEAF: \n");
    return;
  }
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