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
Must be followed by a space and the name of the team whose possible outcomes you'd like to see.

#### `fixtures.completed`
Must be followed by a list of matches which have already been played. Each match must be on a separate line. The end of
the list is indicated by a blank line. A match is described by the names of the two teams who played, separated by a
comma <kbd>,</kbd>. The name of the winning team should be written first. If neither team won (e.g. if the match was
tied), an equals sign <kbd>=</kbd> must separate them.

#### `fixtures.upcoming`
Must be followed by a list of matches which are to be played. Each match must be on a separate line. The end of the
list is indicated by the end of the file. A match is described by the names of the two teams who played, separated by a
comma <kbd>,</kbd>.

## Notes
* Team names are case-sensitive.
* Blank lines are ignored.
  * Remember, however, that the `fixtures.completed` list is terminated with a blank line, as mentioned above.
* Spaces at the start and end of a line are not stripped. They will be treated as parts of the words written on that
  line. This can lead to unexpected behaviour. Hence, spaces must be avoided except as noted above.

## Example
Teams Apple, Banana, Guava, Orange and Jackfruit compete in a tournament. Our favourite team is Banana.

Four matches have been played.

1. Apple beat Orange;
1. Guava beat Banana;
1. Guava beat Orange; and
1. Oange beat Banana.

Six matches remain.

1. Apple v/s Guava;
1. Jackfruit v/s Apple;
1. Orange v/s Jackfruit;
1. Guava v/s Jackfruit;
1. Banana v/s Apple; and
1. Jackfruit v/s Banana.

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

# Output Format
