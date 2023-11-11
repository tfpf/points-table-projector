# Input File Format
The input file must contain keywords describing several aspects of the tournament. Lines in the file may be blank, but
if they contain spaces at the start or end, those spaces are treated as parts of the words written on that line. Hence,
avoid leading and trailing spaces.

A list of keywords follows.

### `points.win`
Must be followed by a space and a number indicating how many points are earned by the winner of a match. Optional;
defaults to 2.

### `points.lose`
Must be followed by a space and a number indicating how many points are earned by the loser of a match. Optional;
defaults to 0.

### `points.other`
Must be followed by a space and a number indicating how many points are earned by each of the two teams if their match
was drawn or tied or similar. Optional; defaults to 1.
