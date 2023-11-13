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
Must be followed by a list of matches which have already been played. Each match must be on a separate line. The end of
the list is indicated by a blank line. A match is described by the names of the two teams who played, separated by a
comma <kbd>,</kbd>. The name of the winning team should be written first. If neither team won (e.g. if the match was
tied), an equals sign <kbd>=</kbd> must separate them. Optional; if not specified, it is assumed that no matches have
been played yet.

If this keyword is used,
* it must appear before `fixtures.upcoming`; and
* `fixtures.results` must not be used.

#### `fixtures.results`
Must be followed by the points table. Each entry of the table must be on a separate line. The end of the table is
indicated by a blank line. An entry consists of the name of team and the points earned by that team, separated by a
space. Optional; if not specified, it is assumed that no matches have been played yet.

If this keyword is used,
* it must appear before `fixtures.upcoming`; and
* `fixtures.completed` must not be used.

#### `fixtures.upcoming`
Must be followed by a list of matches which are to be played. Each match must be on a separate line. The end of the
list is indicated by a blank line or the end of the file. A match is described by the names of the two teams who
played, separated by a comma <kbd>,</kbd>.

## Notes
* Team names are case-sensitive.
* Blank lines are ignored.
  * Remember, however, that the `fixtures.completed` list is terminated with a blank line, as mentioned above.
* Spaces at the start and end of a line are not stripped. They will be treated as parts of the words written on that
  line. This can lead to unexpected behaviour. Hence, spaces must be avoided except as noted above.
* The maximum number of teams supported is 1024.
  * This is not a hard limit. You can increase it simply by changing the number in the constructor of
    `PointsTableProjector`.
* All teams start with zero points.

## Example
Teams Apple, Banana, Guava, Orange and Jackfruit compete in a tournament. Our favourite team is Banana.

Four matches have been played.

1. Apple beat Orange;
1. Guava beat Banana;
1. Guava beat Orange; and
1. Orange beat Banana.

Six matches remain.

1. Apple v/s Guava;
1. Jackfruit v/s Apple;
1. Orange v/s Jackfruit;
1. Guava v/s Jackfruit;
1. Banana v/s Apple; and
1. Jackfruit v/s Banana.

You could write the input file like this:

```
team Banana

fixtures.completed
Apple,Orange
Guava,Banana
Guava,Orange
Orange,Banana

fixtures.upcoming
Apple,Guava
Jackfruit,Apple
Orange,Jackfruit
Guava,Jackfruit
Banana,Apple
Jackfruit,Banana
```

or like this.

```
team Banana

fixtures.results
Apple 4
Orange 2
Guava 4
Banana 0
Jackfruit 0

fixtures.upcoming
Jackfruit,Apple
Banana,Apple
Orange,Jackfruit
Guava,Jackfruit
Jackfruit,Banana
```

The two ways are exactly equivalent.

# Output Format
Assuming Banana win both of their remaining matches, there are eight different scenarios. The program outputs all of
them.

<details>

<summary>Click to view the output.</summary>

```
2
  fixtures.results
    Apple 6
    Banana 4
    Guava 4
    Jackfruit 4
    Orange 2
  fixtures.upcoming
    Apple,Jackfruit
    Banana,Apple
    Jackfruit,Orange
    Jackfruit,Guava
    Banana,Jackfruit
3
  fixtures.results
    Apple 6
    Guava 6
    Banana 4
    Orange 2
    Jackfruit 2
  fixtures.upcoming
    Apple,Jackfruit
    Banana,Apple
    Jackfruit,Orange
    Guava,Jackfruit
    Banana,Jackfruit
2
  fixtures.results
    Apple 6
    Banana 4
    Orange 4
    Guava 4
    Jackfruit 2
  fixtures.upcoming
    Apple,Jackfruit
    Banana,Apple
    Orange,Jackfruit
    Jackfruit,Guava
    Banana,Jackfruit
3
  fixtures.results
    Apple 6
    Guava 6
    Banana 4
    Orange 4
    Jackfruit 0
  fixtures.upcoming
    Apple,Jackfruit
    Banana,Apple
    Orange,Jackfruit
    Guava,Jackfruit
    Banana,Jackfruit
2
  fixtures.results
    Jackfruit 6
    Banana 4
    Apple 4
    Guava 4
    Orange 2
  fixtures.upcoming
    Jackfruit,Apple
    Banana,Apple
    Jackfruit,Orange
    Jackfruit,Guava
    Banana,Jackfruit
2
  fixtures.results
    Guava 6
    Banana 4
    Apple 4
    Jackfruit 4
    Orange 2
  fixtures.upcoming
    Jackfruit,Apple
    Banana,Apple
    Jackfruit,Orange
    Guava,Jackfruit
    Banana,Jackfruit
1
  fixtures.results
    Banana 4
    Apple 4
    Orange 4
    Guava 4
    Jackfruit 4
  fixtures.upcoming
    Jackfruit,Apple
    Banana,Apple
    Orange,Jackfruit
    Jackfruit,Guava
    Banana,Jackfruit
2
  fixtures.results
    Guava 6
    Banana 4
    Apple 4
    Orange 4
    Jackfruit 2
  fixtures.upcoming
    Jackfruit,Apple
    Banana,Apple
    Orange,Jackfruit
    Guava,Jackfruit
    Banana,Jackfruit
```

</details>

In the last (i.e. eighth) scenario, Banana finish at position 2.

The final points table is shown under `fixtures.results`. Banana get 4 points in all. Guava top the table with 6
points. Apple, Orange and Jackfruit end up with 4, 4 and 2 points respectively.

For the tournament to end like this, the events that must transpire are mentioned under `fixtures.upcoming`. (The name
of the winning team is written first.)
* Jackfruit must beat Apple;
* Orange and Guava must beat Jackfruit; and
* Banana must beat Apple and Jackfruit.

Although Banana, Apple and Orange have the same points at the end, Banana is placed highest among them because the
program is written to calculate the _best_ outcome for our favourite team.

The other scenarios are interpreted similarly. Do check out the seventh scenario, in which Banana end up as toppers.
