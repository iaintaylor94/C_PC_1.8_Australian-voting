# Programming Challenges
## Problem 1.8: Australian voting[^1]

### Problem description
This program is used to identify the winning party of the Australian election. Each voter aranges the order of the candidates from favourite to least favourite. Initially only the favourite candidates are used to determine the winning party. If a candidate has more than 50% of the vote they are declared the winner, otherwise the least popular party is eliminated and their votes redistributed to the next most popular candidate. The votes are recalculated and tested to determine whether their is now a winner, this process repeats itself until their is a winner or the candidates are tied.


### Example 
> **Input**
> 1
>
> 3
> John Doe
> Jane Smith
> Jane Austen
> 1 2 3
> 2 1 3
> 2 3 1
> 1 2 3
> 3 1 2           

>**Output**
>John Doe       


[^1]: Programming Challenges By S.Skienna and M.Revilla, page 25