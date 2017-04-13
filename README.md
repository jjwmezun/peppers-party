# Pepper's Party

Some dumb, simply game programmed in C & SDL2 -- mainly as a 'scuse to practice programming in C.

Idea based ( stolen ) on some obscure puzzle game called "Peggy's Party" from some obscure game collection called "Galaxy Games for Girls".


## Progress

So far there's just basic movement & hit detection for solids.

Graphics are all intentionally lazy prototypes, not intended for the final version.


## What remains:

Mainly block interaction:

* Green blocks:
	These you should be able to pick up by pressing down while standing in front o' them, carry them round 'bove you, & then put them down on any empty space in front o' you.
* Orange balls:
	These should roll in a direction you push them, stopping when hitting a solid block or wall, & should obey gravity just like the player.
* Red boxes:
	Touching this should beat the level.
* Black boxes:
	Touching 1 o' these should warp the player to the other.
* Cyan boxes:
	These should disappear after stepping on them & stepping back off, leaving empty space.
	
In addition, I want to add a li'l nuance to the player's movement by allowing the player to turn in place without moving by lightly tapping left or right.


## Author

J. J. W. Mezun


## License

MIT License.

See LICENSE page for mo' info.