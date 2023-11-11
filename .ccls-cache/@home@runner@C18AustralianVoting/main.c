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
struct candidate gCandidates [gMaxCandidates]; // NOT part of tree

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
bool getVotePath (int[]);
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


  
  printf ("GET VOTE PATH\n");
  int votePath [gNumCandidates];
  while (getVotePath(votePath)) { 
    for (int i = 0; i < gNumCandidates; i++) {
      printf ("%d ",votePath[i]);
    }
    printf ("\n");
    printf ("PROCESS VOTE\n");
    processVote (votePath, root);
  }
  
  printf ("PRINT TREE - DEPTH FIRST\n");
  printTree(root);

  printf ("INITIALIZE NUMVOTES\n");
  for (int i = 1; i < gNumCandidates; i++) {
    gCandidates[i].votes = root->branches[i]->numVotes;
  }

  printf ("INITIALIZE BRANCH\n\n\n");
  gCandidates[0].isEliminated = true;

  


  


  // Determine and declare winner
  while (isWinner() == 0) {
    int minVotes = trailingVotes(); // 1 .. < gNumCandidates

    printf ("candidate=votes: ");
    for (int i = 1; i < gNumCandidates; i++) {
      if (gCandidates[i].isEliminated == false) {
        printf ("%s=%d ", gCandidates[i].name, gCandidates[i].votes);
      }
      else {
        printf ("%s=%s ", gCandidates[i].name, "ELIMINATED");
      }
    }
    printf ("\n");

    for (int i = 1; i < gNumCandidates ; i++) {
      printf ("Candidate [%d]: ", i);
      if (gCandidates[i].isEliminated == true) {
        printf ("ELIMINATED\n");
      }
      else if (gCandidates[i].votes == minVotes && gCandidates[i].isEliminated == false) {
        printf ("TRAILING => ELIMINATE\n");
        eliminateCandidate(root->branches[i], i);
        gCandidates[i].isEliminated = true;
      }
      else {
        printf ("NOT TRAILING\n");
      }
    }
  }
  
  printWinner(isWinner());

      

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


// rewrite using fgets and sscanf
bool getVotePath (int p[]) {
  char vpTemp [81];
  fgets (vpTemp, 80, gInputFile);
  
  p[0] = 0;
  int numCopied = sscanf (vpTemp, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d",
    &p[1], &p[2], &p[3], &p[4], &p[5], &p[6], &p[7], &p[8], &p[9], &p[10], 
    &p[11], &p[12], &p[13], &p[14], &p[15], &p[16], &p[17], &p[18], &p[19], &p[20]);

  if (numCopied == 0) {
    printf ("END OF FILE:\n");
    return false;
  }
  else {
    printf ("COPIED LINE: ");
    for (int i = 0; i < numCopied + 1; i++){
      printf ("%d ", p[i]);
    }
    printf ("\n");
    return true;
  }
  
  return false;
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


/*
void eliminateCandidate (struct treeNode *t, int e) {
  printf ("{ ");
  for (int i = 0; i < t->depth; i++) {
    printf ("%d ", t -> path[i]);
  }
  printf ("} ");
  printf ("depth = %d\n", t->depth);

  for (int i = 1; i < gNumCandidates; i++) {
    if (inPath (i, t)) {
      // DO NOTHING
      printf ("  { ");
      for (int i = 0; i < t->depth; i++) {
        printf ("%d ", t->path[i]);
      }
      printf ("%d ", i);
      printf ("} ");
      printf ("   DO NOTHING: ");
      printf ("depth=%d", t->depth);
      printf ("\n");
    }
    else if (gCandidates[i].isEliminated == true) {
      // RECURSIVE
      printf ("  { ");
      for (int i = 0; i < t->depth; i++) {
        printf ("%d ", t -> path[i]);
      }
      printf ("%d ", i);
      printf ("} ");
      printf ("   RECURSIVE: ");
      printf ("depth=%d", t->depth);
      printf ("\n");
      eliminateCandidate(t->branches[i], e);
    }
    else if (gCandidates[i].isEliminated == false) {
      // ADD VOTES
      printf ("  { ");
      for (int i = 0; i < t->depth; i++) {
        printf ("%d ", t->path[i]);
      }
      printf ("%d ", i);
      printf ("} ");
      printf ("   ADD VOTES: from[%s]:to[%s]:%d ", gCandidates[i].name, gCandidates[i].name, t->numVotes);
      printf ("depth=%d", t->depth);
      printf ("\n");
      gCandidates[i].votes += t->branches[i]->numVotes;
    }
  }
  printf ("\n");
}
*/


void eliminateCandidate (struct treeNode *t, int e) {

  for (int i = 1; i < gNumCandidates; i++) {
    if (inPath (i, t)) {
      // DO NOTHING
    }
    else if (gCandidates[i].isEliminated == true) {
      // RECURSIVE
      eliminateCandidate(t->branches[i], e);
    }
    else if (gCandidates[i].isEliminated == false) {
      // ADD VOTES

      gCandidates[i].votes += t->branches[i]->numVotes;
    }
  }
  printf ("\n");
}
