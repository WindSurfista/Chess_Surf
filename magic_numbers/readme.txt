Bishop and rook magic numbers obtained by combining the magic numbers from:
https://github.com/OgnianM/Chess-magic-number-generator

Magic numbers are used in the hash function for the magic lookup hash tables. They are obtained through by brute force searching 64 bit numbers.
The relevant occupancy bits are hashed using the hash function and map to the correct corresponding move bitboard for the given square and board occupancy.
A magic number is more efficient the more constructive collisions they create, i.e. only occupancies which map to the same move bitboard collide.
This is done to keep the size of the magic lookup table relatively small.

As generating magic numbers does not use particularly complicated techniques which would tick boxes on the mark scheme and they only really need 
to be generated once and then reused, I decided to avoid writing the generating code for these numbers, instead opting to find valid numbers online to save time.
