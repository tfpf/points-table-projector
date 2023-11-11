# Input File Format
The input file must contain keywords describing several aspects of the tournament. Lines in the file may be blank, but
if they contain spaces at the start or end, those spaces are treated as parts of the words written on that line. Hence,
avoid leading and trailing spaces.

A list of keywords follows.

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
tied), an equals sign <kbd>=</kbd> must separate them. No spaces must be added.

#### `fixtures.upcoming`
Must be followed by a list of matches which are to be played. Each match must be on a separate line. The end of the
list is indicated by the end of the file. A match is described by the names of the two teams who played, separated by a
comma <kbd>,</kbd>. No spaces must be added.
