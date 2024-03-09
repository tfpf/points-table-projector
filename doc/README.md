# Input File Format
The input file must contain keywords describing several aspects of the tournament.

## Keywords
#### `points.win`
Must be followed by a space and a number indicating how many points are earned by the winner of a match. Optional;
defaults to 2.

#### `points.lose`
Must be followed by a space and a number indicating how many points are earned by the loser of a match. Optional;
defaults to 0.

#### `points.other`
Must be followed by a space and a number indicating how many points are earned by each of the two teams if their match
was drawn or tied or similar. Optional; defaults to 1.

#### `team`
Must be followed by a space and the name of your favourite team (i.e. the team whose possible outcomes you'd like to
see).

#### `fixtures.completed`
Must be followed by a list of matches which have already been played. (The end of the list is indicated by a blank
line.) Each match must be on a separate line. A match is described by the names of the two teams who played, separated
by a comma <kbd>,</kbd>. The name of the winning team should be written first. If neither team won (e.g. if the match
was tied), an equals sign <kbd>=</kbd> must separate them. Optional; if not specified, it is assumed that no matches
have been played yet.

If this keyword is used,
* it must appear before `fixtures.upcoming`; and
* `fixtures.results` must not be used.

#### `fixtures.results`
Must be followed by the points table. (The end of the table is indicated by a blank line.) Each entry of the table must
be on a separate line. An entry consists of the name of team and the points earned by that team, separated by a space.
Optional; if not specified, it is assumed that no matches have been played yet.

If this keyword is used,
* it must appear before `fixtures.upcoming`; and
* `fixtures.completed` must not be used.

#### `fixtures.upcoming`
Must be followed by a list of matches which are to be played. Each match must be on a separate line. The end of the
list is indicated by a blank line or the end of the file. A match is described by the names of the two teams who will
play, separated by a comma <kbd>,</kbd>.

## Notes
* Team names are case-sensitive.
* Blank lines are ignored.
  * Remember, however, that the `fixtures.completed` list and `fixtures.results` table are terminated with a blank
    line, as mentioned above.
* Spaces at the start and end of a line are not stripped. They will be treated as parts of the words written on that
  line. This can lead to unexpected behaviour. Hence, spaces must be avoided except as noted above.
* The maximum number of teams supported is 1024.
  * This is not a hard limit. You can increase it simply by changing the number in the constructor of
    `PointsTableProjector`.
* All teams start with zero points.

## Example
Teams Apple, Banana, Guava, Jackfruit and Orange compete in a tournament. Our favourite team is Banana.

Six matches have been played.

1. Apple beat Orange;
1. Jackfruit beat Guava;
1. Apple beat Jackfruit;
1. Banana tied with Guava;
1. Orange beat Guava; and
1. Apple beat Banana.

Four matches remain.

1. Orange v/s Banana;
1. Apple v/s Guava;
1. Orange v/s Jackfruit; and
1. Jackfruit v/s Banana.

You could write the input file like this:

```
team Banana

fixtures.completed
Apple,Orange
Jackfruit,Guava
Apple,Jackfruit
Banana=Guava
Orange,Guava
Apple,Banana

fixtures.upcoming
Orange,Banana
Apple,Guava
Orange,Jackfruit
Jackfruit,Banana
```

or like this.

```
team Banana

fixtures.results
Apple 6
Orange 2
Guava 1
Banana 1
Jackfruit 2

fixtures.upcoming
Orange,Banana
Apple,Guava
Orange,Jackfruit
Jackfruit,Banana
```

The two ways are equivalent.

# Processing
## Favouritism
The program assumes that our favourite team will win all their remaining matches.

## Pruning
If any match is found to be inconsequential, the program picks its result randomly (instead of exploring two
possibilities, one in which the first team wins and the other in which the second team wins). A match is said to be
inconsequential if it is played between two inconsequential teams. A team is said to be inconsequential if,
irrespective of what happens, it always ends up with more or fewer points than our favourite team.

# Output Format
Ordinarily, since there are four matches left, there would have been 2<sup>4</sup> = 16 different scenarios. However,
because of favouritism, it is assumed that Banana will win both of their remaining matches. Also, note that Banana
cannot earn more points than Apple, and Guava cannot earn more points than Banana. This means that the match between
Apple and Guava is inconsequential, so its result is picked randomly. Hence, only 2 scenarios are output.

<details>

<summary>Click to view possible output.</summary>

```
2
  fixtures.results
    Apple 6
    Banana 5
    Jackfruit 4
    Guava 3
    Orange 2
  fixtures.upcoming
    Banana,Orange
    Guava,Apple
    Jackfruit,Orange
    Banana,Jackfruit
2
  fixtures.results
    Apple 6
    Banana 5
    Orange 4
    Guava 3
    Jackfruit 2
  fixtures.upcoming
    Banana,Orange
    Guava,Apple
    Orange,Jackfruit
    Banana,Jackfruit
```

</details>

In the first scenario, Banana finish at position 2.

The final points table is shown under `fixtures.results`. Banana get 5 points in all. Apple top the table with 6
points. Jackfruit, Guava and Orange end up with 4, 3 and 2 points respectively.

For the tournament to end like this, the events that must transpire are mentioned under `fixtures.upcoming`. (The name
of the winning team is written first.)
* Banana must beat Orange and Jackfruit;
* Guava must beat Apple; and
* Jackfruit must beat Orange.

If multiple teams had earned the same points as Banana, Banana would have been placed highest among them, because the
program is written to calculate the _best_ outcome for our favourite team.

The second scenario is interpreted similarly.
